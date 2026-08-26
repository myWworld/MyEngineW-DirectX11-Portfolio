#pragma once
#include "../MyEngine_Source/MEScenes.h"
#include "../MyEngine_Source/MECamera.h"

namespace ME
{
	class Model;
	class WeaponScript;
	class TitleScene: public Scene
	{
	public:
		TitleScene();
		~TitleScene();

		void Initialize() override;
		void Update() override;
		void LateUpdate() override;
		void Render() override;


		void OnEnter() override;
		void OnExit() override;

		void MakeCharacter(GameObject* player, std::wstring_view modelName) override;
		WeaponScript* MakeWeapon(GameObject* player, std::wstring_view modelName, std::wstring socketName, float damage) override;

	private:


	private:

		class Camera* mCameraComp;
		class GameObject* mPlayer;
	
	};
}


