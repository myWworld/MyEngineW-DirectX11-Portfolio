#include "ServerWorldReplicator.h"

#include <utility>

namespace
{
    Pkt_S_Enter MakeEnterPacket(const ServerPlayer& player)
    {
        Pkt_S_Enter packet = {};
        packet.header.type = ePacketType::S_ENTER;
        packet.header.size = static_cast<std::uint16_t>(sizeof(Pkt_S_Enter));
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
        packet.header.size = static_cast<std::uint16_t>(sizeof(Pkt_S_Move));
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
        packet.header.size = static_cast<std::uint16_t>(sizeof(Pkt_S_State));
        packet.entityId = player.entityId;
        packet.state = player.state;

        return packet;
    }

    Pkt_S_WeaponChange MakeWeaponChangePacket(const ServerPlayer& player)
    {
        Pkt_S_WeaponChange packet = {};
        packet.header.type = ePacketType::S_WEAPON_CHANGE;
        packet.header.size = static_cast<std::uint16_t>(sizeof(Pkt_S_WeaponChange));
        packet.entityId = player.entityId;
        packet.weaponType = player.weaponType;

        return packet;
    }

    Pkt_S_Attack MakeAttackPacket(const ServerPlayer& player, const AttackCommand& command)
    {
        Pkt_S_Attack packet = {};
        packet.header.type = ePacketType::S_ATTACK;
        packet.header.size = static_cast<std::uint16_t>(sizeof(Pkt_S_Attack));
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
        packet.header.size = static_cast<std::uint16_t>(sizeof(Pkt_S_Leave));
        packet.entityId = entityId;

        return packet;
    }

    Pkt_S_MonsterSpawn MakeMonsterSpawnPacket(const ServerMonster& monster)
    {
        Pkt_S_MonsterSpawn packet = {};
        packet.header.type = ePacketType::S_MONSTER_SPAWN;
        packet.header.size = static_cast<std::uint16_t>(sizeof(Pkt_S_MonsterSpawn));
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
        packet.header.size = static_cast<std::uint16_t>(sizeof(Pkt_S_MonsterMove));
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
        packet.header.size = static_cast<std::uint16_t>(sizeof(Pkt_S_MonsterState));
        packet.entityId = monster.entityId;
        packet.state = monster.state;

        return packet;
    }

    Pkt_S_MonsterAttack MakeMonsterAttackPacket(const ServerMonster& monster)
    {
        Pkt_S_MonsterAttack packet = {};
        packet.header.type = ePacketType::S_MONSTER_ATTACK;
        packet.header.size = static_cast<std::uint16_t>(sizeof(Pkt_S_MonsterAttack));
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
        packet.header.size = static_cast<std::uint16_t>(sizeof(Pkt_S_MonsterDespawn));
        packet.entityId = entityId;

        return packet;
    }
}

void ServerWorldReplicator::SetCallbacks(
    SendToCallback sendTo,
    BroadcastExceptCallback broadcastExcept,
    MarkEnteredCallback markEntered)
{
    mSendToCallback = std::move(sendTo);
    mBroadcastExceptCallback = std::move(broadcastExcept);
    mMarkEnteredCallback = std::move(markEntered);
}

void ServerWorldReplicator::MarkEntered(EntityId entityId, bool entered)
{
    if (mMarkEnteredCallback)
    {
        mMarkEnteredCallback(entityId, entered);
    }
}

void ServerWorldReplicator::SendInitialSnapshot(EntityId targetPlayerId, const ServerWorldState& state)
{
    for (const auto& [existingId, existingPlayer] : state.players)
    {
        if (existingId == targetPlayerId)
            continue;

        SendTo(targetPlayerId, MakeEnterPacket(existingPlayer));
    }

    for (const auto& [monsterId, monster] : state.monsters)
    {

        if (!monster.alive)
            continue;

        SendTo(targetPlayerId, MakeMonsterSpawnPacket(monster));
    }
}

void ServerWorldReplicator::BroadcastPlayerEntered(const ServerPlayer& player)
{
    BroadcastExcept(player.entityId, MakeEnterPacket(player));
}

void ServerWorldReplicator::BroadcastPlayerLeft(EntityId entityId)
{
    BroadcastExcept(entityId, MakeLeavePacket(entityId));
}

void ServerWorldReplicator::BroadcastPlayerMove(const ServerPlayer& player)
{
    BroadcastExcept(player.entityId, MakeMovePacket(player));
}

void ServerWorldReplicator::BroadcastPlayerState(const ServerPlayer& player)
{
    BroadcastExcept(player.entityId, MakeStatePacket(player));
}

void ServerWorldReplicator::BroadcastPlayerWeapon(const ServerPlayer& player)
{
    BroadcastExcept(player.entityId, MakeWeaponChangePacket(player));
}

void ServerWorldReplicator::BroadcastPlayerAttack(
    const ServerPlayer& player,
    const AttackCommand& command)
{
    BroadcastExcept(player.entityId, MakeAttackPacket(player, command));
}

void ServerWorldReplicator::BroadcastProjectileSpawn(const ServerProjectile& projectile)
{
    Pkt_S_ProjectileSpawn packet = {};
    packet.header.type = ePacketType::S_PROJECTILE_SPAWN;
    packet.header.size = static_cast<std::uint16_t>(sizeof(Pkt_S_ProjectileSpawn));
    packet.projectileId = projectile.projectileId;
    packet.ownerEntityId = projectile.ownerEntityId;
    packet.start_x = projectile.position.x;
    packet.start_y = projectile.position.y;
    packet.start_z = projectile.position.z;
    packet.velocity_x = projectile.velocity.x;
    packet.velocity_y = projectile.velocity.y;
    packet.velocity_z = projectile.velocity.z;
    packet.lifeTime = projectile.remainingLife;

    // exceptId = 0이면 모든 entered 클라이언트에게 전송한다.
    BroadcastExcept(0, packet);
}

void ServerWorldReplicator::BroadcastProjectileEnd(
    const ServerProjectile& projectile,
    const ProjectileHitResult& hit,
    eProjectileEndReason reason)
{
    Pkt_S_ProjectileEnd packet = {};
    packet.header.type = ePacketType::S_PROJECTILE_END;
    packet.header.size = static_cast<std::uint16_t>(sizeof(Pkt_S_ProjectileEnd));
    packet.projectileId = projectile.projectileId;
    packet.reason = reason;
    packet.hitEntityId = hit.entityId;
    packet.end_x = hit.hitPosition.x;
    packet.end_y = hit.hitPosition.y;
    packet.end_z = hit.hitPosition.z;

    BroadcastExcept(0, packet);
}

void ServerWorldReplicator::BroadcastDamage(
    eDamageCause cause,
    EntityId attackerId,
    EntityId victimId,
    ProjectileId projectileId,
    float appliedDamage,
    float remainingHp,
    bool isDead,
    const ServerVec3& hitPosition)
{
    Pkt_S_Damage packet = {};
    packet.header.type = ePacketType::S_DAMAGE;
    packet.header.size = static_cast<std::uint16_t>(sizeof(Pkt_S_Damage));
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

void ServerWorldReplicator::BroadcastMonsterSpawn(const ServerMonster& monster)
{
    BroadcastExcept(0, MakeMonsterSpawnPacket(monster));
}

void ServerWorldReplicator::BroadcastMonsterDespawn(EntityId entityId)
{
    BroadcastExcept(0, MakeMonsterDespawnPacket(entityId));
}

void ServerWorldReplicator::FlushMonsterReplication(ServerWorldState& state, float deltaTime)
{
    constexpr float MoveSendInterval = 1.0f / 20.0f;

    for (auto& [monsterId, monster] : state.monsters)
    {
        monster.moveReplicationTimer += deltaTime;

        if (monster.stateDirty)
        {
            BroadcastExcept(0, MakeMonsterStatePacket(monster));
            monster.stateDirty = false;
        }

        if (monster.attackEventPending)
        {
            BroadcastExcept(0, MakeMonsterAttackPacket(monster));
            monster.attackEventPending = false;
        }

        if (monster.transformDirty &&
            monster.moveReplicationTimer >= MoveSendInterval)
        {
            BroadcastExcept(0, MakeMonsterMovePacket(monster));
            monster.transformDirty = false;
            monster.moveReplicationTimer = 0.0f;
        }
    }
}
