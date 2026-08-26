#pragma once
#include "CommonInclude.h"
#include "MEBoxCollider2D.h"
#include "MECircleCollider2D.h"
#include "METransform.h"
#include "MEGameObject.h"
#include "MEQuadTree.h"


namespace ME
{

	union CollisionID
	{
		struct
		{
			UINT32 left;
			UINT32 right;
		};

		UINT64 id;
	};

	struct CollisionInfo
	{
		bool isColliding;
		Collider* left;
		Collider* right;
	};

	class CollisionManager
	{
	public:

		static void Iniatialize();
		static void Update();
		static void LateUpdate();
		static void Render();

		static void CollisionLayerCheck(enums::eLayerType left, enums::eLayerType right, bool enable);
		static void LayerCollision( enums::eLayerType left, enums::eLayerType right);
		static void ColliderCollision(Collider* left, Collider* right);

		static bool Intersect(Collider* left, Collider* right);

		static void Clear();

	private:

		static QuadTree* mQuadTree;

		static std::bitset<(UINT)enums::eLayerType::Max> mCollisionLayerMatrix[(UINT)enums::eLayerType::Max];
		static std::unordered_map<UINT64, CollisionInfo> mCollisionMap;
		static std::unordered_set<UINT64> mCheckedThisFrame;
	
	};
}

