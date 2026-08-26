#pragma once

#define WIN32_LEAN_AND_MEAN
#include <WinSock2.h>

#include "../MyEngine_Source/Protocol.h"

#include <atomic>
#include <cstdint>
#include <mutex>
#include <unordered_map>

#pragma comment(lib, "Ws2_32.lib")

struct ClientSession
{
    SOCKET socket = INVALID_SOCKET;
    EntityId playerId = 0;
    bool entered = false;
};

extern std::unordered_map<EntityId, ClientSession> g_sessions;
extern std::mutex g_sessionMutex;
extern std::mutex g_sendMutex;
extern std::atomic<EntityId> g_nextEntityId;

bool SendAll(
    SOCKET socket,
    const char* data,
    int size);

void SendRawToEntity(
    EntityId targetId,
    const void* data,
    std::uint16_t size);

void BroadcastRawExcept(
    EntityId exceptId,
    const void* data,
    std::uint16_t size);

void MarkSessionEntered(
    EntityId entityId,
    bool entered);

void AddSession(
    EntityId entityId,
    SOCKET socket);

void RemoveSession(
    EntityId entityId);