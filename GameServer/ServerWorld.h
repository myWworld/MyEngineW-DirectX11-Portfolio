#pragma once
#include "../MyEngine_Source/FSMBrainCore.h"
#include "MEServerMonsterFSMContext.h"

#include "PacketUtility.h"
#include "ServerTypes.h"
#include <memory>
#include <random>
#include <string>
#include <vector>

#include <atomic>
#include <cstdint>
#include <functional>
#include <mutex>
#include <queue>
#include <unordered_map>

class ServerMonsterFSMContext;
class ServerWorld
{
    friend class ServerMonsterFSMContext;

public:
    using SendToCallback =
        std::function<void(
            EntityId targetId,
            const void* packetData,
            std::uint16_t packetSize)>;

    using BroadcastExceptCallback =
        std::function<void(
            EntityId exceptId,
            const void* packetData,
            std::uint16_t packetSize)>;

    using MarkEnteredCallback =
        std::function<void(
            EntityId entityId,
            bool entered)>;

    using AllocateEntityIdCallback =
        std::function<EntityId()>;
public:
    ServerWorld() = default;
    ~ServerWorld() = default;

    void SetNetworkCallbacks(
        SendToCallback sendTo,
        BroadcastExceptCallback broadcastExcept,
        MarkEnteredCallback markEntered);

    void SetAllocateEntityIdCallback(AllocateEntityIdCallback allocator);

    void EnqueueCommand(WorldCommand command);

    void EnsureWorldInitialization();

    void Run();
    void Stop();

    EntityId SpawnMonster(
        eModelType modelType,
        eWeaponType weaponType,
        const ServerVec3& position,
        float yaw,
        bool broadcast);

private:

    template <typename T>
    void SendTo(EntityId targetId, const T& packet)
    {
        if (!mSendToCallback)
            return;

        mSendToCallback(
            targetId,
            &packet,
            packet.header.size
        );
    }

    void SendMonsterSnapshotTo(EntityId targetPlayerId);



    template <typename T>
    void BroadcastExcept(EntityId exceptId, const T& packet)
    {
        if (!mBroadcastExceptCallback)
            return;

        mBroadcastExceptCallback(
            exceptId,
            &packet,
            packet.header.size
        );
    }

    void ProcessCommands();
    void Tick(float deltaTime);

    void HandleCommand(const EnterCommand& command);
    void HandleCommand(const LeaveCommand& command);
    void HandleCommand(const MoveCommand& command);
    void HandleCommand(const StateCommand& command);
    void HandleCommand(const WeaponChangeCommand& command);
    void HandleCommand(const AttackCommand& command);


  

    void UpdateMonsters(float deltaTime);

    void UpdateProjectiles(float deltaTime);

    void SpawnProjectile(
        const ServerPlayer& player,
        const AttackCommand& command);

    void EndProjectile(
        ProjectileId projectileId,
        eProjectileEndReason reason,
        EntityId hitEntityId,
        const ServerVec3& endPosition);



    void UpdatePlayerMeleeAttacks(float deltaTime);

    bool BeginPlayerMeleeAttack(
        ServerPlayer& player,
        const AttackCommand& command);

    void ResolvePlayerMeleeAttack(
        const ServerPlayer& attacker,
        const ServerMeleeAttack& attack);

    void ResolveMonsterMeleeAttack(
        ServerMonster& monster);

    bool FindClosestProjectileHit(
        const ServerProjectile& projectile,
        const ServerVec3& start,
        const ServerVec3& end,
        ProjectileHitResult& outHit) const;

    void BroadcastProjectileEnd(
        const ServerProjectile& projectile,
        const ProjectileHitResult& hit,
        eProjectileEndReason reason);

    void ApplyServerDamage(
        eDamageCause cause,
        EntityId attackerId,
        EntityId victimId,
        ProjectileId projectileId,
        float damage,
        const ServerVec3& hitPosition);

    bool IsValidProjectileOrigin(
        const ServerPlayer& player,
        const ServerVec3& origin) const;

    ServerAabb MakePlayerAabb(
        const ServerPlayer& player) const;

    ServerAabb MakeMonsterAabb(
        const ServerMonster& monster) const;


    EntityId FindClosestAlivePlayer(
        const ServerVec3& position,
        float maxDistance) const;

    ServerPlayer* FindAlivePlayer(
        EntityId entityId);

    const ServerPlayer* FindAlivePlayer(
        EntityId entityId) const;

    float DistanceSquaredXZ(
        const ServerVec3& lhs,
        const ServerVec3& rhs) const;

    void SelectRandomPatrolTarget(
        ServerMonster& monster,
        float radius);

    bool MoveMonsterToward(
        ServerMonster& monster,
        const ServerVec3& target,
        float speed,
        float stoppingDistance,
        float deltaTime);

    void ApplyMonsterAnimation(
        ServerMonster& monster,
        const std::string& animationName,
        bool loop);

    void BeginMonsterMeleeAttack(
        ServerMonster& monster,
        const std::vector<std::string>& animationNames);

    const AnimationActionMeta* FindMonsterAnimationMeta(const std::string& animationName) const;

    float GetMonsterAnimationDuration(
        const std::string& animationName) const;

    void FlushMonsterReplication(
        float deltaTime);

    void DespawnRequestedMonsters();

    void InitializeMonsterAnimationMeta();

private:


    std::unordered_map<EntityId,std::unique_ptr<ME::FSMBrainCore>>  mMonsterBrains;

    std::unordered_map<std::string,AnimationActionMeta> mMonsterAnimationMeta;

    std::mt19937 mRandomEngine
    {
        std::random_device{}()
    };


    // 이 맵은 ServerWorld 스레드만 수정
    std::unordered_map<EntityId, ServerPlayer> mPlayers;
    std::unordered_map<EntityId, ServerMonster> mMonsters;

    std::queue<WorldCommand> mCommandQueue;
    std::mutex mCommandMutex;

    std::atomic<bool> mbRunning = false;

    SendToCallback mSendToCallback;
    BroadcastExceptCallback mBroadcastExceptCallback;
    MarkEnteredCallback mMarkEnteredCallback;
    AllocateEntityIdCallback  mAllocateEntityIdCallback;

    std::atomic<bool> mbInitialized = false;

    std::unordered_map<ProjectileId, ServerProjectile> mProjectiles;
    ProjectileId mNextProjectileId = 1;

    bool mbFriendlyFire = false;

    // 맵 벽, 기둥 등의 서버 Proxy Collider
    std::vector<ServerStaticCollider> mStaticWorldColliders;
};