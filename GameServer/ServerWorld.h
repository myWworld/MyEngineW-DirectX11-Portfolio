#pragma once

#include "ServerCombatSystem.h"
#include "ServerMonsterSystem.h"
#include "ServerPlayerSystem.h"
#include "ServerWorldReplicator.h"
#include "ServerWorldState.h"

#include <atomic>
#include <mutex>
#include <queue>

// 서버 월드의 단일 스레드 실행 순서와 Command Queue만 조율
// 플레이어/전투/몬스터/복제/충돌 세부 구현은 각 시스템에 위임
class ServerWorld
{
public:
    using SendToCallback =
        ServerWorldReplicator::SendToCallback;

    using BroadcastExceptCallback =
        ServerWorldReplicator::BroadcastExceptCallback;

    using MarkEnteredCallback =
        ServerWorldReplicator::MarkEnteredCallback;

    using AllocateEntityIdCallback =
        ServerMonsterSystem::AllocateEntityIdCallback;

public:
    ServerWorld();
    ~ServerWorld() = default;

    ServerWorld(const ServerWorld&) = delete;
    ServerWorld& operator=(const ServerWorld&) = delete;

    void SetNetworkCallbacks(
        SendToCallback sendTo,
        BroadcastExceptCallback broadcastExcept,
        MarkEnteredCallback markEntered);

    void SetAllocateEntityIdCallback(
        AllocateEntityIdCallback allocator);

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
    void ProcessCommands();
    void Tick(float deltaTime);

    void HandleCommand(const EnterCommand& command);
    void HandleCommand(const LeaveCommand& command);
    void HandleCommand(const MoveCommand& command);
    void HandleCommand(const StateCommand& command);
    void HandleCommand(const WeaponChangeCommand& command);
    void HandleCommand(const AttackCommand& command);

private:

    ServerWorldState mState;
    ServerWorldReplicator mReplicator;
    ServerPlayerSystem mPlayerSystem;
    ServerCombatSystem mCombatSystem;
    ServerMonsterSystem mMonsterSystem;

    std::queue<WorldCommand> mCommandQueue;
    std::mutex mCommandMutex;

    std::atomic<bool> mbRunning = false;
    std::atomic<bool> mbInitialized = false;
};
