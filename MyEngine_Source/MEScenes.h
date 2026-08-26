#pragma once
#include "CommonInclude.h"
#include "MEEntity.h"
#include "MEGameObject.h"
#include "MELayer.h"



namespace ME 
{
	class GameObject;
	class WeaponScript;
	class Scene : public Entity
	{
	public:

		Scene();
		~Scene();

		virtual void Initialize();
		virtual void Update();
		virtual void LateUpdate();
		virtual void Render();
		virtual void Destroy();

		
		virtual void OnEnter();
		virtual void OnExit();

		void AddGameObject(std::unique_ptr<GameObject> gameObject, const enums::eLayerType type);
		void EraseGameObject(GameObject* gameObj);

		Layer* GetLayer(const enums::eLayerType type) { return mLayers[(UINT)type].get(); }
		
		std::unordered_map<UINT, GameObject*>& GetRemotePlayers() { return mRemotePlayers; }
		void AddRemotePlayer(UINT id, std::unique_ptr<GameObject> remotePlayer);
		void EraseRemotePlayer(UINT id);

		std::unordered_map<UINT, GameObject*>& GetRemoteMonsters()
		{
			return mRemoteMonsters;
		}

		void AddRemoteMonster(UINT id, std::unique_ptr<GameObject> monster);

		void EraseRemoteMonster(UINT id);

		void SetLocalPlayer(GameObject* player)
		{
			mLocalPlayer = player;
		}

		GameObject* GetLocalPlayer() const
		{
			return mLocalPlayer;
		}


		virtual void MakeCharacter(GameObject* player, std::wstring_view modelName) {}
		virtual WeaponScript* MakeWeapon(GameObject* player, std::wstring_view modelName, std::wstring socketName, float damage = 10.0f) {return nullptr;}

	private: 

		void createLayers();
	private:

		std::vector<std::unique_ptr<Layer>> mLayers;
		std::unordered_map<UINT, GameObject*> mRemotePlayers;
		std::unordered_map<UINT, GameObject*> mRemoteMonsters;
		GameObject* mLocalPlayer = nullptr;
	};

}
