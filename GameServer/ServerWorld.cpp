#include "ServerWorld.h"

#include "../MyEngine_Source/FSMFactory.h"
#include "../MyEngine_Source/FSMBrainCore.h"
#include "../MyEngine_Source/IFSMContext.h"
#include "VideoLog.h"

#include <algorithm>
#include <cmath>
#include <limits>
#include <utility>
#include <vector>

#include <chrono>
#include <iostream>
#include <thread>


namespace
{
    bool IsFinite(float value)
    {
        return std::isfinite(value);
    }

    bool IsValidPosition(const ServerVec3& position)
    {
        return IsFinite(position.x) && IsFinite(position.y) && IsFinite(position.z);
    }

    bool IsValidWeaponType(eWeaponType weaponType)
    {
        switch (weaponType)
        {
        case eWeaponType::Gun:
        case eWeaponType::Sword:
            return true;

        default:
            return false;
        }
    }

    bool IsFiniteVec3(const ServerVec3& value)
    {
        return std::isfinite(value.x) && std::isfinite(value.y) && std::isfinite(value.z);
    }

    bool IsValidAttackIndex(eWeaponType weaponType, std::uint8_t attackIndex)
    {
        switch (weaponType)
        {
        case eWeaponType::Gun:
            return attackIndex == 0;

        case eWeaponType::Sword:
            return attackIndex < 3;

        default:
            return false;
        }
    }

    float LengthSquared(const ServerVec3& value)
    {
        return value.x * value.x + value.y * value.y + value.z * value.z;
    }

    bool Normalize(ServerVec3& value)
    {
        const float lengthSquared = LengthSquared(value);

        if (!std::isfinite(lengthSquared) || lengthSquared < 0.000001f)
        {
            return false;
        }

        const float inverseLength = 1.0f / std::sqrt(lengthSquared);
        value.x *= inverseLength;
        value.y *= inverseLength;
        value.z *= inverseLength;

        return true;
    }

    bool NormalizeXZ(ServerVec3& value)
    {
        value.y = 0.0f;
        const float lengthSquared = value.x * value.x + value.z * value.z;

        if (!std::isfinite(lengthSquared) || lengthSquared < 0.000001f)
        {
            return false;
        }

        const float inverseLength = 1.0f / std::sqrt(lengthSquared);
        value.x *= inverseLength;
        value.z *= inverseLength;

        return true;
    }

    ServerVec3 Add(const ServerVec3& lhs, const ServerVec3& rhs)
    {
        return { lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z };
    }

    ServerVec3 Multiply(const ServerVec3& value, float scalar)
    {
        return { value.x * scalar, value.y * scalar, value.z * scalar };
    }

    ServerVec3 PointOnSegment(const ServerVec3& start, const ServerVec3& end, float t)
    {
        return { start.x + (end.x - start.x) * t, start.y + (end.y - start.y) * t, start.z + (end.z - start.z) * t };
    }

    bool TestSegmentAxis(float start, float delta, float boxMin, float boxMax, float& inOutMinT, float& inOutMaxT)
    {
        constexpr float Epsilon = 0.000001f;

        if (std::abs(delta) < Epsilon)
        {
            return start >= boxMin && start <= boxMax;
        }

        const float inverseDelta = 1.0f / delta;
        float t1 = (boxMin - start) * inverseDelta;
        float t2 = (boxMax - start) * inverseDelta;

        if (t1 > t2)
        {
            std::swap(t1, t2);
        }

        inOutMinT = (std::max)(inOutMinT, t1);
        inOutMaxT = (std::min)(inOutMaxT, t2);

        return inOutMinT <= inOutMaxT;
    }

    // 총알 또는 근접공격을 선분 + 반경으로 검사
    // 대상 AABB를 반경만큼 확장하는 방식
    bool SegmentIntersectsAabb(const ServerVec3& start, const ServerVec3& end, const ServerAabb& box, float sweepRadius, float& outHitT)
    {
        const ServerVec3 expandedHalfExtent = {
            box.halfExtent.x + sweepRadius, box.halfExtent.y + sweepRadius, box.halfExtent.z + sweepRadius };
        const ServerVec3 boxMin = {
            box.center.x - expandedHalfExtent.x, box.center.y - expandedHalfExtent.y, box.center.z - expandedHalfExtent.z };
        const ServerVec3 boxMax = {
            box.center.x + expandedHalfExtent.x, box.center.y + expandedHalfExtent.y, box.center.z + expandedHalfExtent.z };
        const ServerVec3 delta = { end.x - start.x, end.y - start.y, end.z - start.z };
        float minT = 0.0f;
        float maxT = 1.0f;

        if (!TestSegmentAxis(start.x, delta.x, boxMin.x, boxMax.x, minT, maxT))
        {
            return false;
        }

        if (!TestSegmentAxis(start.y, delta.y, boxMin.y, boxMax.y, minT, maxT))
        {
            return false;
        }

        if (!TestSegmentAxis(start.z, delta.z, boxMin.z, boxMax.z, minT, maxT))
        {
            return false;
        }

        outHitT = minT;

        return true;
    }

    struct SwordAttackConfig
    {
        float duration = 0.85f;
        float hitNormalizedTime = 0.5f;
        float damage = 15.0f;
        float reach = 180.0f;
        float radius = 20.0f;
        float cooldown = 0.45f;
    };

    bool GetSwordAttackConfig(std::uint8_t attackIndex, SwordAttackConfig& outConfig)
    {
        switch (attackIndex)
        {
        case 0:
            outConfig = { 0.85f, 0.50f, 15.0f, 180.0f, 20.0f, 0.45f };
            return true;

        case 1:
            outConfig = { 0.95f, 0.5f, 18.0f, 190.0f, 20.0f, 0.50f };
            return true;

        case 2:
            outConfig = { 1.10f, 0.5f, 25.0f, 210.0f, 20.0f, 0.65f };
            return true;

        default:
            return false;
        }
    }

    Pkt_S_Enter MakeEnterPacket(const ServerPlayer& player)
    {
        Pkt_S_Enter packet = {};
        packet.header.type = ePacketType::S_ENTER;
        packet.header.size = sizeof(Pkt_S_Enter);
        packet.entityId = player.entityId;
        packet.modelType = player.modelType;
        packet.weaponType = player.weaponType;
        packet.state = player.state;
        packet.x = player.position.x;
        packet.y = player.position.y;
        packet.z = player.position.z;
        packet.yaw = player.yaw;

        return packet;
    }

    Pkt_S_Move MakeMovePacket(const ServerPlayer& player)
    {
        Pkt_S_Move packet = {};
        packet.header.type = ePacketType::S_MOVE;
        packet.header.size = sizeof(Pkt_S_Move);
        packet.entityId = player.entityId;
        packet.x = player.position.x;
        packet.y = player.position.y;
        packet.z = player.position.z;
        packet.yaw = player.yaw;

        return packet;
    }

    Pkt_S_State MakeStatePacket(const ServerPlayer& player)
    {
        Pkt_S_State packet = {};
        packet.header.type = ePacketType::S_STATE;
        packet.header.size = sizeof(Pkt_S_State);
        packet.entityId = player.entityId;
        packet.state = player.state;

        return packet;
    }

    Pkt_S_WeaponChange MakeWeaponChangePacket(const ServerPlayer& player)
    {
        Pkt_S_WeaponChange packet = {};
        packet.header.type = ePacketType::S_WEAPON_CHANGE;
        packet.header.size = sizeof(Pkt_S_WeaponChange);
        packet.entityId = player.entityId;
        packet.weaponType = player.weaponType;

        return packet;
    }

    Pkt_S_Attack MakeAttackPacket(const ServerPlayer& player, const AttackCommand& command)
    {
        Pkt_S_Attack packet = {};
        packet.header.type = ePacketType::S_ATTACK;
        packet.header.size = sizeof(Pkt_S_Attack);
        packet.entityId = player.entityId;
        packet.weaponType = player.weaponType;
        packet.attackIndex = command.attackIndex;
        packet.dir_x = command.direction.x;
        packet.dir_y = command.direction.y;
        packet.dir_z = command.direction.z;

        return packet;
    }

    Pkt_S_Leave MakeLeavePacket(EntityId entityId)
    {
        Pkt_S_Leave packet = {};
        packet.header.type = ePacketType::S_LEAVE;
        packet.header.size = sizeof(Pkt_S_Leave);
        packet.entityId = entityId;

        return packet;
    }

    Pkt_S_MonsterSpawn MakeMonsterSpawnPacket(const ServerMonster& monster)
    {
        Pkt_S_MonsterSpawn packet = {};
        packet.header.type = ePacketType::S_MONSTER_SPAWN;
        packet.header.size = sizeof(Pkt_S_MonsterSpawn);
        packet.entityId = monster.entityId;
        packet.modelType = monster.modelType;
        packet.weaponType = monster.weaponType;
        packet.state = monster.state;
        packet.x = monster.position.x;
        packet.y = monster.position.y;
        packet.z = monster.position.z;
        packet.yaw = monster.yaw;
        packet.hp = monster.hp;
        packet.maxHp = monster.maxHp;

        return packet;
    }

    Pkt_S_MonsterMove MakeMonsterMovePacket(const ServerMonster& monster)
    {
        Pkt_S_MonsterMove packet = {};
        packet.header.type = ePacketType::S_MONSTER_MOVE;
        packet.header.size = sizeof(Pkt_S_MonsterMove);
        packet.entityId = monster.entityId;
        packet.x = monster.position.x;
        packet.y = monster.position.y;
        packet.z = monster.position.z;
        packet.yaw = monster.yaw;

        return packet;
    }

    Pkt_S_MonsterState MakeMonsterStatePacket(const ServerMonster& monster)
    {
        Pkt_S_MonsterState packet = {};
        packet.header.type = ePacketType::S_MONSTER_STATE;
        packet.header.size = sizeof(Pkt_S_MonsterState);
        packet.entityId = monster.entityId;
        packet.state = monster.state;

        return packet;
    }

    Pkt_S_MonsterAttack MakeMonsterAttackPacket(const ServerMonster& monster)
    {
        Pkt_S_MonsterAttack packet = {};
        packet.header.type = ePacketType::S_MONSTER_ATTACK;
        packet.header.size = sizeof(Pkt_S_MonsterAttack);
        packet.entityId = monster.entityId;
        packet.targetEntityId = monster.attackTargetId;
        packet.attackIndex = monster.attackIndex;
        packet.dir_x = monster.attackDirection.x;
        packet.dir_y = monster.attackDirection.y;
        packet.dir_z = monster.attackDirection.z;

        return packet;
    }

    Pkt_S_MonsterDespawn MakeMonsterDespawnPacket(EntityId entityId)
    {
        Pkt_S_MonsterDespawn packet = {};
        packet.header.type = ePacketType::S_MONSTER_DESPAWN;
        packet.header.size = sizeof(Pkt_S_MonsterDespawn);
        packet.entityId = entityId;

        return packet;
    }
}

void ServerWorld::SetNetworkCallbacks(SendToCallback sendTo, BroadcastExceptCallback broadcastExcept, MarkEnteredCallback markEntered)
{
    mSendToCallback = std::move(sendTo);
    mBroadcastExceptCallback = std::move(broadcastExcept);
    mMarkEnteredCallback = std::move(markEntered);
}

void ServerWorld::SetAllocateEntityIdCallback(AllocateEntityIdCallback allocator)
{
    mAllocateEntityIdCallback = std::move(allocator);
}

void ServerWorld::EnsureWorldInitialization()
{
    if (mbInitialized.load())
        return;

    mbInitialized.store(true);
    InitializeMonsterAnimationMeta();
    SpawnMonster(eModelType::Mutant, eWeaponType::Gauntlet, { 1500.0f, 0.0f, 0.0f }, 0, true);
}

void ServerWorld::EnqueueCommand(WorldCommand command)
{
    std::lock_guard<std::mutex> lock(mCommandMutex);
    mCommandQueue.push(std::move(command));
}

void ServerWorld::Run()
{
    if (mbRunning.exchange(true))
    {
        // 이미 실행 중
        return;
    }

    using Clock = std::chrono::steady_clock;
    constexpr float FixedDeltaTime = 1.0f / 60.0f;
    constexpr auto TickDuration = std::chrono::microseconds(16667);
    auto nextTick = Clock::now();

    while (mbRunning.load())
    {
        nextTick += TickDuration;
        ProcessCommands();
        Tick(FixedDeltaTime);
        const auto now = Clock::now();

        if (now < nextTick)
        {
            std::this_thread::sleep_until(nextTick);
        }
        else
        {
            // 서버 처리가 너무 늦어진 경우 무한 따라잡기 방지
            nextTick = now;
        }
    }
}

void ServerWorld::Stop()
{
    mbRunning.store(false);
}

void ServerWorld::ProcessCommands()
{
    std::queue<WorldCommand> localCommands;
    {
        std::lock_guard<std::mutex> lock(mCommandMutex);

        // 잠금 상태에서 실제 처리하지 않고
        // 로컬 큐로 한 번에 옮긴다.
        std::swap(localCommands, mCommandQueue);
    }

    while (!localCommands.empty())
    {
        WorldCommand command = std::move(localCommands.front());
        localCommands.pop();
        std::visit(
            [this](const auto& concreteCommand)
            {
                HandleCommand(concreteCommand);
            },
            command);
    }
}

void ServerWorld::Tick(float deltaTime)
{
    for (auto& [playerId, player] : mPlayers)
    {
        player.attackCooldown = (std::max)(0.0f, player.attackCooldown - deltaTime);
    }

    UpdatePlayerMeleeAttacks(deltaTime);

    // 몬스터 non-loop 애니메이션 진행 및
    // 근접 공격 타격 시점
    for (auto& [monsterId, monster] : mMonsters)
    {
        if (monster.actionDuration <= 0.0f)
        {
            continue;
        }

        monster.actionElapsedTime += deltaTime;

        if (monster.actionElapsedTime > monster.actionDuration)
        {
            monster.actionElapsedTime = monster.actionDuration;
        }

        const float normalizedTime = monster.actionElapsedTime / monster.actionDuration;

        if (monster.actionIsAttack && !monster.attackHitProcessed && normalizedTime >= monster.attackHitNormalizedTime)
        {
            monster.attackHitProcessed = true;
            ResolveMonsterMeleeAttack(monster);
        }
    }

    UpdateMonsters(deltaTime);
    UpdateProjectiles(deltaTime);
    FlushMonsterReplication(deltaTime);
    DespawnRequestedMonsters();
}

void ServerWorld::HandleCommand(const EnterCommand& command)
{
    if (command.entityId == 0)
        return;

    if (!IsValidPosition(command.position))
        return;

    if (!IsValidWeaponType(command.weaponType))
        return;

    if (mPlayers.find(command.entityId) != mPlayers.end())
    {
        // 중복 C_ENTER 방지
        return;
    }

    ServerPlayer newPlayer = {};
    newPlayer.entityId = command.entityId;
    newPlayer.modelType = command.modelType;
    newPlayer.weaponType = command.weaponType;
    newPlayer.state = ePlayerState::IDLE;
    newPlayer.position = command.position;
    newPlayer.yaw = command.yaw;
    EnsureWorldInitialization();

    for (const auto& [existingId, existingPlayer] : mPlayers) //새 클라이언트 들에게 새로 들어온 애 알림
    {
        Pkt_S_Enter packet = MakeEnterPacket(existingPlayer);
        SendTo(command.entityId, packet);
    }

    SendMonsterSnapshotTo(newPlayer.entityId); // 새 클라이언트에게 현재 몬스터 스냅샷
    VideoLog::Print("[SNAPSHOT] To=", newPlayer.entityId, " | ExistingPlayers=", mPlayers.size(), " | Monsters=", mMonsters.size());
    mPlayers.emplace(newPlayer.entityId, newPlayer); // 새로운 클라이언트 서버 월드 맵에 등록
    VideoLog::Print("[WORLD] Player Enter"
        " | Id=",
        newPlayer.entityId, " | TotalPlayers=", mPlayers.size());

    if (mMarkEnteredCallback)
    {
        mMarkEnteredCallback(command.entityId, true);
    }

    Pkt_S_Enter newPlayerPacket = MakeEnterPacket(newPlayer);
    BroadcastExcept(command.entityId, newPlayerPacket);

    //std::cout
    //    << "[World] 플레이어 입장 id: "
    //    << command.entityId
    //    << " / 현재 인원: "
    //    << mPlayers.size()
    //    << '\n';
}

void ServerWorld::HandleCommand(const LeaveCommand& command)
{
    auto iter = mPlayers.find(command.entityId);

    if (iter == mPlayers.end()) //이미 존재 안하는 클라이언트면 삭제
        return;

    mPlayers.erase(iter);
    Pkt_S_Leave packet = MakeLeavePacket(command.entityId);
    BroadcastExcept(command.entityId, packet);
    std::cout << "[World] 플레이어 퇴장 id: " << command.entityId << " / 현재 인원: " << mPlayers.size() << '\n';
}

void ServerWorld::HandleCommand(const MoveCommand& command)
{
    if (!IsValidPosition(command.position))
        return;

    if (!IsFinite(command.yaw))
        return;

    auto iter = mPlayers.find(command.entityId);

    if (iter == mPlayers.end())
        return;

    ServerPlayer& player = iter->second;

    if (!player.alive)
        return;

    player.position = command.position;
    player.yaw = command.yaw;
    Pkt_S_Move packet = MakeMovePacket(player);
    BroadcastExcept(player.entityId, packet);
}

void ServerWorld::HandleCommand(const StateCommand& command)
{
    if (command.state != ePlayerState::IDLE && command.state != ePlayerState::WALK) //hit, death는 서버데미지 판정쪽에서
    {
        return;
    }

    auto iter = mPlayers.find(command.entityId);

    if (iter == mPlayers.end())
        return;

    ServerPlayer& player = iter->second;

    if (!player.alive)
        return;

    if (player.state == command.state)
        return;

    player.state = command.state;
    Pkt_S_State packet = MakeStatePacket(player);
    BroadcastExcept(player.entityId, packet);
}

void ServerWorld::HandleCommand(const WeaponChangeCommand& command)
{
    if (!IsValidWeaponType(command.weaponType))
        return;

    auto iter = mPlayers.find(command.entityId);

    if (iter == mPlayers.end())
        return;

    ServerPlayer& player = iter->second;

    if (!player.alive)
        return;

    if (player.weaponType == command.weaponType)
        return;

    player.weaponType = command.weaponType;
    Pkt_S_WeaponChange packet = MakeWeaponChangePacket(player);
    BroadcastExcept(player.entityId, packet);
}

void ServerWorld::HandleCommand(const AttackCommand& command)
{
    auto playerIter = mPlayers.find(command.entityId);

    if (playerIter == mPlayers.end())
        return;

    ServerPlayer& player = playerIter->second;

    if (!player.alive)
        return;

    if (player.attackCooldown > 0.0f)
        return;

    if (!IsValidAttackIndex(player.weaponType, command.attackIndex))
    {
        return;
    }

    ServerVec3 direction = command.direction;

    if (player.weaponType == eWeaponType::Sword)
    {
        if (!NormalizeXZ(direction))
            return;
    }
    else
    {
        if (!Normalize(direction))
            return;
    }

    AttackCommand normalizedCommand = command;
    normalizedCommand.direction = direction;

    if (player.weaponType == eWeaponType::Gun)
    {
        player.attackCooldown = 0.2f;
        SpawnProjectile(player, normalizedCommand);
    }
    else if (player.weaponType == eWeaponType::Sword)
    {
        if (!BeginPlayerMeleeAttack(player, normalizedCommand))
        {
            return;
        }
    }
    else
    {
        return;
    }

    VideoLog::Print("[COMMAND] Processed"
        " | Type=Attack"
        " | Entity=",
        player.entityId);

    // 다른 클라이언트에서 공격 애니메이션 재생
    const Pkt_S_Attack attackPacket = MakeAttackPacket(player, normalizedCommand);
    BroadcastExcept(player.entityId, attackPacket);
}

void ServerWorld::UpdateMonsters(float deltaTime)
{
    for (auto& [monsterId, monster] : mMonsters)
    {
        auto brainIter = mMonsterBrains.find(monsterId);

        if (brainIter == mMonsterBrains.end())
        {
            continue;
        }

        // 이번 Tick에만 유효한 Context
        ServerMonsterFSMContext context(*this, monster, deltaTime);

        if (!monster.alive && monster.state != eMonsterState::DEATH)
        {
            // 현재 Update 바깥이므로
            // pending 상태로 들어간다
            brainIter->second->SendFSMEvent("DEATH");
        }

        const std::string beforeState = brainIter->second->GetActiveStateName();
        brainIter->second->Update(context);
        const std::string afterState = brainIter->second->GetActiveStateName();

        if (beforeState != afterState)
        {
            VideoLog::Print("[FSM] Monster=", monsterId, " | ", beforeState, " -> ", afterState, " | Target=", monster.targetPlayerId);
        }
    }
}

void ServerWorld::UpdateProjectiles(float deltaTime)
{
    std::vector<ProjectileId> removeProjectiles;

    for (auto& [projectileId, projectile] : mProjectiles)
    {
        const ServerVec3 start = projectile.position;
        const ServerVec3 end = { projectile.position.x + projectile.velocity.x * deltaTime,
            projectile.position.y + projectile.velocity.y * deltaTime, projectile.position.z + projectile.velocity.z * deltaTime };
        projectile.previousPosition = start;
        ProjectileHitResult hit = {};

        if (FindClosestProjectileHit( //가까운 플레이어와 충돌한 경우
            projectile, start, end, hit))
        {
            projectile.position = hit.hitPosition;
            eProjectileEndReason endReason = eProjectileEndReason::HitWorld; //벽이나 기둥같은거

            if (hit.kind == eServerHitKind::Player)
            {
                endReason = eProjectileEndReason::HitPlayer;
            }
            else if (hit.kind == eServerHitKind::Monster)
            {
                endReason = eProjectileEndReason::HitMonster;
            }

            VideoLog::Print("[PROJECTILE] Hit"
                " | Id=",
                projectile.projectileId, " | Target=", hit.entityId, " | HitT=", hit.hitT);
            BroadcastProjectileEnd(projectile, hit, endReason);

            if (hit.kind == eServerHitKind::Player || hit.kind == eServerHitKind::Monster)
            {
                ApplyServerDamage(eDamageCause::Projectile, projectile.ownerEntityId, hit.entityId, projectile.projectileId,
                    projectile.damage, hit.hitPosition);
            }

            removeProjectiles.push_back(projectileId);
            continue;
        }

        projectile.position = end;
        projectile.remainingLife -= deltaTime;

        if (projectile.remainingLife <= 0.0f)
        {
            ProjectileHitResult expired = {};
            expired.hitPosition = projectile.position;
            BroadcastProjectileEnd(projectile, expired, eProjectileEndReason::Expired);
            removeProjectiles.push_back(projectileId);
        }
    }

    for (ProjectileId projectileId : removeProjectiles)
    {
        mProjectiles.erase(projectileId);
    }
}

void ServerWorld::SpawnProjectile(const ServerPlayer& player, const AttackCommand& command)
{
    constexpr float BulletSpeed = 1000.0f;
    constexpr float BulletLifeTime = 8.0f;
    ServerVec3 spawnPosition = command.origin;

    if (!IsValidProjectileOrigin(player, spawnPosition))
    {
        constexpr float MuzzleHeight = 100.0f;
        constexpr float ForwardOffset = 60.0f;
        spawnPosition = { player.position.x + command.direction.x * ForwardOffset,
            player.position.y + MuzzleHeight + command.direction.y * ForwardOffset,
            player.position.z + command.direction.z * ForwardOffset };
    }

    ServerProjectile projectile = {};
    projectile.projectileId = mNextProjectileId++;

    if (mNextProjectileId == 0)
    {
        mNextProjectileId = 1;
    }

    projectile.ownerEntityId = player.entityId;
    projectile.previousPosition = spawnPosition;
    projectile.position = spawnPosition;
    projectile.velocity = { command.direction.x * BulletSpeed, command.direction.y * BulletSpeed, command.direction.z * BulletSpeed };
    projectile.radius = 3.0f;
    projectile.damage = 5.0f;
    projectile.remainingLife = BulletLifeTime;
    mProjectiles.emplace(projectile.projectileId, projectile);
    VideoLog::Print("[PROJECTILE] Spawn"
        " | Id=",
        projectile.projectileId, " | Owner=", projectile.ownerEntityId);
    Pkt_S_ProjectileSpawn packet = {};
    packet.header.type = ePacketType::S_PROJECTILE_SPAWN;
    packet.header.size = sizeof(Pkt_S_ProjectileSpawn);
    packet.projectileId = projectile.projectileId;
    packet.ownerEntityId = projectile.ownerEntityId;
    packet.start_x = projectile.position.x;
    packet.start_y = projectile.position.y;
    packet.start_z = projectile.position.z;
    packet.velocity_x = projectile.velocity.x;
    packet.velocity_y = projectile.velocity.y;
    packet.velocity_z = projectile.velocity.z;
    packet.lifeTime = projectile.remainingLife;

    // 발사자 포함 모두에게 전송
    BroadcastExcept(0, packet);
}

void ServerWorld::BroadcastProjectileEnd(const ServerProjectile& projectile, const ProjectileHitResult& hit, eProjectileEndReason reason)
{
    Pkt_S_ProjectileEnd packet = {};
    packet.header.type = ePacketType::S_PROJECTILE_END;
    packet.header.size = sizeof(Pkt_S_ProjectileEnd);
    packet.projectileId = projectile.projectileId;
    packet.reason = reason;
    packet.hitEntityId = hit.entityId;
    packet.end_x = hit.hitPosition.x;
    packet.end_y = hit.hitPosition.y;
    packet.end_z = hit.hitPosition.z;
    BroadcastExcept(0, packet);
}

void ServerWorld::EndProjectile(ProjectileId projectileId, eProjectileEndReason reason, EntityId hitEntityId, const ServerVec3& endPosition)
{
}

bool ServerWorld::FindClosestProjectileHit(
    const ServerProjectile& projectile, const ServerVec3& start, const ServerVec3& end, ProjectileHitResult& outHit) const
{
    outHit = {};
    float closestT = 1.0f;
    auto testTarget = [&](const ServerAabb& box, eServerHitKind kind, EntityId entityId)
        {
            float hitT = 0.0f;

            if (!SegmentIntersectsAabb(start, end, box, projectile.radius, hitT))
            {
                return;
            }

            if (hitT >= closestT)
                return;

            closestT = hitT;
            outHit.hit = true;
            outHit.hitT = hitT;
            outHit.kind = kind;
            outHit.entityId = entityId;
            outHit.hitPosition = PointOnSegment(start, end, hitT);
        };

    // 벽, 기둥 등의 정적 Proxy
    for (const ServerStaticCollider& collider : mStaticWorldColliders)
    {
        testTarget(collider.bounds, eServerHitKind::World, 0);
    }

    const bool ownerIsPlayer = mPlayers.find(projectile.ownerEntityId) != mPlayers.end();
    const bool ownerIsMonster = mMonsters.find(projectile.ownerEntityId) != mMonsters.end();

    if (ownerIsPlayer)
    {
        // 플레이어 총알은 몬스터를 맞힌다
        for (const auto& [monsterId, monster] : mMonsters)
        {
            if (!monster.alive)
                continue;

            testTarget(MakeMonsterAabb(monster), eServerHitKind::Monster, monsterId);
        }

        if (mbFriendlyFire)
        {
            for (const auto& [playerId, player] : mPlayers)
            {
                if (!player.alive || playerId == projectile.ownerEntityId)
                {
                    continue;
                }

                testTarget(MakePlayerAabb(player), eServerHitKind::Player, playerId);
            }
        }
    }
    else if (ownerIsMonster)
    {
        // 몬스터 투사체는 플레이어를 맞힌다
        for (const auto& [playerId, player] : mPlayers)
        {
            if (!player.alive)
                continue;

            testTarget(MakePlayerAabb(player), eServerHitKind::Player, playerId);
        }
    }

    return outHit.hit;
}

bool ServerWorld::IsValidProjectileOrigin(const ServerPlayer& player, const ServerVec3& origin) const
{
    if (!IsFiniteVec3(origin))
        return false;

    const float dx = origin.x - player.position.x;
    const float dy = origin.y - player.position.y;
    const float dz = origin.z - player.position.z;
    const float distanceSquared = dx * dx + dy * dy + dz * dz;
    constexpr float MaxMuzzleDistance = 350.0f;

    return distanceSquared <= MaxMuzzleDistance * MaxMuzzleDistance;
}

ServerAabb ServerWorld::MakePlayerAabb(const ServerPlayer& player) const
{
    ServerAabb result = {};
    result.center = { player.position.x, player.position.y + player.colliderCenterOffsetY, player.position.z };
    result.halfExtent = { player.colliderRadius, player.colliderHalfHeight, player.colliderRadius };

    return result;
}

ServerAabb ServerWorld::MakeMonsterAabb(const ServerMonster& monster) const
{
    ServerAabb result = {};
    result.center = { monster.position.x, monster.position.y + monster.colliderCenterOffsetY, monster.position.z };
    result.halfExtent = { monster.colliderRadius, monster.colliderHalfHeight, monster.colliderRadius };

    return result;
}

void ServerWorld::UpdatePlayerMeleeAttacks(float deltaTime)
{
    for (auto& [playerId, player] : mPlayers)
    {
        ServerMeleeAttack& attack = player.meleeAttack;

        if (!player.alive || !attack.active)
        {
            continue;
        }

        attack.elapsedTime += deltaTime;
        const float normalizedTime = attack.duration > 0.0f ? attack.elapsedTime / attack.duration : 1.0f;

        if (!attack.hitProcessed && normalizedTime >= attack.hitNormalizedTime) //콜라이더 활성화시점이 경우 충돌체크
        {
            attack.hitProcessed = true;
            ResolvePlayerMeleeAttack(player, attack);
        }

        if (attack.elapsedTime >= attack.duration)
        {
            attack.active = false;
        }
    }
}

bool ServerWorld::BeginPlayerMeleeAttack(ServerPlayer& player, const AttackCommand& command)
{
    SwordAttackConfig config = {};

    if (!GetSwordAttackConfig(command.attackIndex, config))
    {
        return false;
    }

    ServerVec3 direction = command.direction;

    if (!NormalizeXZ(direction))
        return false;

    ServerMeleeAttack attack = {};
    attack.active = true;
    attack.hitProcessed = false;
    attack.attackIndex = command.attackIndex;
    attack.direction = direction;
    attack.elapsedTime = 0.0f;
    attack.duration = config.duration;
    attack.hitNormalizedTime = config.hitNormalizedTime;
    attack.damage = config.damage;
    attack.reach = config.reach;
    attack.radius = config.radius;
    player.meleeAttack = attack;
    player.attackCooldown = config.cooldown;

    return true;
}

void ServerWorld::ResolvePlayerMeleeAttack(const ServerPlayer& attacker, const ServerMeleeAttack& attack)
{
    // 몸 중심보다 조금 위에서 검 Sweep 시작
    ServerVec3 start = attacker.position;
    start.y += 80.0f;
    const ServerVec3 end = Add(start, Multiply(attack.direction, attack.reach));

    for (const auto& [monsterId, monster] : mMonsters)
    {
        if (!monster.alive)
            continue;

        float hitT = 0.0f;

        if (!SegmentIntersectsAabb(start, end, MakeMonsterAabb(monster), attack.radius, hitT))
        {
            continue;
        }

        const ServerVec3 hitPosition = PointOnSegment(start, end, hitT);
        ApplyServerDamage(eDamageCause::PlayerMelee, attacker.entityId, monsterId, 0, attack.damage, hitPosition);
    }

    if (!mbFriendlyFire)
        return;

    for (const auto& [playerId, player] : mPlayers)
    {
        if (!player.alive || playerId == attacker.entityId)
        {
            continue;
        }

        float hitT = 0.0f;

        if (!SegmentIntersectsAabb(start, end, MakePlayerAabb(player), attack.radius, hitT))
        {
            continue;
        }

        ApplyServerDamage(eDamageCause::PlayerMelee, attacker.entityId, playerId, 0, attack.damage, PointOnSegment(start, end, hitT));
    }
}

void ServerWorld::ResolveMonsterMeleeAttack(ServerMonster& monster)
{
    ServerPlayer* target = FindAlivePlayer(monster.attackTargetId);

    if (target == nullptr)
        return;

    ServerVec3 direction = monster.attackDirection;

    if (!NormalizeXZ(direction))
    {
        direction = { target->position.x - monster.position.x, 0.0f, target->position.z - monster.position.z };

        if (!NormalizeXZ(direction))
            return;
    }

    ServerVec3 start = monster.position;
    start.y += 80.0f;
    constexpr float AttackReach = 150.0f;
    constexpr float AttackRadius = 25.0f;
    constexpr float AttackDamage = 10.0f;
    const ServerVec3 end = Add(start, Multiply(direction, AttackReach));
    float hitT = 0.0f;

    if (!SegmentIntersectsAabb(start, end, MakePlayerAabb(*target), AttackRadius, hitT))
    {
        return;
    }

    ApplyServerDamage(eDamageCause::MonsterMelee, monster.entityId, target->entityId, 0, AttackDamage, PointOnSegment(start, end, hitT));
}

void ServerWorld::ApplyServerDamage(
    eDamageCause cause, EntityId attackerId, EntityId victimId, ProjectileId projectileId, float damage, const ServerVec3& hitPosition)
{
    if (damage <= 0.0f)
        return;

    float appliedDamage = 0.0f;
    float remainingHp = 0.0f;
    bool isDead = false;
    bool foundVictim = false;
    auto playerIter = mPlayers.find(victimId);

    if (playerIter != mPlayers.end())
    {
        ServerPlayer& victim = playerIter->second;

        if (!victim.alive)
            return;

        appliedDamage = (std::min)(damage, victim.hp);
        victim.hp = (std::max)(0.0f, victim.hp - damage);
        remainingHp = victim.hp;

        if (victim.hp <= 0.0f)
        {
            victim.alive = false;
            victim.state = ePlayerState::DEATH;
            isDead = true;
        }

        foundVictim = true;
    }
    else
    {
        auto monsterIter = mMonsters.find(victimId);

        if (monsterIter == mMonsters.end())
        {
            return;
        }

        ServerMonster& victim = monsterIter->second;

        if (!victim.alive)
            return;

        appliedDamage = (std::min)(damage, victim.hp);
        victim.hp = (std::max)(0.0f, victim.hp - damage);
        remainingHp = victim.hp;
        auto brainIter = mMonsterBrains.find(victimId);

        if (victim.hp <= 0.0f)
        {
            victim.alive = false;
            isDead = true;

            if (brainIter != mMonsterBrains.end())
            {
                brainIter->second->SendFSMEvent("DEATH");
            }
        }
        else
        {
            if (brainIter != mMonsterBrains.end())
            {
                brainIter->second->SendFSMEvent("DAMAGE");
            }
        }

        foundVictim = true;
    }

    if (!foundVictim)
        return;

    VideoLog::Print("[DAMAGE] Attacker=", attackerId, " | Victim=", victimId, " | Applied=", appliedDamage, " | HP=", remainingHp,
        " | Dead=", isDead ? "true" : "false");

    if (isDead)
    {
        VideoLog::Print("[DEATH] Victim=", victimId, " | ConfirmedByServer");
    }

    Pkt_S_Damage packet = {};
    packet.header.type = ePacketType::S_DAMAGE;
    packet.header.size = sizeof(Pkt_S_Damage);
    packet.cause = cause;
    packet.projectileId = projectileId;
    packet.attackerId = attackerId;
    packet.victimId = victimId;
    packet.damage = appliedDamage;
    packet.remainingHp = remainingHp;
    packet.isDead = isDead ? 1 : 0;
    packet.hit_x = hitPosition.x;
    packet.hit_y = hitPosition.y;
    packet.hit_z = hitPosition.z;
    BroadcastExcept(0, packet);
}

void ServerWorld::SendMonsterSnapshotTo( //클라이언트마다 몬스터 생성하게 함
    EntityId targetPlayerId)
{
    for (const auto& [monsterId, monster] : mMonsters)
    {
        if (!monster.alive)
            continue;

        const Pkt_S_MonsterSpawn packet = MakeMonsterSpawnPacket(monster);
        SendTo(targetPlayerId, packet);
    }
}

EntityId ServerWorld::SpawnMonster(eModelType modelType, eWeaponType weaponType, const ServerVec3& position, float yaw, bool broadcast)
{
    if (!mAllocateEntityIdCallback)
    {
        std::cout << "[World] EntityId allocator가 없습니다." << '\n';

        return 0;
    }

    const EntityId monsterId = mAllocateEntityIdCallback();
    ServerMonster monster = {};
    monster.entityId = monsterId;
    monster.modelType = modelType;
    monster.weaponType = weaponType;
    monster.state = eMonsterState::IDLE;
    monster.position = position;
    monster.yaw = yaw;
    monster.maxHp = 100.0f;
    monster.hp = monster.maxHp;
    monster.alive = true;
    auto [iter, inserted] = mMonsters.emplace(monsterId, monster);

    if (!inserted)
        return 0;

    auto brain = std::make_unique<ME::FSMBrainCore>();
    const bool fsmLoaded = ME::FSMFactory::MakeFSMWithJsonFile(brain.get(), "..\\Resources\\EnemyFSMJson.json");

    if (!fsmLoaded)
    {
        std::cout << "[World] 몬스터 FSM 로딩 실패 id: " << monsterId << '\n';
        mMonsters.erase(monsterId);
        return 0;
    }

    iter->second.patrolOrigin = iter->second.position;
    iter->second.patrolTarget = iter->second.position;
    mMonsterBrains.emplace(monsterId, std::move(brain));

    if (broadcast)
    {
        const Pkt_S_MonsterSpawn packet = MakeMonsterSpawnPacket(iter->second);

        // EntityId 0은 사용하지 않으므로
        // exceptId = 0이면 모든 entered 클라이언트에게 전송
        BroadcastExcept(0, packet);
    }

    std::cout << "[World] 몬스터 생성 id: " << monsterId << " / 위치: " << position.x << ", " << position.y << ", " << position.z << '\n';

    return monsterId;
}

EntityId ServerWorld::FindClosestAlivePlayer(const ServerVec3& position, float maxDistance) const
{
    EntityId closestId = 0;
    float closestDistanceSquared = maxDistance * maxDistance;

    for (const auto& [playerId, player] : mPlayers)
    {
        if (!player.alive)
            continue;

        const float distanceSquared = DistanceSquaredXZ(position, player.position);

        if (distanceSquared > closestDistanceSquared)
        {
            continue;
        }

        closestDistanceSquared = distanceSquared;
        closestId = playerId;
    }

    return closestId;
}

ServerPlayer* ServerWorld::FindAlivePlayer(EntityId entityId)
{
    auto iter = mPlayers.find(entityId);

    if (iter == mPlayers.end() || !iter->second.alive)
    {
        return nullptr;
    }

    return &iter->second;
}

const ServerPlayer* ServerWorld::FindAlivePlayer(EntityId entityId) const
{
    auto iter = mPlayers.find(entityId);

    if (iter == mPlayers.end() || !iter->second.alive)
    {
        return nullptr;
    }

    return &iter->second;
}

float ServerWorld::DistanceSquaredXZ(const ServerVec3& lhs, const ServerVec3& rhs) const
{
    const float dx = rhs.x - lhs.x;
    const float dz = rhs.z - lhs.z;

    return dx * dx + dz * dz;
}

void ServerWorld::SelectRandomPatrolTarget(ServerMonster& monster, float radius)
{
    if (!monster.hasPatrolTarget)
    {
        monster.patrolOrigin = monster.position;
    }

    std::uniform_real_distribution<float> distribution(-radius, radius);
    monster.patrolTarget = {
        monster.patrolOrigin.x + distribution(mRandomEngine), monster.patrolOrigin.y, monster.patrolOrigin.z + distribution(mRandomEngine) };
    monster.hasPatrolTarget = true; //도착 여부 판단을 위한 flag
}

bool ServerWorld::MoveMonsterToward(ServerMonster& monster, const ServerVec3& target, float speed, float stoppingDistance, float deltaTime)
{
    float dx = target.x - monster.position.x;
    float dz = target.z - monster.position.z;
    const float distanceSquared = dx * dx + dz * dz;

    if (distanceSquared <= stoppingDistance * stoppingDistance)
    {
        return false;
    }

    const float distance = std::sqrt(distanceSquared);

    if (distance < 0.0001f)
        return false;

    dx /= distance;
    dz /= distance;
    const float remainingDistance = (std::max)(0.0f, distance - stoppingDistance);
    const float movement = (std::min)(speed * deltaTime, remainingDistance);
    monster.position.x += dx * movement;
    monster.position.z += dz * movement;
    constexpr float RadToDegree = 57.295779513f;
    monster.yaw = std::atan2(dx, dz) * RadToDegree + 180.0f;
    monster.transformDirty = true; //한번에 업데이트 하기위한 flag

    return true;
}

void ServerWorld::ApplyMonsterAnimation(ServerMonster& monster, const std::string& animationName, bool loop)
{
    eMonsterState nextState = monster.state;

    if (animationName == "MONSTER_IDLE")
        nextState = eMonsterState::IDLE;

    else if (animationName == "MONSTER_WALK")
        nextState = eMonsterState::WALK;

    else if (animationName == "MONSTER_RUN")
        nextState = eMonsterState::RUN;

    else if (animationName == "MONSTER_ATTACK")
        nextState = eMonsterState::ATTACK_1;

    else if (animationName == "MONSTER_ATTACK2")
        nextState = eMonsterState::ATTACK_2;

    else if (animationName == "MONSTER_ATTACK3")
        nextState = eMonsterState::ATTACK_3;

    else if (animationName == "MONSTER_HIT")
        nextState = eMonsterState::HIT;

    else if (animationName == "MONSTER_DEATH")
        nextState = eMonsterState::DEATH;

    if (monster.state != nextState)
    {
        monster.state = nextState;
        monster.stateDirty = true;
    }

    if (!loop)
    {
        // monster.actionRemainingTime = GetMonsterAnimationDuration(animationName);
    }
}

float ServerWorld::GetMonsterAnimationDuration(const std::string& animationName) const
{
    if (animationName == "MONSTER_ATTACK")
        return 0.9f;

    if (animationName == "MONSTER_ATTACK2")
        return 1.0f;

    if (animationName == "MONSTER_ATTACK3")
        return 1.1f;

    if (animationName == "MONSTER_HIT")
        return 0.6f;

    if (animationName == "MONSTER_DEATH")
        return 2.0f;

    return 0.0f;
}

const AnimationActionMeta* ServerWorld::FindMonsterAnimationMeta(const std::string& animationName) const
{
    auto iter = mMonsterAnimationMeta.find(animationName);

    if (iter == mMonsterAnimationMeta.end())
        return nullptr;

    return &iter->second;
}

void ServerWorld::BeginMonsterMeleeAttack(ServerMonster& monster, const std::vector<std::string>& animationNames)
{
    if (animationNames.empty())
        return;

    const ServerPlayer* target = FindAlivePlayer(monster.targetPlayerId);

    if (target == nullptr)
        return;

    std::uniform_int_distribution<std::size_t> distribution(0, animationNames.size() - 1);
    const std::size_t attackIndex = distribution(mRandomEngine);
    const std::string& animationName = animationNames[attackIndex];
    const AnimationActionMeta* animationMeta = FindMonsterAnimationMeta(animationName);

    if (animationMeta == nullptr || animationMeta->duration <= 0.0f)
    {
        std::cerr << "[Monster FSM] 애니메이션 메타데이터 없음: " << animationName << '\n';

        return;
    }

    monster.attackIndex = static_cast<std::uint8_t>(attackIndex);
    monster.attackTargetId = target->entityId;
    monster.currentActionAnimation = animationName;
    monster.actionElapsedTime = 0.0f;
    monster.actionDuration = animationMeta->duration;
    monster.attackHitNormalizedTime = animationMeta->hitNormalizedTime;
    monster.actionIsAttack = true;
    monster.attackHitProcessed = false;
    monster.state = attackIndex == 0 ? eMonsterState::ATTACK_1 : attackIndex == 1 ? eMonsterState::ATTACK_2 : eMonsterState::ATTACK_3;
    float dirX = target->position.x - monster.position.x;
    float dirY = target->position.y - monster.position.y;
    float dirZ = target->position.z - monster.position.z;
    const float lengthSquared = dirX * dirX + dirY * dirY + dirZ * dirZ;

    if (lengthSquared > 0.0001f)
    {
        const float inverseLength = 1.0f / std::sqrt(lengthSquared);
        dirX *= inverseLength;
        dirY *= inverseLength;
        dirZ *= inverseLength;
    }

    monster.attackDirection = { dirX, dirY, dirZ };

    //   monster.actionRemainingTime = GetMonsterAnimationDuration( animationName);
    monster.attackEventPending = true;

    // S_MONSTER_STATE와 S_MONSTER_ATTACK이
    // 중복으로 공격을 재생하지 않도록 한다.
    monster.stateDirty = false;
}

void ServerWorld::FlushMonsterReplication(float deltaTime) //몬스터 이동, 공격때마다 바로바로 하지않고 flag를 이용하여 한번에 업데이트
{
    constexpr float MoveSendInterval = 1.0f / 20.0f;

    for (auto& [monsterId, monster] : mMonsters)
    {
        monster.moveReplicationTimer += deltaTime;

        if (monster.stateDirty) //상태 변경이 있는지
        {
            const Pkt_S_MonsterState packet = MakeMonsterStatePacket(monster);
            BroadcastExcept(0, packet);
            monster.stateDirty = false;
        }

        if (monster.attackEventPending) //공격 이벤트가 발생했는지
        {
            const Pkt_S_MonsterAttack packet = MakeMonsterAttackPacket(monster);
            BroadcastExcept(0, packet);
            monster.attackEventPending = false;
        }

        if (monster.transformDirty && monster.moveReplicationTimer >= MoveSendInterval) //움직임이 있는지
        {
            const Pkt_S_MonsterMove packet = MakeMonsterMovePacket(monster);
            BroadcastExcept(0, packet);
            monster.transformDirty = false;
            monster.moveReplicationTimer = 0.0f;
        }
    }
}

void ServerWorld::DespawnRequestedMonsters()
{
    std::vector<EntityId> destroyIds;

    for (const auto& [monsterId, monster] : mMonsters)
    {
        if (monster.destroyRequested)
        {
            destroyIds.push_back(monsterId);
        }
    }

    for (EntityId monsterId : destroyIds) //특정 몬스터 나간다고 서버내 모든 클라이언트(플레이어)에게 알림
    {
        const Pkt_S_MonsterDespawn packet = MakeMonsterDespawnPacket(monsterId);
        BroadcastExcept(0, packet);
        mMonsterBrains.erase(monsterId);
        mMonsters.erase(monsterId);
    }
}

void ServerWorld::InitializeMonsterAnimationMeta()
{
    mMonsterAnimationMeta["MONSTER_ATTACK"] = { 1.1f, 0.1f };
    mMonsterAnimationMeta["MONSTER_ATTACK2"] = { 4.63f, 0.25f };
    mMonsterAnimationMeta["MONSTER_ATTACK3"] = { 3.7f, 0.45f };
    mMonsterAnimationMeta["MONSTER_HIT"] = { 2.73f, 0.0f };
    mMonsterAnimationMeta["MONSTER_DEATH"] = { 4.6f, 0.0f };
}