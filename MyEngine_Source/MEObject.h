#pragma once
#include "MEComponent.h"
#include "MESceneManager.h"
#include "MEScenes.h"
#include "CommonInclude.h"
#include "MEGameObject.h"
#include "METransform.h"
#include "MELayer.h"

namespace ME::object
{
	template <typename T>
	 T* Instantiate(ME::enums::eLayerType type)
	{
		std::unique_ptr<T> gameObject = std::make_unique<T>();
		gameObject->SetLayerType(type);
		T* gameObjectPtr = gameObject.get();

		Scene* activeScene = SceneManager::GetActiveScene();
		Layer* layer = activeScene->GetLayer(type);
		
		layer->AddGameObject(std::move(gameObject));

		return gameObjectPtr;
		
	}

	template <typename T>
	 T* Instantiate(ME::enums::eLayerType type, math::Vector3 position)
	{
		T* gameObjectPtr = Instantiate<T>(type);

		Transform* tr = gameObjectPtr->GetComponent<Transform>();
		tr->SetPosition(position);


		 
		return gameObjectPtr;

	}

	static void Destroy(GameObject* obj)
	{
		if (obj != nullptr)
		{
			obj->SetDeath();
		}
	}

	static void DontDestroyOnLoad(GameObject* gameObject)
	{
		Scene* activeScene = SceneManager::GetActiveScene();

		enums::eLayerType layerType = gameObject->GetLayerType();

		//현재 씬의 레이어에서 해당 객체의 소유권을 찾아옴
		std::unique_ptr<GameObject> movedObj = activeScene->GetLayer(layerType)->ExtractGameObject(gameObject);
		
		if (movedObj)
		{
			// DontDestroyOnLoad 씬으로 소유권을 완전히 이전
			Scene* dontDestroyOnLoad = SceneManager::GetDontDestroyOnLoad();
			dontDestroyOnLoad->AddGameObject(std::move(movedObj), layerType);
		}
	}


}