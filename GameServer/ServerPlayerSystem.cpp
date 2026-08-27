#include "ServerPlayerSystem.h"

#include "ServerMath.h"
#include "VideoLog.h"

#include <iostream>

namespace
{
    bool IsValidPlayerWeaponType(eWeaponType weaponType)
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
}

ServerPlayerSystem::ServerPlayerSystem(ServerWorldState& state, ServerWorldReplicator& replicator)
    : mState(state)
    , mReplicator(replicator)
{
}

bool ServerPlayerSystem::CanEnter(const EnterCommand& command) const
{
    if (command.entityId == 0)
        return false;

    if (!ServerMath::IsFinite(command.position))
        return false;

    if (!IsValidPlayerWeaponType(command.weaponType))
        return false;

    return mState.players.find(command.entityId) == mState.players.end();
}

void ServerPlayerSystem::Handle(const EnterCommand& command)
{
    if (!CanEnter(command))
        return;

    ServerPlayer newPlayer = {};
    newPlayer.entityId = command.entityId;
    newPlayer.modelType = command.modelType;
    newPlayer.weaponType = command.weaponType;
    newPlayer.state = ePlayerState::IDLE;
    newPlayer.position = command.position;
    newPlayer.yaw = command.yaw;

    // 현재 월드 상태를 먼저 전송한 뒤 새 플레이어를 등록한
    // 따라서 본인에 대한 S_ENTER가 스냅샷에 중복 포함되지 않는다
    mReplicator.SendInitialSnapshot(newPlayer.entityId, mState);

    VideoLog::Print("[SNAPSHOT] To=", newPlayer.entityId, " | ExistingPlayers=", mState.players.size(), " | Monsters=", mState.monsters.size());

    mState.players.emplace(newPlayer.entityId, newPlayer);

    VideoLog::Print("[WORLD] Player Enter | Id=", newPlayer.entityId, " | TotalPlayers=", mState.players.size());

    mReplicator.MarkEntered(newPlayer.entityId, true);
    mReplicator.BroadcastPlayerEntered(newPlayer);
}

void ServerPlayerSystem::Handle(const LeaveCommand& command)
{
    auto iter = mState.players.find(command.entityId);

    if (iter == mState.players.end())
        return;

    mState.players.erase(iter);

    // main.cpp에서도 연결 종료 직전에 false를 설정하지만,
    // 월드 레벨 Leave 경로만 사용되는 경우도 안전하게 처리함
    mReplicator.MarkEntered(command.entityId, false);
    mReplicator.BroadcastPlayerLeft(command.entityId);

    std::cout
        << "[World] 플레이어 퇴장 id: "
        << command.entityId
        << " / 현재 인원: "
        << mState.players.size()
        << '\n';
}

void ServerPlayerSystem::Handle(const MoveCommand& command)
{
    if (!ServerMath::IsFinite(command.position))
        return;

    if (!ServerMath::IsFinite(command.yaw))
        return;

    auto iter = mState.players.find(command.entityId);

    if (iter == mState.players.end())
        return;

    ServerPlayer& player = iter->second;

    if (!player.alive)
        return;

    player.position = command.position;
    player.yaw = command.yaw;

    mReplicator.BroadcastPlayerMove(player);
}

void ServerPlayerSystem::Handle(const StateCommand& command)
{
    // HIT/DEATH는 서버 데미지 판정에서만 확정한다.
    if (command.state != ePlayerState::IDLE && command.state != ePlayerState::WALK)
    {
        return;
    }

    auto iter = mState.players.find(command.entityId);

    if (iter == mState.players.end())
        return;

    ServerPlayer& player = iter->second;

    if (!player.alive)
        return;

    if (player.state == command.state)
        return;

    player.state = command.state;
    mReplicator.BroadcastPlayerState(player);
}

void ServerPlayerSystem::Handle(const WeaponChangeCommand& command)
{
    if (!IsValidPlayerWeaponType(command.weaponType))
        return;

    auto iter = mState.players.find(command.entityId);

    if (iter == mState.players.end())
        return;

    ServerPlayer& player = iter->second;

    if (!player.alive)
        return;

    if (player.weaponType == command.weaponType)
        return;

    player.weaponType = command.weaponType;
    mReplicator.BroadcastPlayerWeapon(player);
}
