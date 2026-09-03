#pragma once
#include "CommonInclude.h"
#include "MEComponent.h"

namespace ME {
	
	class Model;
	class GameObject
	{
	public:

		friend void Destroy(GameObject* obj);

		enum class eState
		{
			Active,
			Paused,
			Dead,
			NoRender,
			End
		};

		GameObject();
		~GameObject();
		
		virtual void Initialize();
		virtual void Update();
		virtual void LateUpdate();
		virtual void Render();

		template <typename T, typename... Args>
		T* AddComponent(Args&&... args)
		{
			static_assert(std::is_base_of_v<Component, T>, "T must derive from Component");

			auto component = std::make_unique<T>(std::forward<Args>(args)...);

			component->SetOwner(this);
			component->Initialize();

			const auto index = static_cast<std::size_t>(component->GetType());

			if (index >= mComponents.size())
			{
				throw std::out_of_range("Invalid component type");
			}

			if (mComponents[index] != nullptr)
			{
				throw std::logic_error("Component type already exists");
			}

			T* componentPtr = component.get();

			mComponents[index] = std::move(component);

			return componentPtr;
		}

		template <typename T>
		T* GetComponent()
		{
			T* component = nullptr;
			
			for (const auto& comp : mComponents)
			{
					component = dynamic_cast<T*>(comp.get());
					if (component)
					{
						break;
					}
			}

				return component;
		}

		void SetActive(bool power)
		{
			if (power == true) mState = eState::Active;
			if (power == false) mState = eState::Paused;
		}

		void SetNoRender(bool render)
		{
			if (render == true) mState = eState::Active;
			if (render == false) mState = eState::NoRender;
		}

		bool IsNoRender()
		{
			if (mState == eState::NoRender) return true;
			else return false;
		}

		eState GetState() const{ return mState; }
		
		bool IsActive() const{ return mState == eState::Active;}
		bool IsDead() const{ return mState == eState::Dead; }

		void SetDeath() {mState = eState::Dead;}

		void SetLayerType(enums::eLayerType type) { mLayerType = type; }

		enums::eLayerType GetLayerType() const{ return mLayerType; }

		void SetModel(Model* model_)
		{
			model = model_;
		}

		void SetModel(std::shared_ptr<Model> model_)
		{
			model = model_.get();
		}

		Model* GetModel() { return model; }

	

	private:
		
		void initializeTransform(); 
		
	
	private:

		std::vector<std::unique_ptr<Component>> mComponents;
		Model* model;

		eState mState;
		enums::eLayerType mLayerType;

	};



	

}
	