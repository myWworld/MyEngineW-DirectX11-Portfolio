#include "PacketUtility.h"

std::unordered_map<EntityId, ClientSession> g_sessions;
std::mutex g_sessionMutex;
std::mutex g_sendMutex;
std::atomic<EntityId> g_nextEntityId{ 1 };

bool SendAll(SOCKET socket, const char* data, int size)
{
	int totalSent = 0;//지금 까지 보낸 크기

	while (totalSent < size)
	{
		const int sent = send(socket, data + totalSent, size -  totalSent, 0);

		if (sent == SOCKET_ERROR || sent <= 0)
			return false;

		totalSent += sent;
	}

	return true;
}

void SendRawToEntity(EntityId targetId, const void* data, std::uint16_t size)//지정한 소켓으로 보냄
{
	SOCKET targetSocket = INVALID_SOCKET;

	{
		std::lock_guard<std::mutex> lock(g_sessionMutex);

		auto iter = g_sessions.find(targetId);

		if (iter == g_sessions.end())
			return;

		targetSocket = iter->second.socket;
	}

	if (targetSocket == INVALID_SOCKET)
		return;

	std::lock_guard<std::mutex> sendLock(g_sendMutex);

	SendAll(targetSocket, static_cast<const char*>(data), static_cast<int>(size));
}

void BroadcastRawExcept(EntityId exceptId, const void* data, std::uint16_t size)//0일 경우 해당되는 id없으므로 브로드 캐스트, 지정한 id제외 서버에 있을 경우 다 보냄
{
	std::vector<SOCKET> targetSockets;

	{
		std::lock_guard<std::mutex> lock(g_sessionMutex);

		for (const auto& [entityId, session] : g_sessions)
		{
			if (entityId == exceptId)
				continue;

			if (!session.entered)
				continue;

			if (session.socket == INVALID_SOCKET)
				continue;

			targetSockets.push_back(session.socket);
		}
	}

	{
		std::lock_guard<std::mutex> sendLock(g_sendMutex);

		for (const auto& targetSocket : targetSockets)
		{
			SendAll(targetSocket, static_cast<const char*>(data), static_cast<int>(size));
		}

	}
}

void MarkSessionEntered(EntityId entityId, bool entered)//실제로 서버 월드에 들어갔을때나 나갈떄
{
	std::lock_guard<std::mutex> lock(g_sessionMutex);

	if (g_sessions.find(entityId) == g_sessions.end()) return;

	g_sessions[entityId].entered = entered;
}

void AddSession(EntityId entityId, SOCKET socket)//클라이언트 세션 등록
{
	std::lock_guard<std::mutex> lock(g_sessionMutex);

	ClientSession clientSession = {};
	clientSession.playerId = entityId;
	clientSession.socket = socket;
	clientSession.entered = false;

	g_sessions[entityId] = clientSession;
}

void RemoveSession(EntityId entityId)
{
	std::lock_guard<std::mutex> lock(g_sessionMutex);
	g_sessions.erase(entityId);
}
