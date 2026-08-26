#pragma once

#include <string>
#include <vector>

namespace ME
{
    class IFSMContext
    {
    public:
        virtual ~IFSMContext() = default;

        virtual float GetDeltaTime() const = 0;

        // 타겟
        virtual bool DetectTarget(float radius) = 0;
        virtual bool HasTarget() const = 0;
        virtual float GetTargetDistanceSquared() const = 0;

        // 순찰
        virtual void SelectRandomPatrolTarget(float radius) = 0;

        // true: 아직 이동 중
        // false: 도착했거나 이동할 대상 없음
        virtual bool MoveToPatrolTarget(
            float speed,
            float stoppingDistance) = 0;

        virtual bool MoveToTarget(
            float speed,
            float stoppingDistance) = 0;

        // 표현 상태
        virtual void PlayAnimation(
            const std::string& animationName,
            bool loop) = 0;

        virtual bool IsAnimationFinished() const = 0;

        // 공격
        virtual void BeginMeleeAttack(
            const std::vector<std::string>& animationNames) = 0;

        // 제거
        virtual void DestroyOwner() = 0;
    };
}