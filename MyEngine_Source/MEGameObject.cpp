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

	GameObject::~GameObject() {

		for (Component* comp : mComponents)
		{
			if (comp == nullptr)
				continue;

			delete comp;
			comp = nullptr;
		}
	}


	void GameObject::Initialize()
	{
		for (Component* comp : mComponents)
		{

			if (comp == nullptr)
				continue;

			comp->Initialize();
		}
	}

	void GameObject::Update()
	{
		for (Component* comp : mComponents)
		{
			if (comp == nullptr)
				continue;



			comp->Update();
		}
	}

	void GameObject::LateUpdate()
	{
		for (Component* comp : mComponents)
		{
			if (comp == nullptr)
				continue;


			comp->LateUpdate();
		}
	}
	

	void GameObject::Render()
	{

 		for (Component* comp : mComponents)
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