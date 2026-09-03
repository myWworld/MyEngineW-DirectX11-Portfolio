#include "MEGameObject.h"
#include "MEInput.h"
#include "METime.h"
#include "MEApplication.h"
#include "METransform.h"

using namespace std;

namespace ME {


	GameObject::GameObject()
		:mState(eState::Active)
		, mLayerType(enums::eLayerType::None)
	{
		mComponents.resize((UINT)enums::eComponentType::End);
		initializeTransform();
	}

	GameObject::~GameObject() 
	{

	}


	void GameObject::Initialize()
	{
		for (auto& comp : mComponents)
		{

			if (comp == nullptr)
				continue;

			comp->Initialize();
		}
	}

	void GameObject::Update()
	{
		for (auto& comp : mComponents)
		{
			if (comp == nullptr)
				continue;

			comp->Update();
		}
	}

	void GameObject::LateUpdate()
	{
		for (auto& comp : mComponents)
		{
			if (comp == nullptr)
				continue;


			comp->LateUpdate();
		}
	}
	

	void GameObject::Render()
	{

		for (auto& comp : mComponents)
		{
			if (comp == nullptr)
				continue;


			comp->Render();
		} 

	}
	
	void GameObject::initializeTransform()
	{
		AddComponent<Transform>();
	}


}