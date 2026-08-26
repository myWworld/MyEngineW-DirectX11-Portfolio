#pragma once

#include "CommonInclude.h"
#include "../MyEngine_Source/Protocol.h"
#include <atomic>
#include <mutex>

namespace ME
{
    class NetworkManager
    {
    public:
        static bool Initialize();
        static void Update();
        static void Release();

        static bool IsConnected()
        {
            return mbIsConnected.load();
        }

        template <typename T>
        static bool SendPacket(T* packet)
        {
            if (packet == nullptr)
                return false;

            std::lock_guard<std::mutex> lock(mSendMutex);

            if (!mbIsConnected.load() ||
                mClientSocket ==
                INVALID_SOCKET)
            {
                return false;
            }

            packet->header.size =
                static_cast<std::uint16_t>(sizeof(T));

            return SendAll(reinterpret_cast<const char*>(packet), static_cast<int>(sizeof(T)));
        }

    private:
        static bool SendAll(const char* data, int size);

        static void RecvThread();

    private:
        static SOCKET mClientSocket;

        static std::thread mRecvThread;

        static std::atomic_bool mbIsConnected;

        static std::queue<std::vector<char>> mPacketQueue;

        static std::mutex mPacketMutex;
        static std::mutex mSendMutex;

        static EntityId mMyEntityId;
        static bool mbIsHost;
    };
}