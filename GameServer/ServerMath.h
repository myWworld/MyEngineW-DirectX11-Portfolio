#pragma once

#include "ServerTypes.h"

namespace ServerMath
{
    bool IsFinite(float value);
    bool IsFinite(const ServerVec3& value);

    float LengthSquared(const ServerVec3& value);

    bool Normalize(ServerVec3& value);
    bool NormalizeXZ(ServerVec3& value);

    ServerVec3 Add(const ServerVec3& lhs, const ServerVec3& rhs);
    ServerVec3 Multiply(const ServerVec3& value, float scalar);

    ServerVec3 PointOnSegment(
        const ServerVec3& start,
        const ServerVec3& end,
        float t);

    float DistanceSquaredXZ(
        const ServerVec3& lhs,
        const ServerVec3& rhs);

    // 총알 또는 근접 공격을 선분 + 반경으로 검사한다.
    // 대상 AABB를 sweepRadius만큼 확장한 뒤 선분 교차를 구한다.
    bool SegmentIntersectsAabb(
        const ServerVec3& start,
        const ServerVec3& end,
        const ServerAabb& box,
        float sweepRadius,
        float& outHitT);

    ServerAabb MakePlayerAabb(const ServerPlayer& player);
    ServerAabb MakeMonsterAabb(const ServerMonster& monster);
}
