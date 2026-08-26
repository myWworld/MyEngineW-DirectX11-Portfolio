#pragma once

#include "../MyEngine_Source/MEMath.h"
#include "../MyEngine_Source/MEObjectPool.h"
#include "../MyEngine_Source/Protocol.h"

#include <cstddef>
#include <memory>
#include <unordered_map>

namespace ME
{
    class GameObject;

    class ProjectileVisualManager
    {
    public:
        static void Initialize(std::size_t poolSize = 100);

        static void Release();

        static void Spawn(
            ProjectileId projectileId,
            EntityId ownerEntityId,
            const math::Vector3& startPosition,
            const math::Vector3& velocity,
            float lifeTime);

        static void End(
            ProjectileId projectileId,
            const math::Vector3& endPosition,
            eProjectileEndReason reason);

        static void ReturnByLifetime(ProjectileId projectileId);

        static bool IsInitialized()
        {
            return mPool != nullptr;
        }

    private:
        static GameObject* CreateProjectile();

        static void ReturnInternal(
            ProjectileId projectileId,
            const math::Vector3* endPosition);

    private:
        static std::unique_ptr<ObjectPool<GameObject>> mPool;

        static std::unordered_map<ProjectileId, GameObject*> mActiveProjectiles;
    };
}