#pragma once

#include "ServerTypes.h"

#include <unordered_map>
#include <vector>

// 서버 월드의 권위 상태를 한곳에 보관한다.
// 실제 수정은 기존과 동일하게 ServerWorld 단일 스레드에서만 수행한다.
struct ServerWorldState
{
    std::unordered_map<EntityId, ServerPlayer> players;
    std::unordered_map<EntityId, ServerMonster> monsters;
    std::unordered_map<ProjectileId, ServerProjectile> projectiles;

    // 맵 벽, 기둥 등의 서버 Proxy Collider
    std::vector<ServerStaticCollider> staticWorldColliders;

    ProjectileId nextProjectileId = 1;
    bool friendlyFire = false;
};
