#pragma once
#include "CommonInclude.h"
#include "MEScenes.h"

namespace ME
{
	class SceneManager
	{
	public:
		template<typename T>
		static Scene* CreateScene(const std::wstring& name)
		{
			std::unique_ptr<T> scene = std::make_unique<T>();
			scene->SetName(name);
			scene->Initialize();
			T* scenePtr = scene.get();

			mScene.insert({ name,std::move(scene)});

			return scenePtr;
		}


		static bool SetActiveScene(const std::wstring& name);
		static Scene* LoadScene(const std::wstring& name);
		
		static Scene* GetActiveScene() { return mActiveScene; }
		static Scene* GetDontDestroyOnLoad() {return mDontDestroyOnLoad;}

		static std::vector<GameObject*> GetGameObject(enums::eLayerType layer);

		static void Initialize();
		static void Update();
		static void LateUpdate();
		static void Render();
		static void Destroy();

		static void Release();

	private:
		
		static std::unordered_map<std::wstring, std::unique_ptr<Scene>> mScene;
		static Scene* mActiveScene;
		static Scene* mDontDestroyOnLoad;
	};

}

