#include "MEServerMonsterFSMContext.h"

#include "ServerMath.h"
#include "ServerMonsterSystem.h"

#include <limits>

ServerMonsterFSMContext::ServerMonsterFSMContext(
    ServerMonsterSystem& monsterSystem,
    ServerMonster& monster,
    float deltaTime)
    : mMonsterSystem(monsterSystem)
    , mMonster(monster)
    , mDeltaTime(deltaTime)
{
}

float ServerMonsterFSMContext::GetDeltaTime() const
{
    return mDeltaTime;
}

bool ServerMonsterFSMContext::DetectTarget(float radius)
{
    const EntityId targetId =
        mMonsterSystem.FindClosestAlivePlayer(
            mMonster.position,
            radius);

    mMonster.targetPlayerId = targetId;
    return targetId != 0;
}

bool ServerMonsterFSMContext::HasTarget() const
{
    return mMonsterSystem.FindAlivePlayer(
        mMonster.targetPlayerId) != nullptr;
}

float ServerMonsterFSMContext::GetTargetDistanceSquared() const
{
    const ServerPlayer* target =
        mMonsterSystem.FindAlivePlayer(
            mMonster.targetPlayerId);

    if (target == nullptr)
    {
        return (std::numeric_limits<float>::max)();
    }

    return ServerMath::DistanceSquaredXZ(
        mMonster.position,
        target->position);
}

void ServerMonsterFSMContext::SelectRandomPatrolTarget(float radius)
{
    mMonsterSystem.SelectRandomPatrolTarget(
        mMonster,
        radius);
}

bool ServerMonsterFSMContext::MoveToPatrolTarget(
    float speed,
    float stoppingDistance)
{
    if (!mMonster.hasPatrolTarget)
        return false;

    return mMonsterSystem.MoveMonsterToward(
        mMonster,
        mMonster.patrolTarget,
        speed,
        stoppingDistance,
        mDeltaTime);
}

bool ServerMonsterFSMContext::MoveToTarget(
    float speed,
    float stoppingDistance)
{
    const ServerPlayer* target =
        mMonsterSystem.FindAlivePlayer(
            mMonster.targetPlayerId);

    if (target == nullptr)
    {
        mMonster.targetPlayerId = 0;
        return false;
    }

    return mMonsterSystem.MoveMonsterToward(
        mMonster,
        target->position,
        speed,
        stoppingDistance,
        mDeltaTime);
}

void ServerMonsterFSMContext::PlayAnimation(
    const std::string& animationName,
    bool loop)
{
    mMonsterSystem.ApplyMonsterAnimation(
        mMonster,
        animationName,
        loop);
}

bool ServerMonsterFSMContext::IsAnimationFinished() const
{
    if (mMonster.actionDuration <= 0.0f)
        return true;

    return mMonster.actionElapsedTime >=
        mMonster.actionDuration;
}

void ServerMonsterFSMContext::BeginMeleeAttack(
    const std::vector<std::string>& animationNames)
{
    mMonsterSystem.BeginMonsterMeleeAttack(
        mMonster,
        animationNames);
}

void ServerMonsterFSMContext::DestroyOwner()
{
    mMonster.destroyRequested = true;
}
