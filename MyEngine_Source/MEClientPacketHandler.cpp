#include "MEClientPacketHandler.h"
#include "MENetworkManager.h" // GetMyEntityId()를 위해 포함
#include "../MyEngine_Source/MESceneManager.h"
#include "../MyEngine_Source/MEScenes.h"
#include "../MyEngine_Source/METransform.h"
#include "../MyEngine_Source/MEObject.h"
#include "../MyEngine_Source/MEAnimator3D.h"
#include "../MyEngine_W/MEPlayerScript.h"
#include "../MyEngine_W/MERemotePlayerScript.h"
#include "../MyEngine_Source/MEFSMBrain.h"
#include "../MyEngine_W/MERemoteMonsterScript.h"
#include "../MyEngine_W/MEWeaponScript.h"
#include "../MyEngine_W/MEProjectileVisualManager.h"

#include <iostream>

namespace ME
{
	void ClientPacketHandler::Handle_S_AssignId(const std::vector<char>& packetData)
	{
		if (packetData.size() != sizeof(Pkt_S_AssignId)) return; // 안전 검사

		Pkt_S_AssignId pkt = {};
		std::memcpy(&pkt, packetData.data(), sizeof(pkt)); // memcpy로 안전하게 캐스팅

		NetworkManager::SetMyEntityId(pkt.entityId); // NetworkManager에 setter 추가 필요
		std::cout << "[네트워크] 내 entityId 할당: " << pkt.entityId << std::endl;
	}

	void ClientPacketHandler::Handle_S_State(const std::vector<char>& packetData)
	{
		if (packetData.size() != sizeof(Pkt_S_State)) return;


		Scene* activeScene = SceneManager::GetActiveScene();

		if (activeScene == nullptr) return;

		auto& remotePlayers = activeScene->GetRemotePlayers();

		Pkt_S_State pkt = {};
		std::memcpy(&pkt, packetData.data(), sizeof(pkt));

		auto iter = remotePlayers.find(pkt.entityId);

		if (iter == remotePlayers.end())
			return;

		GameObject* targetPlayer = iter->second;

		RemotePlayerScript* remoteScript = targetPlayer->GetComponent<RemotePlayerScript>();

		if (remoteScript)
		{
			remoteScript->ApplyState(pkt.state);
		}
	}

	void ClientPacketHandler::Handle_S_Enter(const std::vector<char>& packetData)
	{
		if (packetData.size() != sizeof(Pkt_S_Enter)) return;

		Scene* activeScene = SceneManager::GetActiveScene();

		if (activeScene == nullptr)
			return;

		Pkt_S_Enter enterPkt = {};
		std::memcpy(&enterPkt, packetData.data(), sizeof(enterPkt));

		auto& remotePlayers = activeScene->GetRemotePlayers();

		if (enterPkt.entityId == NetworkManager::GetMyEntityId())
			return;

		if (remotePlayers.find(enterPkt.entityId) != remotePlayers.end())
			return;

		auto dummyPlayer = std::make_unique<GameObject>();
		dummyPlayer->SetLayerType(enums::eLayerType::Player);


		std::wstring modelKey = L"";

		if (enterPkt.modelType == eModelType::Character)
			modelKey = L"CharacterModel";
		else if (enterPkt.modelType == eModelType::Mutant)
			modelKey = L"MutantModel";
		else if (enterPkt.modelType == eModelType::Alien)
			modelKey = L"AlienModel";

		activeScene->MakeCharacter(dummyPlayer.get(), modelKey);
		RemotePlayerScript* remoteScript = dummyPlayer->AddComponent<RemotePlayerScript>();

		WeaponScript* gun =
			activeScene->MakeWeapon(
				dummyPlayer.get(), L"PistolModel", L"LeftHand", 0.0f);

		WeaponScript* sword =
			activeScene->MakeWeapon(
				dummyPlayer.get(), L"SwordModel", L"LeftHand", 0.0f);

		remoteScript->RegisterWeapon(eWeaponType::Gun, gun);
		remoteScript->RegisterWeapon(eWeaponType::Sword, sword);

		remoteScript->ApplyWeaponChange(enterPkt.weaponType);

		remoteScript->ApplyState(enterPkt.state);

		remoteScript->ApplyMove(
			enterPkt.x, enterPkt.y, enterPkt.z, enterPkt.yaw
		);

		activeScene->AddRemotePlayer(enterPkt.entityId, std::move(dummyPlayer));

	}
	void ClientPacketHandler::Handle_S_Move(const std::vector<char>& packetData)
	{
		if (packetData.size() != sizeof(Pkt_S_Move)) return;

		Pkt_S_Move pkt = {};
		std::memcpy(&pkt, packetData.data(), sizeof(pkt));

		Scene* activeScene = SceneManager::GetActiveScene();
		if (!activeScene) return;

		auto& remotePlayers = activeScene->GetRemotePlayers();
		auto iter = remotePlayers.find(pkt.entityId);
		if (iter == remotePlayers.end()) return;

		GameObject* targetPlayer = iter->second;
		if (!targetPlayer) return;

		RemotePlayerScript* remoteScript = targetPlayer->GetComponent<RemotePlayerScript>();
		if (remoteScript)
		{
			remoteScript->ApplyMove(pkt.x, pkt.y, pkt.z, pkt.yaw);
		}
	}
	void ClientPacketHandler::Handle_S_Attack(const std::vector<char>& packetData)
	{

		if (packetData.size() != sizeof(Pkt_S_Attack)) return;

		Scene* activeScene = SceneManager::GetActiveScene();
		if (!activeScene) return;

		auto& remotePlayers = activeScene->GetRemotePlayers();

		Pkt_S_Attack pkt = {};
		std::memcpy(&pkt, packetData.data(), sizeof(pkt));

		auto iter = remotePlayers.find(pkt.entityId);
		if (iter == remotePlayers.end())
			return;

		GameObject* targetPlayer = iter->second;

		if (targetPlayer == nullptr) return;

		RemotePlayerScript* remoteScript = targetPlayer->GetComponent<RemotePlayerScript>();

		if (remoteScript)
		{
			remoteScript->ApplyAttack(pkt.weaponType, pkt.attackIndex, math::Vector3(pkt.dir_x, pkt.dir_y, pkt.dir_z));
		}

	}

	void ClientPacketHandler::Handle_S_WeaponChange(const std::vector<char>& packetData)
	{
		if (packetData.size() != sizeof(Pkt_S_WeaponChange)) return;

		Scene* activeScene = SceneManager::GetActiveScene();
		if (!activeScene) return;

		auto& remotePlayers = activeScene->GetRemotePlayers();

		Pkt_S_WeaponChange pkt = {};
		std::memcpy(&pkt, packetData.data(), sizeof(pkt));

		auto iter = remotePlayers.find(pkt.entityId);
		if (iter == remotePlayers.end())
			return;

		GameObject* targetPlayer = iter->second;

		if (targetPlayer == nullptr) return;

		RemotePlayerScript* remoteScript = targetPlayer->GetComponent<RemotePlayerScript>();

		if (remoteScript)
		{
			remoteScript->ApplyWeaponChange(pkt.weaponType);
		}

	}

	void ClientPacketHandler::Handle_S_Leave(const std::vector<char>& packetData)
	{


		if (packetData.size() != sizeof(Pkt_S_Leave)) return;

		Scene* activeScene = SceneManager::GetActiveScene();
		if (!activeScene) return;

		auto& remotePlayers = activeScene->GetRemotePlayers();

		Pkt_S_Leave pkt = {};
		std::memcpy(&pkt, packetData.data(), sizeof(pkt));

		auto iter = remotePlayers.find(pkt.entityId);

		if (iter != remotePlayers.end())
		{
			object::Destroy(iter->second);
			remotePlayers.erase(iter);
		}
	}

	void ClientPacketHandler::Handle_S_MonsterSpawn(const std::vector<char>& packetData)
	{
		if (packetData.size() != sizeof(Pkt_S_MonsterSpawn)) return;

		Scene* activeScene = SceneManager::GetActiveScene();

		if (activeScene == nullptr)
			return;

		Pkt_S_MonsterSpawn pkt = {};
		std::memcpy(&pkt, packetData.data(), sizeof(pkt));


		auto& remoteMonsters = activeScene->GetRemoteMonsters();

		if (remoteMonsters.find(pkt.entityId) != remoteMonsters.end())
		{
			return;
		}

		auto enemyDummy = std::make_unique<GameObject>();

		enemyDummy->SetLayerType(enums::eLayerType::Monster);

		std::wstring modelKey;

		switch (pkt.modelType)
		{
		case eModelType::Mutant:
			modelKey = L"MutantModel";
			break;

		case eModelType::Alien:
			modelKey = L"AlienModel";
			break;

		default:
			break;
		}

		if (modelKey.empty())
			return;

		activeScene->MakeCharacter(enemyDummy.get(), modelKey);

		RemoteMonsterScript* remoteScript = enemyDummy->AddComponent<RemoteMonsterScript>();

		WeaponScript* leftGauntlet =
			activeScene->MakeWeapon(enemyDummy.get(),L"GauntletModel", L"LeftHand", 0.0f);

		WeaponScript* rightGauntlet =
			activeScene->MakeWeapon(enemyDummy.get(), L"GauntletModel", L"RightHand", 0.0f);

		if (leftGauntlet)
		{
			leftGauntlet->SetSocketOffsetAntRot(math::Vector3(129.0f, 139.0f, -9.0f), math::Vector3::Zero);

			remoteScript->SetLeftWeapon(leftGauntlet);
		}

		if (rightGauntlet)
		{
			rightGauntlet->SetSocketOffsetAntRot(math::Vector3(-96.0f, 149.0f, 1.0f), math::Vector3::Zero);

			remoteScript->SetRightWeapon(rightGauntlet);
		}

		remoteScript->ApplyMove(pkt.x, pkt.y, pkt.z, pkt.yaw);

		remoteScript->ApplyState(pkt.state);

		activeScene->AddRemoteMonster(pkt.entityId, std::move(enemyDummy));
	}

	void ClientPacketHandler::Handle_S_MonsterMove(const std::vector<char>& packetData)
	{
		if (packetData.size() != sizeof(Pkt_S_MonsterMove)) return;

		Scene* activeScene = SceneManager::GetActiveScene();

		if (activeScene == nullptr)
			return;

		Pkt_S_MonsterMove pkt = {};
		std::memcpy(&pkt, packetData.data(), sizeof(pkt));


		auto& remoteMonsters = activeScene->GetRemoteMonsters();

		auto iter = remoteMonsters.find(pkt.entityId);

		if (iter == remoteMonsters.end())
			return;

		RemoteMonsterScript* script = iter->second->GetComponent<RemoteMonsterScript>();

		if (script)
		{
			script->ApplyMove( pkt.x, pkt.y, pkt.z, pkt.yaw);
		}
	}

	void ClientPacketHandler::Handle_S_MonsterState(const std::vector<char>& packetData)
	{
		if (packetData.size() != sizeof(Pkt_S_MonsterState)) return;

		Scene* activeScene = SceneManager::GetActiveScene();

		if (activeScene == nullptr)
			return;

		Pkt_S_MonsterState pkt = {};
		std::memcpy(&pkt, packetData.data(), sizeof(pkt));


		auto& remoteMonsters = activeScene->GetRemoteMonsters();

		auto iter = remoteMonsters.find(pkt.entityId);

		if (iter == remoteMonsters.end())
			return;

		RemoteMonsterScript* script = iter->second->GetComponent<RemoteMonsterScript>();

		if (script)
		{
			script->ApplyState(pkt.state);
		}
	}

	void ClientPacketHandler::Handle_S_MonsterAttack(const std::vector<char>& packetData)
	{
		if (packetData.size() != sizeof(Pkt_S_MonsterAttack)) return;

		Scene* activeScene = SceneManager::GetActiveScene();

		if (activeScene == nullptr)
			return;

		Pkt_S_MonsterAttack pkt = {};
		std::memcpy(&pkt, packetData.data(), sizeof(pkt));


		auto& remoteMonsters = activeScene->GetRemoteMonsters();

		auto iter = remoteMonsters.find(pkt.entityId);

		if (iter == remoteMonsters.end())
			return;

		RemoteMonsterScript* script = iter->second->GetComponent<RemoteMonsterScript>();

		if (script)
		{
			script->ApplyAttack(pkt.attackIndex, math::Vector3(pkt.dir_x, pkt.dir_y, pkt.dir_z));
		}
	}

	void ClientPacketHandler::Handle_S_MonsterDespawn(const std::vector<char>& packetData)
	{
		if (packetData.size() != sizeof(Pkt_S_MonsterDespawn)) return;

		Scene* activeScene = SceneManager::GetActiveScene();

		if (activeScene == nullptr)
			return;

		Pkt_S_MonsterDespawn pkt = {};
		std::memcpy(&pkt, packetData.data(), sizeof(pkt));


		activeScene->EraseRemoteMonster(pkt.entityId);
	}

	void ClientPacketHandler::Handle_S_ProjectileSpawn(const std::vector<char>& packetData)
	{
		if (packetData.size() != sizeof(Pkt_S_ProjectileSpawn)) return;

		Pkt_S_ProjectileSpawn pkt = {};
		std::memcpy(&pkt, packetData.data(), sizeof(pkt));

		ProjectileVisualManager::Spawn(
			pkt.projectileId, pkt.ownerEntityId,
			math::Vector3(pkt.start_x, pkt.start_y, pkt.start_z),
			math::Vector3(pkt.velocity_x, pkt.velocity_y, pkt.velocity_z),
			pkt.lifeTime);
	}

	void ClientPacketHandler::Handle_S_ProjectileEnd(const std::vector<char>& packetData)
	{
		if (packetData.size() != sizeof(Pkt_S_ProjectileEnd)) return;

		Pkt_S_ProjectileEnd pkt = {};
		std::memcpy(&pkt, packetData.data(), sizeof(pkt));

		ProjectileVisualManager::End(
			pkt.projectileId,
			math::Vector3(pkt.end_x, pkt.end_y, pkt.end_z),
			pkt.reason
		);
	}

	void ClientPacketHandler::Handle_S_Damage(const std::vector<char>& packetData)
	{
		if (packetData.size() != sizeof(Pkt_S_Damage))
		{
			return;
		}

		Scene* activeScene = SceneManager::GetActiveScene();

		if (activeScene == nullptr)
			return;

		Pkt_S_Damage packet = {};

		std::memcpy(&packet, packetData.data(), sizeof(packet));

		const bool isDead = packet.isDead != 0;

		const math::Vector3 hitPosition(packet.hit_x, packet.hit_y, packet.hit_z);
		


		auto& remotePlayers = activeScene->GetRemotePlayers();

		auto& remoteMonsters = activeScene->GetRemoteMonsters();

		if (packet.victimId == NetworkManager::GetMyEntityId())//나일 경우
		{
			GameObject* localPlayer = activeScene->GetLocalPlayer();

			if (localPlayer)
			{
				PlayerScript* script = localPlayer->GetComponent<PlayerScript>();

				if (script)
				{
					script->ApplyServerDamage(packet.remainingHp, isDead, hitPosition);
				}
			}

			return;
		}

		auto playerIter = remotePlayers.find(packet.victimId);

		if (playerIter != remotePlayers.end())
		{
			RemotePlayerScript* script = playerIter->second->GetComponent<RemotePlayerScript>();

			if (script)
			{
				script->ApplyServerDamage(packet.remainingHp, isDead, hitPosition);
			}

			return;
		}

		auto monsterIter = remoteMonsters.find(packet.victimId);

		if (monsterIter != remoteMonsters.end())
		{
			RemoteMonsterScript* script = monsterIter->second->GetComponent<RemoteMonsterScript>();

			if (script)
			{
				script->ApplyServerDamage(packet.remainingHp, isDead, hitPosition);
			}
		}
	}
}