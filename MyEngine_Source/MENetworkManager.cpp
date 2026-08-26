#include "MENetworkManager.h"

#include <iostream>

#pragma comment(lib, "ws2_32.lib")

#include "../MyEngine_Source/MESceneManager.h"
#include "../MyEngine_Source/MEScenes.h"
#include "../MyEngine_Source/METransform.h"
#include "../MyEngine_Source/MEObject.h"
#include "../MyEngine_Source/MEAnimator3D.h"
#include "MEClientPacketHandler.h"

namespace ME
{
	SOCKET NetworkManager::mClientSocket = INVALID_SOCKET;
	EntityId NetworkManager::mMyEntityId = 0;
	bool NetworkManager::mbIsHost = false;
	std::thread NetworkManager::mRecvThread = {}; // 스레드 객체
	std::queue<std::vector<char>> NetworkManager::mPacketQueue = {};
	std::mutex NetworkManager::mPacketMutex = {};
	std::unordered_map<ePacketType, std::function<void(const std::vector<char>&)>> NetworkManager::mPacketHandlers = {};
	
	std::atomic_bool NetworkManager::mbIsConnected = false;
	std::mutex NetworkManager::mSendMutex = {};

	void NetworkManager::RegisterHandlers()
	{
		mPacketHandlers[ePacketType::S_ASSIGN_ID] = ClientPacketHandler::Handle_S_AssignId;
		mPacketHandlers[ePacketType::S_STATE] = ClientPacketHandler::Handle_S_State;
		mPacketHandlers[ePacketType::S_ENTER] = ClientPacketHandler::Handle_S_Enter;
		mPacketHandlers[ePacketType::S_MOVE] = ClientPacketHandler::Handle_S_Move;
		mPacketHandlers[ePacketType::S_ATTACK] = ClientPacketHandler::Handle_S_Attack;
		mPacketHandlers[ePacketType::S_WEAPON_CHANGE] = ClientPacketHandler::Handle_S_WeaponChange;
		mPacketHandlers[ePacketType::S_LEAVE] = ClientPacketHandler::Handle_S_Leave;

		mPacketHandlers[ePacketType::S_MONSTER_SPAWN] = ClientPacketHandler::Handle_S_MonsterSpawn;
		mPacketHandlers[ePacketType::S_MONSTER_MOVE] = ClientPacketHandler::Handle_S_MonsterMove;
		mPacketHandlers[ePacketType::S_MONSTER_STATE] = ClientPacketHandler::Handle_S_MonsterState;
		mPacketHandlers[ePacketType::S_MONSTER_ATTACK] = ClientPacketHandler::Handle_S_MonsterAttack;
		mPacketHandlers[ePacketType::S_MONSTER_DESPAWN] = ClientPacketHandler::Handle_S_MonsterDespawn;

		mPacketHandlers[ePacketType::S_PROJECTILE_SPAWN] = ClientPacketHandler::Handle_S_ProjectileSpawn;
		mPacketHandlers[ePacketType::S_PROJECTILE_END] = ClientPacketHandler::Handle_S_ProjectileEnd;

		mPacketHandlers[ePacketType::S_DAMAGE] = ClientPacketHandler::Handle_S_Damage;
	}

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

		RegisterHandlers();
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

			std::swap(localPackets, mPacketQueue);
		}

		while (!localPackets.empty())
		{
			std::vector<char> packetData = std::move(localPackets.front());

			localPackets.pop();

			if (packetData.size() < sizeof(PacketHeader))
			{
				continue;
			}
			

			PacketHeader header = {};
			std::memcpy(&header, packetData.data(), sizeof(PacketHeader));

			auto it = mPacketHandlers.find(header.type);

			if (it != mPacketHandlers.end())
			{
				it->second(packetData); // 맵에 등록된 처리 함수 실행
			}
			else
			{
				std::cout << "[네트워크 경고] 등록되지 않은 패킷 타입 수신: " << (int)header.type << std::endl;
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
				recv(mClientSocket, receiveBuffer.data(), static_cast<int>(receiveBuffer.size()),0);

			if (receivedBytes <= 0)
			{
				mbIsConnected.store(false);
				break;
			}

			pendingBuffer.insert(
				pendingBuffer.end(),
				receiveBuffer.data(),
				receiveBuffer.data() + receivedBytes
			);

			std::size_t consumedBytes = 0;

			while (true)
			{
				const std::size_t remainingBytes = pendingBuffer.size() - consumedBytes;

				if (remainingBytes < sizeof(PacketHeader))
				{
					break;
				}

				PacketHeader header = {};

				std::memcpy(&header, pendingBuffer.data() + consumedBytes, sizeof(PacketHeader));

				if (header.size < sizeof(PacketHeader) 
					|| header.size > MaxPacketSize)
				{
					std::cout << "[Network] 잘못된 패킷 크기: " << header.size << '\n';

					mbIsConnected.store(false);
					break;
				}

				if (remainingBytes < header.size)
				{
					break;
				}

				std::vector<char> packetData(pendingBuffer.begin() + consumedBytes, pendingBuffer.begin() + consumedBytes + header.size);

				{
					std::lock_guard<std::mutex> lock(mPacketMutex);

					mPacketQueue.push(std::move(packetData));
				}

				consumedBytes += header.size;
			}

			if (consumedBytes > 0)
			{
				pendingBuffer.erase(
					pendingBuffer.begin(),
					pendingBuffer.begin() + consumedBytes
				);
			}
		}
	}

	bool NetworkManager::SendAll(const char* data, int size)
	{
		int totalSent = 0;//현재까지 보낸크기

		while (totalSent < size)
		{
			const int sentBytes = send(mClientSocket, data + totalSent, size - totalSent, 0);

			if (sentBytes == SOCKET_ERROR ||
				sentBytes <= 0)
			{
				return false;
			}

			totalSent += sentBytes;
		}

		return true;
	}

	void NetworkManager::Release()
	{
		mbIsConnected.store(false);

		{
			std::lock_guard<std::mutex> lock(mSendMutex);

			if (mClientSocket != INVALID_SOCKET)
			{
				shutdown(mClientSocket, SD_BOTH);
			}
		}

		if (mRecvThread.joinable())
		{
			mRecvThread.join();
		}

		{
			std::lock_guard<std::mutex> lock(mSendMutex);

			if (mClientSocket != INVALID_SOCKET)
			{
				closesocket(mClientSocket);

				mClientSocket = INVALID_SOCKET;
			}
		}

		WSACleanup();
	}
}