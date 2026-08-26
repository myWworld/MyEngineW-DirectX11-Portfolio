#pragma once

#include "../MyEngine_Source/MEScript.h"
#include "../MyEngine_Source/MEMath.h"
#include "../MyEngine_Source/Protocol.h"

namespace ME
{
    class NetworkProjectileScript final: public Script
    {
    public:
        NetworkProjectileScript();
        ~NetworkProjectileScript();

        void Initialize() override;
        void Update() override;
        void LateUpdate() override;
        void Render() override;

        void Launch(
            ProjectileId projectileId,
            EntityId ownerEntityId,
            const math::Vector3& velocity,
            float lifeTime);

        void ResetForPool();

        ProjectileId GetProjectileId() const
        {
            return mProjectileId;
        }

        EntityId GetOwnerEntityId() const
        {
            return mOwnerEntityId;
        }

    private:
        ProjectileId mProjectileId = 0;
        EntityId mOwnerEntityId = 0;

        math::Vector3 mVelocity = math::Vector3::Zero;

        float mRemainingLife = 0.0f;

        bool mbLaunched = false;
        bool mbReturnRequested = false;
    };
}