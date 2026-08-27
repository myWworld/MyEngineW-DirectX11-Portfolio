#pragma once

#include "ServerWorldReplicator.h"
#include "ServerWorldState.h"

// 플레이어의 입장/퇴장/이동/상태/무기 변경만 담당한다.
// 공격과 데미지는 ServerCombatSystem이 담당한다.
class ServerPlayerSystem
{
public:
    ServerPlayerSystem(
        ServerWorldState& state,
        ServerWorldReplicator& replicator);

    bool CanEnter(const EnterCommand& command) const;

    void Handle(const EnterCommand& command);
    void Handle(const LeaveCommand& command);
    void Handle(const MoveCommand& command);
    void Handle(const StateCommand& command);
    void Handle(const WeaponChangeCommand& command);

private:
    ServerWorldState& mState;
    ServerWorldReplicator& mReplicator;
};
