
#include "MEScenes.h"
#include "MECollisionManager.h"
#include "MESceneManager.h"

namespace ME
{
	Scene::Scene()
		:mLayers{}
	{
		Scene::createLayers();
	}
	Scene::~Scene()
	{
	
	}

	void Scene::Initialize()
	{
		for (auto& layer : mLayers)
		{
			if (layer == nullptr) continue;
			layer->Initialize();
		}
	
	}

	void Scene::Update()
	{

		for (auto& layer : mLayers)
		{
			if (layer == nullptr)
				continue;

			layer->Update();
		}
	}
	void Scene::LateUpdate()
	{

		for (auto& layer : mLayers)
		{
			if (layer == nullptr)
				continue;

			layer->LateUpdate();
		}
	}
	void Scene::Render()
	{
		
			for (auto& layer : mLayers)
			{
				if (layer == nullptr)
					continue;

				layer->Render();
			}
	}
	void Scene::Destroy()
	{
		for (auto& layer : mLayers)
		{
			if (layer == nullptr)
				continue;

			layer->Destroy();
		}
	}

	void Scene::OnEnter()
	{

	 }
	void Scene::OnExit()
	{
		CollisionManager::Clear();
	}

	void Scene::AddGameObject(std::unique_ptr<GameObject> gameObject, const enums::eLayerType type)
	{
		if (gameObject == nullptr) return;

		mLayers[(UINT)type]->AddGameObject(std::move(gameObject));
	}

	void Scene::AddRemotePlayer(UINT id, std::unique_ptr<GameObject> remotePlayer)
	{
		// 소유권을 넘기기(move) 전에 주소를 먼저 확보
		mRemotePlayers[id] = remotePlayer.get();

		//  소유권을 Layer로 
		mLayers[(UINT)enums::eLayerType::Player]->AddGameObject(std::move(remotePlayer));
	}

	void Scene::EraseRemotePlayer(UINT id)
	{

		auto it = mRemotePlayers.find(id);
		if (it != mRemotePlayers.end())
		{

			GameObject* player = it->second;

			EraseGameObject(player);

			mRemotePlayers.erase(it);
		}
	}

	void Scene::AddRemoteMonster(UINT id, std::unique_ptr<GameObject> monster)
	{
		if (!monster)
			return;

		mRemoteMonsters[id] =
			monster.get();

		mLayers[static_cast<UINT>(enums::eLayerType::Monster)]->AddGameObject(
			std::move(monster)
		);
	}

	void Scene::EraseRemoteMonster(UINT id)
	{
		auto iter =
			mRemoteMonsters.find(id);

		if (iter == mRemoteMonsters.end())
			return;

		GameObject* monster =
			iter->second;

		EraseGameObject(monster);

		mRemoteMonsters.erase(iter);
	}

	void Scene::EraseGameObject(GameObject* gameObj)
	{
		if (gameObj == nullptr)
			return;

		enums::eLayerType layerType = gameObj->GetLayerType();
		mLayers[(UINT)layerType]->EraseGameObject(gameObj);
	}

	void Scene::createLayers()
	{
		mLayers.resize((UINT)enums::eLayerType::Max);

		for (size_t i = 0; i < (UINT)enums::eLayerType::Max; i++)
		{
			mLayers[i] = std::make_unique<Layer>();
		}
	}
}