#pragma once
#include "CommonInclude.h"
#include "MEEntity.h"

namespace ME
{
	class GameObject;
	class Component : public Entity
	{
	public:
			
		Component(enums::eComponentType type);
		virtual ~Component();

		virtual void Initialize();
		virtual void Update();
		virtual void LateUpdate();
		virtual void Render();

		void SetOwner(GameObject* owner) { mOwner = owner; }
		GameObject* GetOwner() const{ return mOwner; }
		enums::eComponentType GetType() const{ return mType; }

	private:
		GameObject* mOwner;
		enums::eComponentType mType;

		Vector2 Rotate;
	};

}

