#pragma once
#include "../MyEngine_Source/MEScenes.h"
#include "../MyEngine_Source/MECamera.h"

namespace ME
{
	class PlayScene :public Scene
	{
	public:
		PlayScene();
		~PlayScene();

		void Initialize() override;
		void Update() override;
		void LateUpdate() override;
		void Render() override;


		void OnEnter() override;
		void OnExit() override;
		
	private:

		class Camera* mCameraComp;
		class GameObject* titleBg;

	};
}


