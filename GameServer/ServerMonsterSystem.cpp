#include "ServerMonsterSystem.h"

#include "../MyEngine_Source/FSMBrainCore.h"
#include "../MyEngine_Source/FSMFactory.h"
#include "MEServerMonsterFSMContext.h"
#include "ServerCombatSystem.h"
#include "ServerMath.h"
#include "VideoLog.h"

#include <algorithm>
#include <cmath>
#include <iostream>
#include <utility>
#include <vector>

ServerMonsterSystem::ServerMonsterSystem(
    ServerWorldState& state,
    ServerWorldReplicator& replicator)
    : mState(state)
    , mReplicator(replicator)
{
    InitializeAnimationMeta();
}

ServerMonsterSystem::~ServerMonsterSystem() = default;

void ServerMonsterSystem::SetAllocateEntityIdCallback(AllocateEntityIdCallback allocator)
{
    mAllocateEntityIdCallback = std::move(allocator);
}

EntityId ServerMonsterSystem::SpawnMonster(
    eModelType modelType,
    eWeaponType weaponType,
    const ServerVec3& position,
    float yaw,
    bool broadcast)
{
    if (!mAllocateEntityIdCallback)
    {
        std::cout << "[World] EntityId allocator가 없습니다." << '\n';
        return 0;
    }

    const EntityId monsterId = mAllocateEntityIdCallback();

    if (monsterId == 0)
        return 0;

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

    auto [iter, inserted] = mState.monsters.emplace(monsterId, monster);

    if (!inserted)
        return 0;

    auto brain = std::make_unique<ME::FSMBrainCore>();

    const bool fsmLoaded = ME::FSMFactory::MakeFSMWithJsonFile(brain.get(), "..\\Resources\\EnemyFSMJson.json"); //fsm 등록

    if (!fsmLoaded)
    {
        std::cout << "[World] 몬스터 FSM 로딩 실패 id: " << monsterId << '\n';

        mState.monsters.erase(monsterId);
        return 0;
    }

    iter->second.patrolOrigin = iter->second.position;
    iter->second.patrolTarget = iter->second.position;

    mBrains.emplace(monsterId, std::move(brain));

    if (broadcast)
    {
        mReplicator.BroadcastMonsterSpawn(iter->second);
    }

    std::cout
        << "[World] 몬스터 생성 id: " << monsterId << " / 위치: "
        << position.x << ", " << position.y << ", " << position.z << '\n';

    return monsterId;
}

void ServerMonsterSystem::TickActions(float deltaTime, ServerCombatSystem& combatSystem)
{
    for (auto& [monsterId, monster] : mState.monsters)
    {

        if (monster.actionDuration <= 0.0f)
            continue;

        monster.actionElapsedTime =
            (std::min)(monster.actionElapsedTime + deltaTime, monster.actionDuration);

        const float normalizedTime = monster.actionElapsedTime / monster.actionDuration;

        if (monster.alive &&
            monster.actionIsAttack &&
            !monster.attackHitProcessed &&
            normalizedTime >= monster.attackHitNormalizedTime)
        {
            monster.attackHitProcessed = true;
            combatSystem.ResolveMonsterMeleeAttack(monster);
        }
    }
}

void ServerMonsterSystem::TickAI(float deltaTime)
{
    for (auto& [monsterId, monster] : mState.monsters)
    {
        auto brainIter = mBrains.find(monsterId);

        if (brainIter == mBrains.end())
            continue;

        // 이번 Tick에만 유효한 Context
        ServerMonsterFSMContext context( *this, monster,deltaTime);

        if (!monster.alive &&
            monster.state != eMonsterState::DEATH)
        {
            // Update 바깥에서 발생한 이벤트는 FSM Core의 pending 전환으로 들어감
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

void ServerMonsterSystem::NotifyDamage(EntityId monsterId,bool isDead)
{
    auto brainIter = mBrains.find(monsterId);

    if (brainIter == mBrains.end())
        return;

    brainIter->second->SendFSMEvent(isDead ? "DEATH" : "DAMAGE");
}

void ServerMonsterSystem::DespawnRequestedMonsters()
{
    std::vector<EntityId> destroyIds;

    for (const auto& [monsterId, monster] : mState.monsters)
    {
        if (monster.destroyRequested)
        {
            destroyIds.push_back(monsterId);
        }
    }

    for (EntityId monsterId : destroyIds)
    {
        mReplicator.BroadcastMonsterDespawn(monsterId);
        mBrains.erase(monsterId);
        mState.monsters.erase(monsterId);
    }
}

EntityId ServerMonsterSystem::FindClosestAlivePlayer(const ServerVec3& position, float maxDistance) const
{
    EntityId closestId = 0;
    float closestDistanceSquared = maxDistance * maxDistance;

    for (const auto& [playerId, player] : mState.players)
    {
        if (!player.alive)
            continue;

        const float distanceSquared = ServerMath::DistanceSquaredXZ(position, player.position);

        if (distanceSquared > closestDistanceSquared)
            continue;

        closestDistanceSquared = distanceSquared;
        closestId = playerId;
    }

    return closestId;
}

const ServerPlayer* ServerMonsterSystem::FindAlivePlayer(EntityId entityId) const
{
    auto iter = mState.players.find(entityId);

    if (iter == mState.players.end() || !iter->second.alive)
        return nullptr;

    return &iter->second;
}

void ServerMonsterSystem::SelectRandomPatrolTarget(ServerMonster& monster, float radius)
{
    if (!monster.hasPatrolTarget)
    {
        monster.patrolOrigin = monster.position;
    }

    std::uniform_real_distribution<float> distribution(-radius, radius);

    monster.patrolTarget =
    {
        monster.patrolOrigin.x + distribution(mRandomEngine),
        monster.patrolOrigin.y,
        monster.patrolOrigin.z + distribution(mRandomEngine)
    };

    monster.hasPatrolTarget = true;
}

bool ServerMonsterSystem::MoveMonsterToward(
    ServerMonster& monster,
    const ServerVec3& target,
    float speed,
    float stoppingDistance,
    float deltaTime)
{
    float dx = target.x - monster.position.x;
    float dz = target.z - monster.position.z;

    const float distanceSquared = dx * dx + dz * dz;

    if (distanceSquared <= stoppingDistance * stoppingDistance)
        return false;

    const float distance = std::sqrt(distanceSquared);

    if (distance < 0.0001f)
        return false;

    dx /= distance;
    dz /= distance;

    const float remainingDistance =
        (std::max)(0.0f, distance - stoppingDistance);

    const float movement =
        (std::min)(speed * deltaTime, remainingDistance);

    monster.position.x += dx * movement;
    monster.position.z += dz * movement;

    constexpr float RadToDegree = 57.295779513f;
    monster.yaw = std::atan2(dx, dz) * RadToDegree + 180.0f;
    monster.transformDirty = true;

    return true;
}

void ServerMonsterSystem::ApplyMonsterAnimation(
    ServerMonster& monster,
    const std::string& animationName,
    bool loop)
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

    monster.currentActionAnimation = animationName;
    monster.actionElapsedTime = 0.0f;
    monster.actionIsAttack = false;
    monster.attackHitProcessed = true;

    if (loop)
    {
        monster.actionDuration = 0.0f;
        return;
    }

    const AnimationActionMeta* animationMeta =FindAnimationMeta(animationName);

    if (animationMeta == nullptr ||
        animationMeta->duration <= 0.0f)
    {
        std::cerr
            << "[Monster FSM] 애니메이션 메타데이터 없음: "
            << animationName
            << '\n';

        monster.actionDuration = 0.0f;
        return;
    }

    // DAMAGE/DEATH도 AnimFinishDecision이 정상적으로 기다리도록
    // 단일 메타데이터 테이블에서 지속시간을 설정
    monster.actionDuration = animationMeta->duration;
    monster.attackHitNormalizedTime = animationMeta->hitNormalizedTime;
}

void ServerMonsterSystem::BeginMonsterMeleeAttack(ServerMonster& monster, const std::vector<std::string>& animationNames)
{
    if (animationNames.empty())
        return;

    const ServerPlayer* target =  FindAlivePlayer(monster.targetPlayerId);

    if (target == nullptr)
        return;

    std::uniform_int_distribution<std::size_t> distribution( 0, animationNames.size() - 1);

    const std::size_t selectedIndex = distribution(mRandomEngine);

    const std::string& animationName = animationNames[selectedIndex];

    const AnimationActionMeta* animationMeta = FindAnimationMeta(animationName);

    if (animationMeta == nullptr ||
        animationMeta->duration <= 0.0f)
    {
        std::cerr
            << "[Monster FSM] 애니메이션 메타데이터 없음: "
            << animationName
            << '\n';
        return;
    }

    monster.attackIndex = static_cast<std::uint8_t>(selectedIndex);

    monster.attackTargetId = target->entityId;
    monster.currentActionAnimation = animationName;
    monster.actionElapsedTime = 0.0f;
    monster.actionDuration = animationMeta->duration;
    monster.attackHitNormalizedTime = animationMeta->hitNormalizedTime;
    monster.actionIsAttack = true;
    monster.attackHitProcessed = false;

    if (selectedIndex == 0)
    {
        monster.state = eMonsterState::ATTACK_1;
    }
    else if (selectedIndex == 1)
    {
        monster.state = eMonsterState::ATTACK_2;
    }
    else
    {
        monster.state = eMonsterState::ATTACK_3;
    }

    float dirX = target->position.x - monster.position.x;
    float dirY = target->position.y - monster.position.y;
    float dirZ = target->position.z - monster.position.z;

    const float lengthSquared =  dirX * dirX + dirY * dirY + dirZ * dirZ;

    if (lengthSquared > 0.0001f)
    {
        const float inverseLength = 1.0f / std::sqrt(lengthSquared);

        dirX *= inverseLength;
        dirY *= inverseLength;
        dirZ *= inverseLength;
    }

    monster.attackDirection =
    {
        dirX,
        dirY,
        dirZ
    };

    monster.attackEventPending = true;

    // S_MONSTER_STATE와 S_MONSTER_ATTACK으로 공격 애니메이션이
    // 중복 재생되지 않도록 공격 이벤트만 복제
    monster.stateDirty = false;
}

const AnimationActionMeta* ServerMonsterSystem::FindAnimationMeta(const std::string& animationName) const
{
    auto iter = mAnimationMeta.find(animationName);

    if (iter == mAnimationMeta.end())
        return nullptr;

    return &iter->second;
}

void ServerMonsterSystem::InitializeAnimationMeta()
{
    // 서버 판정용 duration과 hit normalized time의 단일 소스
    mAnimationMeta["MONSTER_ATTACK"] = { 1.1f, 0.1f };
    mAnimationMeta["MONSTER_ATTACK2"] = { 4.63f, 0.25f };
    mAnimationMeta["MONSTER_ATTACK3"] = { 3.7f, 0.45f };
    mAnimationMeta["MONSTER_HIT"] = { 2.73f, 0.0f };
    mAnimationMeta["MONSTER_DEATH"] = { 4.6f, 0.0f };
}
