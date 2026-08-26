#pragma once
#include "MECollider.h"

namespace ME
{

	class BoxCollider3D :public Collider
	{
	public:


		BoxCollider3D();
		~BoxCollider3D();

		void Initialize() override;
		void Update()override;
		void LateUpdate()override;
		void Render()override;

		virtual bool Intersect(Collider* other) override { return other->IntersectWith(this); }

		virtual bool  IntersectWith(BoxCollider3D* other) override;

	public:

		void SetRotation(float rot) { mRot = rot; }
		Vector3 GetCentralPoint() { return mCentralPoint; }

		bool IsRotate() { return mbIsRotate; }
		float GetHeight() { return mHeight; }
		float GetWidth() { return mWidth; }

		POINT* GetPoint() { return mPoint; }

		bool CheckOBBCollision(
			const Vector3& centerA, const Vector3& extentsA, const Quaternion& rotA,
			const Vector3& centerB, const Vector3& extentsB, const Quaternion& rotB);

	private:

		Vector3 RotateCollider(float rot, int x, int y, int width, int height);

	private:

		float mRot;
		Vector3 mCentralPoint;

		float mWidth, mHeight;

		bool mbIsRotate;
		POINT* mPoint;


	};

}
