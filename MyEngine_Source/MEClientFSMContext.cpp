#include "MEClientFSMContext.h"

#include "MEAnimator3D.h"
#include "MEGameObject.h"
#include "MELayer.h"
#include "MEObject.h"
#include "MESceneManager.h"
#include "MEScenes.h"
#include "METransform.h"

#include <cmath>
#include <limits>
#include <random>

namespace ME
{
    namespace
    {
        constexpr float RadToDegree = 57.295779513f;

        std::mt19937& GetRandomEngine()
        {
            static std::mt19937 engine(std::random_device{}());

            return engine;
        }
    }

    ClientFSMContext::ClientFSMContext(GameObject* owner)
        : mOwner(owner)
    {
        CacheComponents();
    }

    void ClientFSMContext::SetDeltaTime( float deltaTime)
    {
        mDeltaTime = deltaTime;
    }

    float ClientFSMContext::GetDeltaTime() const
    {
        return mDeltaTime;
    }

    bool ClientFSMContext::DetectTarget(float radius)
    {
        CacheComponents();

        Scene* scene =
            SceneManager::GetActiveScene();

        if (scene == nullptr || mOwnerTransform == nullptr)
        {
            mTargetTransform = nullptr;
            return false;
        }

        Layer* playerLayer =
            scene->GetLayer(enums::eLayerType::Player);

        if (playerLayer == nullptr)
        {
            mTargetTransform = nullptr;
            return false;
        }

        const auto& players =
            playerLayer->GetGameObject();

        const math::Vector3 ownerPosition =
            mOwnerTransform->GetPosition();

        float nearestDistanceSquared =
            radius * radius;

        Transform* nearestTarget = nullptr;

        for (GameObject* player : players)
        {
            if (player == nullptr ||
                player == mOwner ||
                !player->IsActive())
            {
                continue;
            }

            Transform* transform =
                player->GetComponent<Transform>();

            if (transform == nullptr)
                continue;

            const float distanceSquared =
                math::Vector3::DistanceSquared( ownerPosition,transform->GetPosition());

            if (distanceSquared > nearestDistanceSquared)
            {
                continue;
            }

            nearestDistanceSquared = distanceSquared;

            nearestTarget = transform;
        }

        mTargetTransform = nearestTarget;

        return mTargetTransform != nullptr;
    }

    bool ClientFSMContext::HasTarget() const
    {
        return mTargetTransform != nullptr;
    }

    float ClientFSMContext::GetTargetDistanceSquared() const
    {
        if (mOwnerTransform == nullptr ||
            mTargetTransform == nullptr)
        {
            return (std::numeric_limits<float>::max)();
        }

        return math::Vector3::DistanceSquared(
            mOwnerTransform->GetPosition(),
            mTargetTransform->GetPosition()
        );
    }

    void ClientFSMContext::SelectRandomPatrolTarget(
        float radius)
    {
        CacheComponents();

        if (mOwnerTransform == nullptr)
            return;

        if (!mbHasPatrolOrigin)
        {
            mPatrolOrigin =
                mOwnerTransform->GetPosition();

            mbHasPatrolOrigin = true;
        }

        std::uniform_real_distribution<float>
            distribution(-radius, radius);

        mPatrolTarget =
            mPatrolOrigin +
            math::Vector3(
                distribution(GetRandomEngine()),
                0.0f,
                distribution(GetRandomEngine())
            );
    }

    bool ClientFSMContext::MoveToPatrolTarget(
        float speed,
        float stoppingDistance)
    {
        return MoveToPosition(
            mPatrolTarget,
            speed,
            stoppingDistance
        );
    }

    bool ClientFSMContext::MoveToTarget(
        float speed,
        float stoppingDistance)
    {
        if (mTargetTransform == nullptr)
            return false;

        return MoveToPosition(
            mTargetTransform->GetPosition(),
            speed,
            stoppingDistance
        );
    }

    void ClientFSMContext::PlayAnimation(
        const std::string& animationName, bool loop)
    {
        CacheComponents();

        if (mAnimator == nullptr ||
            animationName.empty())
        {
            return;
        }

        const std::wstring wideName(
            animationName.begin(),
            animationName.end()
        );

        auto* targetAnimation =
            mAnimator->FindAnimation(wideName);

        if (targetAnimation == nullptr)
            return;

        if (mAnimator->GetActiveAnimation()
            != targetAnimation)
        {
            mAnimator->PlayAnimation(wideName, loop);
        }
    }

    bool ClientFSMContext::IsAnimationFinished() const
    {
        return mAnimator != nullptr &&  mAnimator->IsAnimationComplete();
    }

    void ClientFSMContext::BeginMeleeAttack(
        const std::vector<std::string>& animationNames)
    {
        if (animationNames.empty())
            return;

        std::uniform_int_distribution<std::size_t>
            distribution(0, animationNames.size() - 1);

        const std::string& animationName =
            animationNames[distribution(GetRandomEngine())];

        PlayAnimation(animationName, false);
    }

    void ClientFSMContext::DestroyOwner()
    {
        if (mOwner)
        {
            object::Destroy(mOwner);
        }
    }

    void ClientFSMContext::CacheComponents()
    {
        if (mOwner == nullptr)
            return;

        if (mOwnerTransform == nullptr)
        {
            mOwnerTransform =
                mOwner->GetComponent<Transform>();
        }

        if (mAnimator == nullptr)
        {
            mAnimator =
                mOwner->GetComponent<Animator3D>();
        }
    }

    bool ClientFSMContext::MoveToPosition(
        const math::Vector3& targetPosition,
        float speed,
        float stoppingDistance)
    {
        CacheComponents();

        if (mOwnerTransform == nullptr)
            return false;

        math::Vector3 currentPosition =
            mOwnerTransform->GetPosition();

        math::Vector3 direction =
            targetPosition - currentPosition;

        direction.y = 0.0f;

        const float distanceSquared = direction.LengthSquared();

        if (distanceSquared <= stoppingDistance * stoppingDistance)
        {
            return false;
        }

        if (distanceSquared < 0.0001f)
            return false;

        direction.Normalize();

        const float yaw =
            std::atan2(
                direction.x,
                direction.z
            ) * RadToDegree + 180.0f;

        math::Vector3 rotation =
            mOwnerTransform->GetRotation();

        rotation.y = yaw;

        mOwnerTransform->SetRotation(rotation);

        currentPosition += direction * speed * mDeltaTime;

        mOwnerTransform->SetPosition(currentPosition);

        return true;
    }
}