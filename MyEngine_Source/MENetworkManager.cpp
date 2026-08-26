#include "MENetworkManager.h"

#include <iostream>

#pragma comment(lib, "ws2_32.lib")

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

namespace ME
{
	SOCKET NetworkManager::mClientSocket = INVALID_SOCKET;
	EntityId NetworkManager::mMyEntityId = 0;
	bool NetworkManager::mbIsHost = false;
	std::thread NetworkManager::mRecvThread = {}; // 스레드 객체
	std::queue<std::vector<char>> NetworkManager::mPacketQueue = {};
	std::mutex NetworkManager::mPacketMutex = {};
	
	std::atomic_bool NetworkManager::mbIsConnected = false;
	std::mutex NetworkManager::mSendMutex = {};


	bool NetworkManager::Initialize()
	{
		WSADATA wsaData;//win socket 초기화
		int startUpResult = WSAStartup(MAKEWORD(2, 2), &wsaData); //2.2버젼

		if (startUpResult != 0)
		{
			std::cerr << "클라이언트 winsocket 초기화실패! 에러 코드: " << startUpResult << std::endl;
			return false; // 실패 시 프로그램 종료
		}

		mClientSocket = socket(AF_INET, SOCK_STREAM, 0);//ipv4, tcp 프로토콜 사용
		if (mClientSocket == INVALID_SOCKET)
		{
			std::cerr << "클라이언트 소켓 생성 실패! 에러 코드: " << WSAGetLastError() << std::endl;
			WSACleanup();
			return false;
		}

		SOCKADDR_IN serverAddr = {}; //연결할 서버 세팅 ip주소와 포트 번호 (로컬 호스트: 127.0.0.1, 포트: 7777)
		serverAddr.sin_family = AF_INET;
		inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr);//로컬 환경이니까 
		serverAddr.sin_port = htons(7777);

		std::cout << "서버에 접속 시도 중..." << std::endl;

		if (connect(mClientSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) //존재하는 서버에 연결
		{
			std::cerr << "서버 연결 실패! 서버가 켜져 있는지 확인하세요. 에러 코드: " << WSAGetLastError() << std::endl;
			closesocket(mClientSocket);
			WSACleanup();
			return false;
		}

		std::cout << "서버 연결 성공!" << std::endl;
		mbIsConnected = true;
		mRecvThread = std::thread(RecvThread);
		
		return true;
	}

	void NetworkManager::Update()
	{
		std::queue<std::vector<char>> localPackets; //로컬로 스왑만 하고 락을 풀어주어 큐에 패킷을 빠르게 받아올수 있도록함

		{
			std::lock_guard<std::mutex> lock(mPacketMutex);

			std::swap(
				localPackets,
				mPacketQueue
			);
		}

		while (!localPackets.empty())
		{
			std::vector<char> packetData =
				std::move(
					localPackets.front()
				);

			localPackets.pop();

			if (packetData.size() <
				sizeof(PacketHeader))
			{
				continue;
			}

			PacketHeader header = {};

			std::memcpy(
				&header,
				packetData.data(),
				sizeof(PacketHeader)
			);

			Scene* activeScene =
				SceneManager::GetActiveScene();

			if (activeScene == nullptr)
				continue;

			auto& remotePlayers =
				activeScene->GetRemotePlayers();

			auto& remoteMonsters =
				activeScene->GetRemoteMonsters();

			switch (header.type)
			{
			case ePacketType::S_ASSIGN_ID:
			{
				Pkt_S_AssignId* pkt = reinterpret_cast<Pkt_S_AssignId*>(packetData.data());

				mMyEntityId = pkt->entityId;

				std::cout << "[네트워크] 내 entityId 할당: "
					<< mMyEntityId << std::endl;

				break;
			}

			case ePacketType::S_STATE:
			{
				Pkt_S_State* statePkt = reinterpret_cast<Pkt_S_State*>(packetData.data());

				auto iter = remotePlayers.find(statePkt->entityId);
				if (iter == remotePlayers.end())
					break;

				GameObject* targetPlayer = iter->second;

				RemotePlayerScript* remoteScript =
					targetPlayer->GetComponent<RemotePlayerScript>();

				if (remoteScript)
				{
					remoteScript->ApplyState(statePkt->state);
				}

				break;
			}
			case ePacketType::S_ENTER:
			{
				Pkt_S_Enter* enterPkt = reinterpret_cast<Pkt_S_Enter*>(packetData.data());

				if (enterPkt->entityId == mMyEntityId)
					break;

				if (remotePlayers.find(enterPkt->entityId) != remotePlayers.end())
					break;

				auto dummyPlayer = std::make_unique<GameObject>();
				dummyPlayer->SetLayerType(enums::eLayerType::Player);
			

				std::wstring modelKey = L"";

				if (enterPkt->modelType == eModelType::Character)
					modelKey = L"CharacterModel";
				else if (enterPkt->modelType == eModelType::Mutant)
					modelKey = L"MutantModel";
				else if (enterPkt->modelType == eModelType::Alien)
					modelKey = L"AlienModel";

		/*		std::wstring weaponKey = L"";

				if (enterPkt->weaponType == eWeaponType::Sword)
					weaponKey = L"SwordModel";
				else if (enterPkt->weaponType == eWeaponType::Gun)
					weaponKey = L"PistolModel";*/

				activeScene->MakeCharacter(dummyPlayer.get(), modelKey);
				RemotePlayerScript* remoteScript = dummyPlayer->AddComponent<RemotePlayerScript>();

				WeaponScript* gun =
					activeScene->MakeWeapon(
						dummyPlayer.get(),
						L"PistolModel",
						L"LeftHand",
						0.0f
					);

				WeaponScript* sword =
					activeScene->MakeWeapon(
						dummyPlayer.get(),
						L"SwordModel",
						L"LeftHand",
						0.0f
					);

				remoteScript->RegisterWeapon(
					eWeaponType::Gun,
					gun
				);

				remoteScript->RegisterWeapon(
					eWeaponType::Sword,
					sword
				);

				remoteScript->ApplyWeaponChange(
					enterPkt->weaponType
				);

				remoteScript->ApplyState(
					enterPkt->state
				);

				remoteScript->ApplyMove(
					enterPkt->x,
					enterPkt->y,
					enterPkt->z,
					enterPkt->yaw
				);

				activeScene->AddRemotePlayer(enterPkt->entityId, std::move(dummyPlayer));

				break;
			}

			case ePacketType::S_MOVE:
			{
				Pkt_S_Move* movePkt = reinterpret_cast<Pkt_S_Move*>(packetData.data());

				auto iter = remotePlayers.find(movePkt->entityId);
				if (iter == remotePlayers.end())
					break;

				GameObject* targetPlayer = iter->second;

				if (targetPlayer == nullptr) return;

				RemotePlayerScript* remoteScript =
					targetPlayer->GetComponent<RemotePlayerScript>();

				if (remoteScript)
				{
					remoteScript->ApplyMove(
						movePkt->x,
						movePkt->y,
						movePkt->z,
						movePkt->yaw
					);
				}

				break;
			}

			case ePacketType::S_ATTACK:
			{
				Pkt_S_Attack* attackPkt = reinterpret_cast<Pkt_S_Attack*>(packetData.data());

				auto iter = remotePlayers.find(attackPkt->entityId);
				if (iter == remotePlayers.end())
					break;

				GameObject* targetPlayer = iter->second;

				if (targetPlayer == nullptr) return;

				RemotePlayerScript* remoteScript =
					targetPlayer->GetComponent<RemotePlayerScript>();

				if (remoteScript)
				{
					remoteScript->ApplyAttack(
						attackPkt->weaponType,
						attackPkt->attackIndex,
						math::Vector3(attackPkt->dir_x, attackPkt->dir_y, attackPkt->dir_z)
					);
				}

				break;
			}

			case ePacketType::S_WEAPON_CHANGE:
			{
				Pkt_S_WeaponChange* weaponChangePkt = reinterpret_cast<Pkt_S_WeaponChange*>(packetData.data());

				auto iter = remotePlayers.find(weaponChangePkt->entityId);
				if (iter == remotePlayers.end())
					break;

				GameObject* targetPlayer = iter->second;

				if (targetPlayer == nullptr) return;

				RemotePlayerScript* remoteScript =
					targetPlayer->GetComponent<RemotePlayerScript>();

				if (remoteScript)
				{
					remoteScript->ApplyWeaponChange(
						weaponChangePkt->weaponType
					);
				}

				break;
			}

			case ePacketType::S_LEAVE:
			{
				Pkt_S_Leave* leavePkt = reinterpret_cast<Pkt_S_Leave*>(packetData.data());

				auto iter = remotePlayers.find(leavePkt->entityId);

				if (iter != remotePlayers.end())
				{
					object::Destroy(iter->second);
					remotePlayers.erase(iter);
				}

				break;
			}

			case ePacketType::S_MONSTER_SPAWN:
			{
				const Pkt_S_MonsterSpawn* packet =
					reinterpret_cast<const Pkt_S_MonsterSpawn*>(	packetData.data());

				if (remoteMonsters.find(
					packet->entityId)
					!= remoteMonsters.end())
				{
					break;
				}

				auto enemyDummy =
					std::make_unique<GameObject>();

				enemyDummy->SetLayerType(
					enums::eLayerType::Monster
				);

				std::wstring modelKey;

				switch (packet->modelType)
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
					break;

				activeScene->MakeCharacter(
					enemyDummy.get(),
					modelKey
				);

				RemoteMonsterScript* remoteScript =
					enemyDummy
					->AddComponent<RemoteMonsterScript>();

				WeaponScript* leftGauntlet =
					activeScene->MakeWeapon(
						enemyDummy.get(),
						L"GauntletModel",
						L"LeftHand",
						0.0f
					);

				WeaponScript* rightGauntlet =
					activeScene->MakeWeapon(
						enemyDummy.get(),
						L"GauntletModel",
						L"RightHand",
						0.0f
					);

				if (leftGauntlet)
				{
					leftGauntlet->SetSocketOffsetAntRot(
						math::Vector3(
							129.0f,
							139.0f,
							-9.0f
						),
						math::Vector3::Zero
					);

					remoteScript->SetLeftWeapon(
						leftGauntlet
					);
				}

				if (rightGauntlet)
				{
					rightGauntlet->SetSocketOffsetAntRot(
						math::Vector3(
							-96.0f,
							149.0f,
							1.0f
						),
						math::Vector3::Zero
					);

					remoteScript->SetRightWeapon(
						rightGauntlet
					);
				}

				remoteScript->ApplyMove(
					packet->x,
					packet->y,
					packet->z,
					packet->yaw
				);

				remoteScript->ApplyState(
					packet->state
				);

				activeScene->AddRemoteMonster(
					packet->entityId,
					std::move(enemyDummy)
				);

				break;
			}


			case ePacketType::S_MONSTER_MOVE:
			{
				const Pkt_S_MonsterMove* packet =
					reinterpret_cast<const Pkt_S_MonsterMove*>(packetData.data());

				auto iter =
					remoteMonsters.find(
						packet->entityId
					);

				if (iter == remoteMonsters.end())
					break;

				RemoteMonsterScript* script =iter->second
					->GetComponent<RemoteMonsterScript>();

				if (script)
				{
					script->ApplyMove(
						packet->x,
						packet->y,
						packet->z,
						packet->yaw
					);
				}

				break;
			}


			case ePacketType::S_MONSTER_STATE:
			{
				const Pkt_S_MonsterState* packet =
					reinterpret_cast<const Pkt_S_MonsterState*>(packetData.data());

				auto iter = remoteMonsters.find(packet->entityId);

				if (iter == remoteMonsters.end())
					break;

				RemoteMonsterScript* script =
					iter->second->GetComponent<RemoteMonsterScript>();

				if (script)
				{
					script->ApplyState(packet->state);
				}

				break;
			}

			case ePacketType::S_MONSTER_ATTACK:
			{
				const Pkt_S_MonsterAttack* packet =
					reinterpret_cast<const Pkt_S_MonsterAttack*>(packetData.data());

				auto iter = remoteMonsters.find(packet->entityId);

				if (iter == remoteMonsters.end())
					break;

				RemoteMonsterScript* script =
					iter->second->GetComponent<RemoteMonsterScript>();

				if (script)
				{
					script->ApplyAttack(
						packet->attackIndex,
						math::Vector3(
							packet->dir_x,
							packet->dir_y,
							packet->dir_z
						)
					);
				}

				break;
			}

			case ePacketType::S_MONSTER_DESPAWN:
			{
				const Pkt_S_MonsterDespawn* packet =
					reinterpret_cast<const Pkt_S_MonsterDespawn*>(packetData.data());

				activeScene->EraseRemoteMonster(
					packet->entityId
				);

				break;
			}

			case ePacketType::S_PROJECTILE_SPAWN:
			{
				const Pkt_S_ProjectileSpawn* packet =
					reinterpret_cast<const Pkt_S_ProjectileSpawn*>(packetData.data());

				ProjectileVisualManager::Spawn(
					packet->projectileId,
					packet->ownerEntityId,
					math::Vector3(
						packet->start_x,
						packet->start_y,
						packet->start_z
					),
					math::Vector3(
						packet->velocity_x,
						packet->velocity_y,
						packet->velocity_z
					),
					packet->lifeTime
				);

				break;
			}

			case ePacketType::S_PROJECTILE_END:
			{
				const Pkt_S_ProjectileEnd* packet =
					reinterpret_cast<const Pkt_S_ProjectileEnd*>(packetData.data());

				ProjectileVisualManager::End(
					packet->projectileId,
					math::Vector3(
						packet->end_x,
						packet->end_y,
						packet->end_z
					),
					packet->reason
				);

				break;
			}

			case ePacketType::S_DAMAGE:
			{
				if (packetData.size() !=
					sizeof(Pkt_S_Damage))
				{
					break;
				}

				Pkt_S_Damage packet = {};

				std::memcpy(
					&packet,
					packetData.data(),
					sizeof(packet)
				);

				const bool isDead = packet.isDead != 0;

				const math::Vector3 hitPosition(
					packet.hit_x,
					packet.hit_y,
					packet.hit_z
				);

				if (packet.victimId ==	mMyEntityId)
				{
					GameObject* localPlayer =
						activeScene
						->GetLocalPlayer();

					if (localPlayer)
					{
						PlayerScript* script =
							localPlayer->GetComponent<PlayerScript>();

						if (script)
						{
							script->ApplyServerDamage(
								packet.remainingHp,
								isDead,
								hitPosition
							);
						}
					}

					break;
				}

				auto playerIter =
					remotePlayers.find(
						packet.victimId
					);

				if (playerIter != remotePlayers.end())
				{
					RemotePlayerScript* script =
						playerIter->second->GetComponent<RemotePlayerScript>();

					if (script)
					{
						script->ApplyServerDamage(
							packet.remainingHp,
							isDead,
							hitPosition
						);
					}

					break;
				}

				auto monsterIter =
					remoteMonsters.find(
						packet.victimId
					);

				if (monsterIter !=
					remoteMonsters.end())
				{
					RemoteMonsterScript* script =
						monsterIter->second->GetComponent<RemoteMonsterScript>();

					if (script)
					{
						script->ApplyServerDamage(
							packet.remainingHp,
							isDead,
							hitPosition
						);
					}
				}

				break;
			}

			default:
				break;
			}
		}
		
	}

	void NetworkManager::RecvThread()
	{
		constexpr std::uint16_t MaxPacketSize = 4096;

		std::array<char, 4096> receiveBuffer = {};

		std::vector<char> pendingBuffer;
		pendingBuffer.reserve(8192);

		while (mbIsConnected.load())
		{
			const int receivedBytes =
				recv(
					mClientSocket,
					receiveBuffer.data(),
					static_cast<int>(
						receiveBuffer.size()
						),
					0
				);

			if (receivedBytes <= 0)
			{
				mbIsConnected.store(false);
				break;
			}

			pendingBuffer.insert(
				pendingBuffer.end(),
				receiveBuffer.data(),
				receiveBuffer.data() +
				receivedBytes
			);

			std::size_t consumedBytes = 0;

			while (true)
			{
				const std::size_t remainingBytes =
					pendingBuffer.size() -
					consumedBytes;

				if (remainingBytes <
					sizeof(PacketHeader))
				{
					break;
				}

				PacketHeader header = {};

				std::memcpy(
					&header,
					pendingBuffer.data() +
					consumedBytes,
					sizeof(PacketHeader)
				);

				if (header.size <
					sizeof(PacketHeader) ||
					header.size >
					MaxPacketSize)
				{
					std::cout
						<< "[Network] 잘못된 패킷 크기: "
						<< header.size
						<< '\n';

					mbIsConnected.store(false);
					break;
				}

				if (remainingBytes <
					header.size)
				{
					break;
				}

				std::vector<char> packetData(
					pendingBuffer.begin() +
					consumedBytes,

					pendingBuffer.begin() +
					consumedBytes +
					header.size
				);

				{
					std::lock_guard<std::mutex>
						lock(mPacketMutex);

					mPacketQueue.push(
						std::move(packetData)
					);
				}

				consumedBytes += header.size;
			}

			if (consumedBytes > 0)
			{
				pendingBuffer.erase(
					pendingBuffer.begin(),
					pendingBuffer.begin() +
					consumedBytes
				);
			}
		}
	}

	bool NetworkManager::SendAll(
		const char* data,
		int size)
	{
		int totalSent = 0;

		while (totalSent < size)
		{
			const int sentBytes =
				send(
					mClientSocket,
					data + totalSent,
					size - totalSent,
					0
				);

			if (sentBytes ==
				SOCKET_ERROR ||
				sentBytes <= 0)
			{
				return false;
			}

			totalSent +=
				sentBytes;
		}

		return true;
	}

	void NetworkManager::Release()
	{
		mbIsConnected.store(false);

		{
			std::lock_guard<std::mutex> lock(mSendMutex);

			if (mClientSocket !=
				INVALID_SOCKET)
			{
				shutdown(
					mClientSocket,
					SD_BOTH
				);
			}
		}

		if (mRecvThread.joinable())
		{
			mRecvThread.join();
		}

		{
			std::lock_guard<std::mutex> lock(mSendMutex);

			if (mClientSocket !=
				INVALID_SOCKET)
			{
				closesocket(mClientSocket);

				mClientSocket = INVALID_SOCKET;
			}
		}

		WSACleanup();
	}
}