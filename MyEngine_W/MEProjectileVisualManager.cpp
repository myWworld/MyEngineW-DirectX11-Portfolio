#include "MEProjectileVisualManager.h"

#include "MENetworkProjectileScript.h"

#include "../MyEngine_Source/MEGameObject.h"
#include "../MyEngine_Source/MEModel.h"
#include "../MyEngine_Source/MEModelRenderer.h"
#include "../MyEngine_Source/MEObject.h"
#include "../MyEngine_Source/MEResources.h"
#include "../MyEngine_Source/METransform.h"

#include <cmath>

namespace ME
{
    std::unique_ptr<ObjectPool<GameObject>> ProjectileVisualManager::mPool = nullptr;

    std::unordered_map<
        ProjectileId,
        GameObject*>
        ProjectileVisualManager::mActiveProjectiles;

    void ProjectileVisualManager::Initialize(std::size_t poolSize)
    {
        if (mPool != nullptr)
            return;

        mPool = std::make_unique<ObjectPool<GameObject>>(poolSize,
                []() -> GameObject*
                {
                    return CreateProjectile();
                }
                );//총스크립트에 있던거 이동
    }

    void ProjectileVisualManager::Release()
    {
        for (auto& [projectileId, object] : mActiveProjectiles)
        {
            if (object != nullptr)
            {
                object->SetActive(false);
            }
        }

        mActiveProjectiles.clear();
        mPool.reset();
    }

    GameObject* ProjectileVisualManager::CreateProjectile()
    {
        GameObject* projectile = object::Instantiate<GameObject>(enums::eLayerType::Bullet, math::Vector3::Zero);

        if (projectile == nullptr)
            return nullptr;

        projectile->AddComponent<NetworkProjectileScript>();//로컬에서 사용되는 렌더링 전용 컴포넌트

        Transform* transform = projectile->GetComponent<Transform>();

        if (transform != nullptr)
        {
            transform->SetScale(0.1f, 0.1f,0.1f);
        }

        std::shared_ptr<Model> model = Resources::Find<Model>(L"BulletModel");

        if (model != nullptr)
        {
            ModelRenderer* renderer = projectile->AddComponent<ModelRenderer>();

            renderer->SetMesh(model->GetMeshes());

            projectile->SetModel(model);
        }

        // 서버가 실제 충돌을 처리하므로
        // Collider와 Rigidbody는 붙이지 않는다.

        return projectile;
    }

    void ProjectileVisualManager::Spawn(
        ProjectileId projectileId,
        EntityId ownerEntityId,
        const math::Vector3& startPosition,
        const math::Vector3& velocity,
        float lifeTime)
    {
        if (projectileId == 0)
            return;

        if (mPool == nullptr)
        {
            Initialize();
        }

        // 동일 ID가 이미 있다면 이전 시각 오브젝트 정리
        auto oldIter = mActiveProjectiles.find(projectileId);

        if (oldIter != mActiveProjectiles.end())
        {
            ReturnInternal(projectileId, nullptr);
        }

        GameObject* projectile = mPool->Get();

        if (projectile == nullptr)
            return;

        Transform* transform = projectile->GetComponent<Transform>();

        if (transform != nullptr)//시작 위치 발사 방향(회전) 결정
        {
            transform->SetPosition(startPosition);

            math::Vector3 flatDirection = velocity;

            if (flatDirection.LengthSquared() > 0.0001f)
            {
                flatDirection.Normalize();

                const float yaw = std::atan2(flatDirection.x,flatDirection.z);

                transform->SetRotation(math::Quaternion::CreateFromYawPitchRoll(yaw, 0.0f,0.0f));
            }
        }

        NetworkProjectileScript* script = projectile->GetComponent<NetworkProjectileScript>();

        if (script == nullptr)
        {
            mPool->Return(projectile);
            return;
        }

        script->Launch(projectileId, ownerEntityId, velocity, lifeTime);//실제 로컬에서 발사체 렌더링 시작

        mActiveProjectiles[projectileId] = projectile;
    }

    void ProjectileVisualManager::End(
        ProjectileId projectileId,
        const math::Vector3& endPosition,
        eProjectileEndReason reason)
    {
        // 여기에서 reason에 따라
        // 벽/플레이어/몬스터 피격 이펙트를
        // 추가할 수 있음

        ReturnInternal(projectileId, &endPosition);//풀 반환
    }

    void ProjectileVisualManager::ReturnByLifetime(ProjectileId projectileId)
    {
        ReturnInternal(projectileId, nullptr);
    }

    void ProjectileVisualManager::ReturnInternal(ProjectileId projectileId, const math::Vector3* endPosition)
    {
        auto iter = mActiveProjectiles.find(projectileId);

        if (iter == mActiveProjectiles.end())
        {
            return;
        }

        GameObject* projectile = iter->second;

        // 먼저 맵에서 제거
        mActiveProjectiles.erase(iter);

        if (projectile == nullptr)
            return;

        if (endPosition != nullptr)
        {
            Transform* transform = projectile ->GetComponent<Transform>();

            if (transform != nullptr)
            {
                transform->SetPosition(*endPosition);
            }
        }

        NetworkProjectileScript* script = projectile->GetComponent<NetworkProjectileScript>();

        if (script != nullptr)//풀반환전용 초기화
        {
            script->ResetForPool();
        }

        if (mPool != nullptr)
        {
            mPool->Return(projectile);
        }
        else
        {
            projectile->SetActive(false);
        }
    }
}