#include "MEServerMonsterFSMContext.h"

#include "ServerWorld.h"

#include <limits>

ServerMonsterFSMContext::ServerMonsterFSMContext(
    ServerWorld& world,
    ServerMonster& monster,
    float deltaTime)
    : mWorld(world)
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
    const EntityId targetId = mWorld.FindClosestAlivePlayer(mMonster.position, radius);

    mMonster.targetPlayerId = targetId;

    return targetId != 0;
}

bool ServerMonsterFSMContext::HasTarget() const
{
    return mWorld.FindAlivePlayer(mMonster.targetPlayerId) != nullptr;
}

float ServerMonsterFSMContext::GetTargetDistanceSquared() const
{
    const ServerPlayer* target = mWorld.FindAlivePlayer(mMonster.targetPlayerId);

    if (target == nullptr)
    {
        return (std::numeric_limits<float>::max)();
    }

    return mWorld.DistanceSquaredXZ(
        mMonster.position,
        target->position
    );
}

void ServerMonsterFSMContext::SelectRandomPatrolTarget(float radius)
{
    mWorld.SelectRandomPatrolTarget(
        mMonster,
        radius
    );
}

bool ServerMonsterFSMContext::MoveToPatrolTarget(
    float speed,
    float stoppingDistance)
{
    if (!mMonster.hasPatrolTarget)
        return false;

    return mWorld.MoveMonsterToward(
        mMonster,
        mMonster.patrolTarget,
        speed,
        stoppingDistance,
        mDeltaTime
    );
}

bool ServerMonsterFSMContext::MoveToTarget(
    float speed,
    float stoppingDistance)
{
    const ServerPlayer* target = mWorld.FindAlivePlayer(mMonster.targetPlayerId);

    if (target == nullptr)
    {
        mMonster.targetPlayerId = 0;
        return false;
    }

    return mWorld.MoveMonsterToward(
        mMonster,
        target->position,
        speed,
        stoppingDistance,
        mDeltaTime
    );
}

void ServerMonsterFSMContext::PlayAnimation(
    const std::string& animationName, bool loop)
{
    mWorld.ApplyMonsterAnimation(
        mMonster,
        animationName,
        loop
    );
}

bool ServerMonsterFSMContext::IsAnimationFinished() const
{
    if (mMonster.actionDuration <= 0.0f)
        return true;

    return mMonster.actionElapsedTime >= mMonster.actionDuration;
}

void ServerMonsterFSMContext::BeginMeleeAttack(
    const std::vector<std::string>& animationNames)
{
    mWorld.BeginMonsterMeleeAttack(mMonster, animationNames);
}

void ServerMonsterFSMContext::DestroyOwner()
{
    mMonster.destroyRequested = true;
}