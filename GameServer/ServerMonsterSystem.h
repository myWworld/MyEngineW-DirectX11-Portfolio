#pragma once

#include "ServerWorldReplicator.h"
#include "ServerWorldState.h"

#include <functional>
#include <memory>
#include <random>
#include <string>
#include <unordered_map>
#include <vector>

namespace ME
{
    class FSMBrainCore;
}

class ServerCombatSystem;

// 몬스터 생성/삭제, FSM runtime, 이동, 순찰, 애니메이션 Action을 담당
class ServerMonsterSystem
{
public:
    using AllocateEntityIdCallback = std::function<EntityId()>;

public:
    ServerMonsterSystem(
        ServerWorldState& state,
        ServerWorldReplicator& replicator);

    ~ServerMonsterSystem();

    ServerMonsterSystem(const ServerMonsterSystem&) = delete;
    ServerMonsterSystem& operator=(const ServerMonsterSystem&) = delete;

    void SetAllocateEntityIdCallback(AllocateEntityIdCallback allocator);

    EntityId SpawnMonster(
        eModelType modelType,
        eWeaponType weaponType,
        const ServerVec3& position,
        float yaw,
        bool broadcast);

    void TickActions(
        float deltaTime,
        ServerCombatSystem& combatSystem);

    void TickAI(float deltaTime);

    void NotifyDamage(EntityId monsterId, bool isDead);
    void DespawnRequestedMonsters();

    // 아래 메서드는 MEServerMonsterFSMContext가 사용
    EntityId FindClosestAlivePlayer(const ServerVec3& position, float maxDistance) const;

    const ServerPlayer* FindAlivePlayer(EntityId entityId) const;

    void SelectRandomPatrolTarget(ServerMonster& monster, float radius);

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

    void BeginMonsterMeleeAttack(ServerMonster& monster, const std::vector<std::string>& animationNames);

private:
    const AnimationActionMeta* FindAnimationMeta(const std::string& animationName) const;

    void InitializeAnimationMeta();

private:
    ServerWorldState& mState;
    ServerWorldReplicator& mReplicator;

    std::unordered_map<EntityId, std::unique_ptr<ME::FSMBrainCore>> mBrains;

    std::unordered_map<std::string,AnimationActionMeta> mAnimationMeta;

    std::mt19937 mRandomEngine
    {
        std::random_device{}()
    };

    AllocateEntityIdCallback mAllocateEntityIdCallback;
};
