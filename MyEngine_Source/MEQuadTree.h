#pragma once
#include "CommonInclude.h"
#include "MECollider.h"


namespace ME
{
	class QuadTree
	{
	private:
		int mLevel;
		math::Vector2 mBoundsMin;
		math::Vector2 mBoundsMax;

		std::vector<Collider*> mObjects;
		QuadTree* mNodes[4];

		const int MAX_OBJECTS = 10;
		const int MAX_LEVELS = 5;

	public:

		QuadTree(int level, math::Vector2 boundsMin, math::Vector2 boundsMax);
		~QuadTree();

		// 매 프레임마다 트리를 비우기
		void Clear();

		// 화면을 4개의 자식 노드(구역)로 쪼개는 함수
		void Split();

		// 특정 충돌체가 4개의 구역 중 어디에 속하는지 인덱스(0~3)를 반환하는 함수
		int GetIndex(Collider* col);

		// 충돌체를 트리에 삽입하는 함수
		void Insert(Collider* col);

		// 특정 충돌체 주변(같은 구역)에 있는 다른 충돌체들의 목록만 반환하는 함수
		std::vector<Collider*> Retrieve(std::vector<Collider*>& returnObjects, Collider* col);

	};


}
