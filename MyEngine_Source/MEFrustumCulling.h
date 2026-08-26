#pragma once
#include "CommonInclude.h"

namespace ME
{

	struct Plane
	{
		math::Vector3 normal;
		float d;


		float Dot(const math::Vector3& p) const
		{
			return normal.Dot(p) + d;
		}


		void Normalize()
		{
			float length = normal.Length();
			normal /= length;
			d /= length;
		}
	};

	class FrustumCulling
	{
	public:

		FrustumCulling();

		void ConstructFrustum(const math::Matrix& vp);
		bool CheckAABB(const DirectX::BoundingBox& box);

	private:

		ME::Plane mPlanes[6];
	};

}
