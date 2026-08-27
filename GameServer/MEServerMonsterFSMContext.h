#pragma once

#include "../MyEngine_Source/IFSMContext.h"
#include "ServerTypes.h"

class ServerMonsterSystem;

// FSM Core는 ServerWorld 전체가 아니라 몬스터 시스템의 좁은 API만 사용한다.
class ServerMonsterFSMContext final : public ME::IFSMContext
{
public:
    ServerMonsterFSMContext(
        ServerMonsterSystem& monsterSystem,
        ServerMonster& monster,
        float deltaTime);

    float GetDeltaTime() const override;

    bool DetectTarget(float radius) override;
    bool HasTarget() const override;
    float GetTargetDistanceSquared() const override;

    void SelectRandomPatrolTarget(float radius) override;

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
    ServerMonsterSystem& mMonsterSystem;
    ServerMonster& mMonster;
    float mDeltaTime = 0.0f;
};
