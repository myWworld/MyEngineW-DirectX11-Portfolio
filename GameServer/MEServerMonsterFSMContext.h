#pragma once

#include "../MyEngine_Source/IFSMContext.h"
#include "ServerTypes.h"

class ServerWorld;

class ServerMonsterFSMContext final : public ME::IFSMContext
{
public:
    ServerMonsterFSMContext(
        ServerWorld& world,
        ServerMonster& monster,
        float deltaTime);

    float GetDeltaTime() const override;

    bool DetectTarget(float radius) override;
    bool HasTarget() const override;
    float GetTargetDistanceSquared() const override;

    void SelectRandomPatrolTarget(
        float radius) override;

    bool MoveToPatrolTarget(
        float speed,
        float stoppingDistance) override;

    bool MoveToTarget(
        float speed,
        float stoppingDistance) override;

    void PlayAnimation(
        const std::string& animationName,
        bool loop) override;

    bool IsAnimationFinished() const override;

    void BeginMeleeAttack(
        const std::vector<std::string>& animationNames) override;

    void DestroyOwner() override;

private:
    ServerWorld& mWorld;
    ServerMonster& mMonster;

    float mDeltaTime;
};


