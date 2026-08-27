#include "ServerMath.h"

#include <algorithm>
#include <cmath>
#include <utility>

namespace
{
    bool TestSegmentAxis(
        float start,
        float delta,
        float boxMin,
        float boxMax,
        float& inOutMinT,
        float& inOutMaxT)
    {
        constexpr float Epsilon = 0.000001f;

        if (std::abs(delta) < Epsilon)
        {
            return start >= boxMin && start <= boxMax;
        }

        const float inverseDelta = 1.0f / delta;
        float t1 = (boxMin - start) * inverseDelta;
        float t2 = (boxMax - start) * inverseDelta;

        if (t1 > t2)
        {
            std::swap(t1, t2);
        }

        inOutMinT = (std::max)(inOutMinT, t1);
        inOutMaxT = (std::min)(inOutMaxT, t2);

        return inOutMinT <= inOutMaxT;
    }
}

namespace ServerMath
{
    bool IsFinite(float value)
    {
        return std::isfinite(value);
    }

    bool IsFinite(const ServerVec3& value)
    {
        return std::isfinite(value.x) &&
            std::isfinite(value.y) &&
            std::isfinite(value.z);
    }

    float LengthSquared(const ServerVec3& value)
    {
        return value.x * value.x +
            value.y * value.y +
            value.z * value.z;
    }

    bool Normalize(ServerVec3& value)
    {
        const float lengthSquared = LengthSquared(value);

        if (!std::isfinite(lengthSquared) || lengthSquared < 0.000001f)
        {
            return false;
        }

        const float inverseLength = 1.0f / std::sqrt(lengthSquared);
        value.x *= inverseLength;
        value.y *= inverseLength;
        value.z *= inverseLength;

        return true;
    }

    bool NormalizeXZ(ServerVec3& value)
    {
        value.y = 0.0f;

        const float lengthSquared =
            value.x * value.x + value.z * value.z;

        if (!std::isfinite(lengthSquared) || lengthSquared < 0.000001f)
        {
            return false;
        }

        const float inverseLength = 1.0f / std::sqrt(lengthSquared);
        value.x *= inverseLength;
        value.z *= inverseLength;

        return true;
    }

    ServerVec3 Add(const ServerVec3& lhs, const ServerVec3& rhs)
    {
        return
        {
            lhs.x + rhs.x,
            lhs.y + rhs.y,
            lhs.z + rhs.z
        };
    }

    ServerVec3 Multiply(const ServerVec3& value, float scalar)
    {
        return
        {
            value.x * scalar,
            value.y * scalar,
            value.z * scalar
        };
    }

    ServerVec3 PointOnSegment(
        const ServerVec3& start,
        const ServerVec3& end,
        float t)
    {
        return
        {
            start.x + (end.x - start.x) * t,
            start.y + (end.y - start.y) * t,
            start.z + (end.z - start.z) * t
        };
    }

    float DistanceSquaredXZ(
        const ServerVec3& lhs,
        const ServerVec3& rhs)
    {
        const float dx = rhs.x - lhs.x;
        const float dz = rhs.z - lhs.z;

        return dx * dx + dz * dz;
    }

    bool SegmentIntersectsAabb(
        const ServerVec3& start,
        const ServerVec3& end,
        const ServerAabb& box,
        float sweepRadius,
        float& outHitT)
    {
        const ServerVec3 expandedHalfExtent =
        {
            box.halfExtent.x + sweepRadius,
            box.halfExtent.y + sweepRadius,
            box.halfExtent.z + sweepRadius
        };

        const ServerVec3 boxMin =
        {
            box.center.x - expandedHalfExtent.x,
            box.center.y - expandedHalfExtent.y,
            box.center.z - expandedHalfExtent.z
        };

        const ServerVec3 boxMax =
        {
            box.center.x + expandedHalfExtent.x,
            box.center.y + expandedHalfExtent.y,
            box.center.z + expandedHalfExtent.z
        };

        const ServerVec3 delta =
        {
            end.x - start.x,
            end.y - start.y,
            end.z - start.z
        };

        float minT = 0.0f;
        float maxT = 1.0f;

        if (!TestSegmentAxis(
            start.x,
            delta.x,
            boxMin.x,
            boxMax.x,
            minT,
            maxT))
        {
            return false;
        }

        if (!TestSegmentAxis(
            start.y,
            delta.y,
            boxMin.y,
            boxMax.y,
            minT,
            maxT))
        {
            return false;
        }

        if (!TestSegmentAxis(
            start.z,
            delta.z,
            boxMin.z,
            boxMax.z,
            minT,
            maxT))
        {
            return false;
        }

        outHitT = minT;
        return true;
    }

    ServerAabb MakePlayerAabb(const ServerPlayer& player)
    {
        ServerAabb result = {};
        result.center =
        {
            player.position.x,
            player.position.y + player.colliderCenterOffsetY,
            player.position.z
        };
        result.halfExtent =
        {
            player.colliderRadius,
            player.colliderHalfHeight,
            player.colliderRadius
        };

        return result;
    }

    ServerAabb MakeMonsterAabb(const ServerMonster& monster)
    {
        ServerAabb result = {};
        result.center =
        {
            monster.position.x,
            monster.position.y + monster.colliderCenterOffsetY,
            monster.position.z
        };
        result.halfExtent =
        {
            monster.colliderRadius,
            monster.colliderHalfHeight,
            monster.colliderRadius
        };

        return result;
    }
}
