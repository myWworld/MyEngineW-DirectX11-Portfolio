#include "MELayer.h"

namespace ME
{
	ME::Layer::Layer()
	{
	}

	ME::Layer::~Layer()
	{

	}

	void ME::Layer::Initialize()
	{
		for (auto& gameObj : mGameObjects)
		{
			if (gameObj == nullptr)
				continue;

			gameObj->Initialize();
		}
	}

	void ME::Layer::Update()
		{
		for (auto& gameObj : mGameObjects)
		{
			if (gameObj == nullptr)
				continue;

			GameObject::eState state = gameObj->GetState();

			if (state == GameObject::eState::Dead
					|| state == GameObject::eState::Paused)
			{
				continue;
			}

			gameObj->Update();
		}

	}

	void ME::Layer::LateUpdate()
	{
		for (auto& gameObj : mGameObjects)
		{
			if (gameObj == nullptr)
				continue;


			GameObject::eState state = gameObj->GetState();

			if (state == GameObject::eState::Dead
				|| state == GameObject::eState::Paused)
			{
				continue;
			}

			gameObj->LateUpdate();
		}

	
	}

	void ME::Layer::Render()
	{
		for (auto& gameObj : mGameObjects)
		{
			if (gameObj == nullptr)
				continue;


			GameObject::eState state = gameObj->GetState();
			if (state == GameObject::eState::Dead
				|| state == GameObject::eState::Paused
				|| state == GameObject::eState::NoRender)
			{
				continue;
			}

			gameObj->Render();
		}

	}
	
	void Layer::Destroy()
	{
	
		eraseDeadGameObject();
	}

	void Layer::EraseGameObject(GameObject* obj)
	{

		std::erase_if(mGameObjects,
			[obj](std::unique_ptr<GameObject>& gameObj)
			{
				return gameObj.get() == obj;
			});
	}

	void ME::Layer::AddGameObject(std::unique_ptr<GameObject> gameObject)
	{

		if (gameObject == nullptr)
			return;

		mGameObjects.push_back(std::move(gameObject));

	}

	std::unique_ptr<GameObject> Layer::ExtractGameObject(GameObject* gameObj)
	{
		if (gameObj == nullptr)
			return nullptr;

		auto iter = std::find_if(mGameObjects.begin(), mGameObjects.end(),
			[gameObj](std::unique_ptr<GameObject>& obj) -> bool
			{
				return obj.get() == gameObj;
			});

		if (iter != mGameObjects.end())
		{
			std::unique_ptr<GameObject> extractedObj = std::move(*iter);
			mGameObjects.erase(iter);
			return extractedObj;
		}

		return nullptr;
	}


	void Layer::findDeadGameObjects(OUT std::vector<GameObject*>& gameObjs)
	{
		for (auto& gameObj : mGameObjects)
		{
			if (gameObj == nullptr)
				continue;

			GameObject::eState active = gameObj->GetState();

			if (active == GameObject::eState::Dead)
			{
				gameObjs.push_back(gameObj.get());
			}
		}
	}


	void Layer::eraseDeadGameObject()
	{
		std::erase_if(mGameObjects,
			[](std::unique_ptr<GameObject>& gameObj)
			{
				return (gameObj->IsDead());
			}
		);
	}

}