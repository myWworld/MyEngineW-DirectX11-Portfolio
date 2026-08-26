#pragma once
#include "MECollider.h"
#include "CommonInclude.h"

namespace ME
{
	class BoxCollider2D;
	class CircleCollider2D:public Collider
	{
	public:

		CircleCollider2D();
		~CircleCollider2D();

		void Initialize() override;
		void Update()override;
		void LateUpdate()override;
		void Render()override;

		virtual bool Intersect(Collider* other) override { return other->IntersectWith(this); }

		virtual bool  IntersectWith(BoxCollider2D* other)override;
		virtual bool  IntersectWith(CircleCollider2D* other) override;


	public:

	private:
		float mRadius;

	};
}

