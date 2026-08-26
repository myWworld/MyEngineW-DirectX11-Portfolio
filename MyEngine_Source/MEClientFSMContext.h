#pragma once

#include "IFSMContext.h"
#include "MEMath.h"

namespace ME
{
    class Animator3D;
    class GameObject;
    class Transform;

    class ClientFSMContext final
        : public IFSMContext
    {
    public:
        explicit ClientFSMContext(
            GameObject* owner);

        void SetDeltaTime(float deltaTime);

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
        void CacheComponents();

        bool MoveToPosition(
            const math::Vector3& targetPosition,
            float speed,
            float stoppingDistance);

    private:
        GameObject* mOwner = nullptr;

        Transform* mOwnerTransform = nullptr;
        Transform* mTargetTransform = nullptr;

        Animator3D* mAnimator = nullptr;

        math::Vector3 mPatrolOrigin =
            math::Vector3::Zero;

        math::Vector3 mPatrolTarget =
            math::Vector3::Zero;

        float mDeltaTime = 0.0f;
        bool mbHasPatrolOrigin = false;
    };
}