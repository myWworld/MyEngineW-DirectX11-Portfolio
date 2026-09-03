#include "MENetworkProjectileScript.h"

#include "MEProjectileVisualManager.h"

#include "../MyEngine_Source/MEGameObject.h"
#include "../MyEngine_Source/METime.h"
#include "../MyEngine_Source/METransform.h"

namespace ME
{
    NetworkProjectileScript::
        NetworkProjectileScript()
    {
    }

    NetworkProjectileScript::
        ~NetworkProjectileScript()
    {
    }

    void NetworkProjectileScript::Initialize()
    {
    }

    void NetworkProjectileScript::Update()
    {
        if (!mbLaunched)
            return;

        Transform* transform = GetOwner()->GetComponent<Transform>();

        if (transform == nullptr)
            return;

        math::Vector3 position = transform->GetPosition();

        position += mVelocity * Time::DeltaTime();

        transform->SetPosition(position);

        mRemainingLife -= Time::DeltaTime();

        // 서버 END 패킷을 못 받은 경우를 위한
        // 클라이언트 시각 오브젝트 안전장치
        if (mRemainingLife <= 0.0f && !mbReturnRequested)
        {
            mbReturnRequested = true;

            ProjectileVisualManager::ReturnByLifetime(mProjectileId);
        }
    }

    void NetworkProjectileScript::LateUpdate()
    {
    }

    void NetworkProjectileScript::Render()
    {
    }

    void NetworkProjectileScript::Launch(
        ProjectileId projectileId,
        EntityId ownerEntityId,
        const math::Vector3& velocity,
        float lifeTime)
    {
        mProjectileId = projectileId;
        mOwnerEntityId = ownerEntityId;

        mVelocity = velocity;
        mRemainingLife = lifeTime;

        mbLaunched = true;//발사 시작 플래그
        mbReturnRequested = false;
    }

    void NetworkProjectileScript::ResetForPool()//풀 반환전 발사체 객체에서 초기화 담당하여 책임 나눔
    {
        mProjectileId = 0;
        mOwnerEntityId = 0;

        mVelocity = math::Vector3::Zero;
        mRemainingLife = 0.0f;

        mbLaunched = false;
        mbReturnRequested = false;
    }
}