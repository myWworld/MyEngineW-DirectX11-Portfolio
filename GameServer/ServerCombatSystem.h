#pragma once

#include "ServerWorldReplicator.h"
#include "ServerWorldState.h"

#include <functional>

// 플레이어 공격, 근접 공격 타이밍, 투사체, 충돌 결과와 데미지를 담당한다.
class ServerCombatSystem
{
public:
    using MonsterDamageCallback =
        std::function<void(EntityId monsterId, bool isDead)>;

public:
    ServerCombatSystem(
        ServerWorldState& state,
        ServerWorldReplicator& replicator);

    void SetMonsterDamageCallback(MonsterDamageCallback callback);

    void Handle(const AttackCommand& command);

    void TickPlayerCombat(float deltaTime);
    void TickProjectiles(float deltaTime);

    void ResolveMonsterMeleeAttack(ServerMonster& monster);

private:
    void UpdatePlayerMeleeAttacks(float deltaTime);

    bool BeginPlayerMeleeAttack(
        ServerPlayer& player,
        const AttackCommand& command);

    void ResolvePlayerMeleeAttack(
        const ServerPlayer& attacker,
        const ServerMeleeAttack& attack);

    void SpawnProjectile(
        const ServerPlayer& player,
        const AttackCommand& command);

    bool FindClosestProjectileHit(
        const ServerProjectile& projectile,
        const ServerVec3& start,
        const ServerVec3& end,
        ProjectileHitResult& outHit) const;

    bool IsValidProjectileOrigin(
        const ServerPlayer& player,
        const ServerVec3& origin) const;

    ServerPlayer* FindAlivePlayer(EntityId entityId);

    void ApplyDamage(
        eDamageCause cause,
        EntityId attackerId,
        EntityId victimId,
        ProjectileId projectileId,
        float damage,
        const ServerVec3& hitPosition);

private:
    ServerWorldState& mState;
    ServerWorldReplicator& mReplicator;
    MonsterDamageCallback mMonsterDamageCallback;
};
