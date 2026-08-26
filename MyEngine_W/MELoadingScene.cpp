#include "MELoadingScene.h"
#include "MERenderer.h"
#include "MESceneManager.h"
#include "MEResources.h"
#include "METexture.h"
#include "json.hpp"
#include "MEModel.h"
#include "METitleScene.h"
#include "BoneMapManager.h"
#include "../MyEngine_Source/StringUtility.h"
#include "../MyEngine_Source/MEAnimation3D.h"
#include "../MyEngine_Source/FSMFactory.h"


#include "MEApplication.h"


extern ME::Application application;

using json = nlohmann::json;

namespace ME
{
	LoadingScene::LoadingScene()
		:mbLoadCompleted(false)
		,mMutualExclusion()
		,mResourcesLoadThread()
	{
	}
	LoadingScene::~LoadingScene()
	{
		if (mResourcesLoadThread != nullptr)
		{
			// 스레드가 아직 실행 중이거나 합류(join) 대기 중이라면
			if (mResourcesLoadThread->joinable())
			{
				// 메인 스레드가 작업 스레드가 끝날 때까지 안전하게 대기한 후 파괴
				mResourcesLoadThread->join();
			}
			delete mResourcesLoadThread;
			mResourcesLoadThread = nullptr;
		}

	}
	void LoadingScene::Initialize()
	{
		mResourcesLoadThread = new std::thread(&LoadingScene::resourcesLoad,this, std::ref(mMutualExclusion));
	}
	void LoadingScene::Update()
	{

	}
	void LoadingScene::LateUpdate()
	{
	}
	void LoadingScene::Render()
	{
		if (mbLoadCompleted)
		{
			mResourcesLoadThread->join();

			SceneManager::LoadScene(L"TitleScene");
		}
	}
	void LoadingScene::Destroy()
	{
	}
	void LoadingScene::OnEnter()
	{
	}
	void LoadingScene::OnExit()
	{
	}
	void LoadingScene::resourcesLoad(std::mutex& m)
	{

		while (true)
		{
			if (application.IsLoaded() == true)
			{
				break;
			}
			std::this_thread::yield();
	
		}

		
		{
			std::lock_guard<std::mutex> lock(m);
		//	Resources::Load<graphics::Texture>(L"Character", L"..\\Resources\\characterBase.fbx");


			renderer::Initialize();
			FSMFactory::Initialize();
			std::ifstream file("..\\Resources\\ResourceList.json");
			json datas = json::parse(file);

			Resources::LoadFromJSON<graphics::Texture>(datas, "Texture");
			Resources::LoadFromJSON<Model>(datas, "Model");
			Resources::LoadFromJSON<Animation3D>(datas, "Animation");

			BoneMapManager::LoadJsonBoneMap("..\\Resources\\BoneMap.json", enums::eBoneProfile::Humanoid);
			
			SceneManager::CreateScene<TitleScene>(L"TitleScene");
		
		}

		SceneManager::SetActiveScene(L"LoadingScene");

		mbLoadCompleted = true;

	}


}