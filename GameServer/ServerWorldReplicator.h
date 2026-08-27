#pragma once

#include "ServerWorldState.h"

#include <cstdint>
#include <functional>

// 게임 규칙 코드가 패킷 구조체와 전송 세부사항을 직접 다루지 않도록
// 서버 월드의 모든 복제 패킷 생성을 한곳에 모음
class ServerWorldReplicator
{
public:
    using SendToCallback =
        std::function<void(EntityId targetId, const void* packetData, std::uint16_t packetSize)>;

    using BroadcastExceptCallback =
        std::function<void(EntityId exceptId, const void* packetData, std::uint16_t packetSize)>;

    using MarkEnteredCallback =
        std::function<void(EntityId entityId, bool entered)>;

public:
    void SetCallbacks(
        SendToCallback sendTo,
        BroadcastExceptCallback broadcastExcept,
        MarkEnteredCallback markEntered);

    void MarkEntered(EntityId entityId, bool entered);

    void SendInitialSnapshot(EntityId targetPlayerId, const ServerWorldState& state);

    void BroadcastPlayerEntered(const ServerPlayer& player);
    void BroadcastPlayerLeft(EntityId entityId);
    void BroadcastPlayerMove(const ServerPlayer& player);
    void BroadcastPlayerState(const ServerPlayer& player);
    void BroadcastPlayerWeapon(const ServerPlayer& player);

    void BroadcastPlayerAttack(const ServerPlayer& player, const AttackCommand& command);

    void BroadcastProjectileSpawn(const ServerProjectile& projectile);

    void BroadcastProjectileEnd(
        const ServerProjectile& projectile,
        const ProjectileHitResult& hit,
        eProjectileEndReason reason);

    void BroadcastDamage(
        eDamageCause cause,
        EntityId attackerId,
        EntityId victimId,
        ProjectileId projectileId,
        float appliedDamage,
        float remainingHp,
        bool isDead,
        const ServerVec3& hitPosition);

    void BroadcastMonsterSpawn(const ServerMonster& monster);
    void BroadcastMonsterDespawn(EntityId entityId);

    void FlushMonsterReplication(ServerWorldState& state, float deltaTime);

private:
    template <typename T>
    void SendTo(EntityId targetId, const T& packet)
    {
        if (!mSendToCallback)
            return;

        mSendToCallback(targetId, &packet, packet.header.size);
    }

    template <typename T>
    void BroadcastExcept(EntityId exceptId, const T& packet)
    {
        if (!mBroadcastExceptCallback)
            return;

        mBroadcastExceptCallback(exceptId, &packet, packet.header.size);
    }

private:
    SendToCallback mSendToCallback;
    BroadcastExceptCallback mBroadcastExceptCallback;
    MarkEnteredCallback mMarkEnteredCallback;
};
