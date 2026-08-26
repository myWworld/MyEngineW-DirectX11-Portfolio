#pragma once
#include "../MyEngine_Source/MEScenes.h"
#include "../MyEngine_Source/MESceneManager.h"
#include "../MyEngine_Source/MECamera.h"
#include "METile.h"


namespace ME
{
	class ToolScene :public Scene
	{
	public:

		ToolScene();
		~ToolScene();

		void Initialize() override;
		void Update()override;
		void LateUpdate()override;
		void Render()override;
		void Destroy()override;

		void OnEnter()override;
		void OnExit()override;

		void Save();
		void Load();

	private:
		std::vector<Tile*> mTiles;
		Camera* mCameraComp;
	};

}

LRESULT CALLBACK    WndTileProc(HWND, UINT, WPARAM, LPARAM);
