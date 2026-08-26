#pragma once
#include "MEEntity.h"

namespace ME
{


	class UIBase :public Entity
	{
	public:
		UIBase(enums::eUIType type);
		virtual ~UIBase();

		struct Event
		{
			void operator=(std::function<void()> func)
			{
				mEvent = std::move(func);
			}

			void operator()()
			{
				if (mEvent != nullptr)
					mEvent();
			}


			std::function<void()> mEvent;
		};

		void Initialize();
		void Render();
		void Active();

		void InActive();

		void Update();
		void LateUpdate();

		void UIClear();

		virtual void OnInit();
		virtual void OnActive();
		virtual void OnInActive();
		virtual void OnUpdate();
		virtual void OnLateUpdate();
		virtual void OnRender();
		virtual void OnClear();

		enums::eUIType GetType() { return mType; }
		void SetType(enums::eUIType type) { mType = type; }

		void SetFullScreen(bool enable) { mbFullScreen = enable; }
		bool IsFullScreen() { return mbFullScreen; }

		void SetPos(Vector2 pos) { mPosition = pos; }
		Vector2 GetPos() { return mPosition; }
		void SetSize(Vector2 size) { mSize = size; }
		Vector2 GetSize() { return mSize; }

		void SetOwner(class GameObject* owner) { this->mOwner = owner; }
		class GameObject* GetOwner() const { return mOwner; }
		void SetFollowOwner(bool follow) { mbFollowingOwner = follow; }

		math::Vector2 WolrdPosToScreenPos(math::Vector3& worldPos);

	protected:
		Vector2 mPosition;
		Vector2 mSize;
		bool mbMouseOn;

	private:

		enums::eUIType mType;
		bool mbFullScreen;
		bool mbFollowingOwner;
		bool mbEnabled;

		class GameObject* mOwner;

		UIBase* mParent;
	};

}
