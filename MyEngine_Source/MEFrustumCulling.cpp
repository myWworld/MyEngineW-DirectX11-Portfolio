#include "MEFrustumCulling.h"
#include <DirectXCollision.h>

namespace ME
{

	FrustumCulling::FrustumCulling() {}
		

	void FrustumCulling::ConstructFrustum(const math::Matrix& vp)
	{
		//클립 스페이스 = 월드 Mat * View Mat
        // Left Plane
        mPlanes[0].normal.x = vp._14 + vp._11;
        mPlanes[0].normal.y = vp._24 + vp._21;
        mPlanes[0].normal.z = vp._34 + vp._31;
        mPlanes[0].d = vp._44 + vp._41;

        // Right Plane
        mPlanes[1].normal.x = vp._14 - vp._11;
        mPlanes[1].normal.y = vp._24 - vp._21;
        mPlanes[1].normal.z = vp._34 - vp._31;
        mPlanes[1].d = vp._44 - vp._41;

        // Bottom Plane
        mPlanes[2].normal.x = vp._14 + vp._12;
        mPlanes[2].normal.y = vp._24 + vp._22;
        mPlanes[2].normal.z = vp._34 + vp._32;
        mPlanes[2].d = vp._44 + vp._42;

        // Top Plane
        mPlanes[3].normal.x = vp._14 - vp._12;
        mPlanes[3].normal.y = vp._24 - vp._22;
        mPlanes[3].normal.z = vp._34 - vp._32;
        mPlanes[3].d = vp._44 - vp._42;

        // Near Plane 
        mPlanes[4].normal.x = vp._13;
        mPlanes[4].normal.y = vp._23;
        mPlanes[4].normal.z = vp._33;
        mPlanes[4].d = vp._43;

        // Far Plane
        mPlanes[5].normal.x = vp._14 - vp._13;
        mPlanes[5].normal.y = vp._24 - vp._23;
        mPlanes[5].normal.z = vp._34 - vp._33;
        mPlanes[5].d = vp._44 - vp._43;

        // 정규화
        for (int i = 0; i < 6; ++i) mPlanes[i].Normalize();
	}

	bool FrustumCulling::CheckAABB(const DirectX::BoundingBox& box)
	{
        for (int i = 0; i < 6; ++i)
        {
            math::Vector3 positiveVertex(box.Center.x, box.Center.y, box.Center.z);
            positiveVertex.x += (mPlanes[i].normal.x >= 0.0f) ? box.Extents.x : -box.Extents.x;
            positiveVertex.y += (mPlanes[i].normal.y >= 0.0f) ? box.Extents.y : -box.Extents.y;
            positiveVertex.z += (mPlanes[i].normal.z >= 0.0f) ? box.Extents.z : -box.Extents.z;

            // 평면 뒤점 체크 -> 시야 밖
            if (mPlanes[i].Dot(positiveVertex) < 0) {
                return false; 
            }
        }
        return true;
        
	}
}