#pragma once
#include "MEComponent.h"

namespace ME
{
	namespace graphics
	{
		class Shader;
	}

	class BoxCollider2D;
	class CircleCollider2D;
	class BoxCollider3D;
	
	class Collider:public Component
	{

	public:
		
		Collider(enums::eColliderType type);
		~Collider();

		virtual bool Intersect(Collider* other) = 0;

		virtual bool IntersectWith(BoxCollider2D* other) { return false; }
		virtual bool IntersectWith(CircleCollider2D* other) { return false; }
		virtual bool IntersectWith(BoxCollider3D* other) { return false; }

		void Initialize() override;
		void Update()override;
		void LateUpdate()override;
		void Render()override;

		virtual void OnCollisionEnter(Collider* other);
		virtual void OnCollisionStay(Collider* other);
		virtual void OnCollisionExit(Collider* other);

		Vector3 GetOffset() const{ return mOffset; }
		void SetOffset(Vector3 offset) { mOffset = offset; }

		UINT32 GetID() const{ return mID; }
		Vector3 GetSize() const { return mSize; }
	
		void SetSize(Vector3 size) { mSize = size; }

		enums::eColliderType GetColliderType() const{ return mType; }

	protected:

		static UINT32 mCollisionID;

		UINT32 mID;
		Vector3 mOffset;
		Vector3 mSize;

		enums::eColliderType mType;

		std::weak_ptr<graphics::Shader> mWireShader;
	};

}
