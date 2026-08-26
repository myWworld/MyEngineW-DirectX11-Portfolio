#include "MESceneManager.h"
#include "MEDontDestroyOnLoad.h"

namespace ME
{

	std::unordered_map<std::wstring, std::unique_ptr<Scene>> SceneManager::mScene = {};
	Scene* SceneManager::mActiveScene = nullptr;
	Scene* SceneManager::mDontDestroyOnLoad = nullptr;
		

	bool SceneManager::SetActiveScene(const std::wstring& name)
	{
		std::unordered_map<std::wstring, std::unique_ptr<Scene>>::iterator iter = mScene.find(name);

		if (iter == mScene.end())
			return false;

		mActiveScene = iter->second.get();
		return true;
	}

	void SceneManager::Initialize()
	{
		mDontDestroyOnLoad = CreateScene<Scene>(L"DontDestroyOnLoad");
	}

	Scene* SceneManager::LoadScene(const std::wstring& name)
	{
		if (mActiveScene)
			mActiveScene->OnExit();

		if (!(SetActiveScene(name)))
		{
			return nullptr;
		}


		mActiveScene->OnEnter();

		return mActiveScene;
	}


	std::vector<GameObject*> SceneManager::GetGameObject(enums::eLayerType layer)
	{
		std::vector<GameObject*> outList;


		const auto& activeObjs = mActiveScene->GetLayer(layer)->GetGameObject();
		for (auto& uPtr : activeObjs)
		{
			outList.push_back(uPtr); 
		}

		const auto& dontDestroyObjs = mDontDestroyOnLoad->GetLayer(layer)->GetGameObject();
		for (auto& uPtr : dontDestroyObjs)
		{
			outList.push_back(uPtr);
		}

		return outList;

	}

	 
	void SceneManager::Update()
	{
		mActiveScene->Update();
		mDontDestroyOnLoad->Update();
	}
	void SceneManager::LateUpdate()
	{
		mActiveScene->LateUpdate();
		mDontDestroyOnLoad->LateUpdate();
	}
	void SceneManager::Render()
	{
		mActiveScene->Render();
		mDontDestroyOnLoad->Render();
	}

	void SceneManager::Destroy()
	{
		mActiveScene->Destroy();
		mDontDestroyOnLoad->Destroy();
	}

	void SceneManager::Release()
	{
		mScene.clear();
	}
}
