#pragma once
#include "../MyEngine_Source/CommonInclude.h"
#include "../MyEngine_Source/MESceneManager.h"

#include "METitleScene.h"

#include "METoolScene.h"
#include "MELoadingScene.h"
#include "MEPlayScene.h"


namespace ME
{
	void LoadScenes()
	{
		
		SceneManager::CreateScene<LoadingScene>(L"LoadingScene"); //초기에 가장 먼저 실행되는 로딩 씬 만들기
					//이때 내부 Initialize()에서 쓰레드로 리소스 로딩 시작
		//

		SceneManager::LoadScene(L"LoadingScene");//로딩 씬 실행
	

	}
}
