#include <iostream>
#define WIN32_LEAN_AND_MEAN
#include "ServerTypes.h"
#include "ServerWorld.h"
#include "PacketUtility.h"
#include "VideoLog.h"

#include <array>
#include <cstring>
#include <iostream>
#include <vector>

namespace
{
    constexpr std::uint16_t MaxPacketSize = 4096;

    template <typename T>
    bool DecodePacket(const std::vector<char>& packetData, ePacketType expectedType, T& outPacket)
    {
        if (packetData.size() != sizeof(T))
            return false;

        std::memcpy(&outPacket, packetData.data(), sizeof(T));

        return outPacket.header.type == expectedType && outPacket.header.size == sizeof(T);
    }

    bool DispatchClientPacket(EntityId clientId, const std::vector<char>& packetData, ServerWorld& world)
    {
        if (packetData.size() < sizeof(PacketHeader))
        {
            return false;
        }

        PacketHeader header = {};
        std::memcpy(&header, packetData.data(), sizeof(PacketHeader));

        switch (header.type)
        {
        case ePacketType::C_ENTER:
        {
            Pkt_C_Enter packet = {};

            if (!DecodePacket(packetData, ePacketType::C_ENTER, packet))
            {
                return false;
            }

            EnterCommand command = {};
            command.entityId = clientId;
            command.modelType = packet.modelType;
            command.weaponType = packet.weaponType;
            command.position = { packet.x, packet.y, packet.z };
            command.yaw = packet.yaw;

            world.EnqueueCommand(command);

            return true;
        }

        case ePacketType::C_MOVE:
        {
            Pkt_C_Move packet = {};

            if (!DecodePacket(packetData, ePacketType::C_MOVE, packet))
            {
                return false;
            }

            MoveCommand command = {};
            command.entityId = clientId;
            command.position = { packet.x, packet.y, packet.z };
            command.yaw = packet.yaw;

            world.EnqueueCommand(command);

            return true;
        }

        case ePacketType::C_STATE:
        {
            Pkt_C_State packet = {};

            if (!DecodePacket(packetData, ePacketType::C_STATE, packet))
            {
                return false;
            }

            StateCommand command = {};
            command.entityId = clientId;
            command.state = packet.state;

            world.EnqueueCommand(command);

            return true;
        }

        case ePacketType::C_WEAPON_CHANGE:
        {
            Pkt_C_WeaponChange packet = {};

            if (!DecodePacket(packetData, ePacketType::C_WEAPON_CHANGE, packet))
            {
                return false;
            }

            WeaponChangeCommand command = {};
            command.entityId = clientId;
            command.weaponType = packet.weaponType;

            world.EnqueueCommand(command);

            return true;
        }

        case ePacketType::C_ATTACK:
        {
            Pkt_C_Attack packet = {};

            if (!DecodePacket(packetData, ePacketType::C_ATTACK, packet))
            {
                return false;
            }

            AttackCommand command = {};
            command.entityId = clientId;
            command.attackIndex = packet.attackIndex;
            command.origin = { packet.origin_x, packet.origin_y, packet.origin_z };
            command.direction = { packet.dir_x, packet.dir_y, packet.dir_z };

            world.EnqueueCommand(command);

            VideoLog::Print("[COMMAND] Queued" " | Type=Attack" " | Entity=", clientId);

            return true;
        }

        default:
            std::cout << "[Protocol] 알 수 없는 클라이언트 패킷: " << static_cast<int>(header.type) << '\n';
            return false;
        }
    }
}

void HandleClient(SOCKET clientSocket, EntityId clientId, ServerWorld& world)
{
    std::array<char, 4096> receiveBuffer = {};

    std::vector<char> pendingBuffer;
    pendingBuffer.reserve(8192); //메모리는 8192바이트 정도 확보했지만, 벡터 안에 유효한 원소는 아직 0

    bool protocolError = false;

    while (!protocolError)
    {
        const int receivedBytes = recv(clientSocket, receiveBuffer.data(), static_cast<int>(receiveBuffer.size()), 0);

        if (receivedBytes <= 0)
        {
            break;
        }

        pendingBuffer.insert(
            pendingBuffer.end(), //end를 해도 사실상 유효한 원소 없으면 처음부터 시작하는거랑 같다
            receiveBuffer.data(),
            receiveBuffer.data() + receivedBytes);

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

            if (header.size < sizeof(PacketHeader) || header.size > MaxPacketSize)
            {
                std::cout << "[Protocol] 잘못된 패킷 크기: " << header.size << '\n';

                protocolError = true;
                break;
            }

            if (remainingBytes < header.size)
            {
                // 다음 recv까지 대기
                break;
            }

            std::vector<char> packetData(
                pendingBuffer.begin() + consumedBytes,
                pendingBuffer.begin() + consumedBytes + header.size);

            if (!DispatchClientPacket(clientId, packetData, world))
            {
                protocolError = true;
                break;
            }

            consumedBytes += header.size;
        }

        if (consumedBytes > 0)
        {
            pendingBuffer.erase(pendingBuffer.begin(), pendingBuffer.begin() + consumedBytes);
        }
    }

    MarkSessionEntered(clientId, false);
    shutdown(clientSocket, SD_BOTH);
    closesocket(clientSocket);
    RemoveSession(clientId);
    world.EnqueueCommand(LeaveCommand{ clientId });
}

int main()
{
    WSADATA wsaData; //win socket 초기화
    int startUpResult = WSAStartup(MAKEWORD(2, 2), &wsaData); //2.2버젼

    if (startUpResult != 0)
    {
        std::cerr << "WSAStartup 초기화 실패! 에러 코드: " << startUpResult << std::endl;
        return 1; // 실패 시 프로그램 종료
    }

    SOCKET listenSocket = socket(AF_INET, SOCK_STREAM, 0); //ipv4, tcp 프로토콜 사용

    if (listenSocket == INVALID_SOCKET)
    {
        std::cerr << "리스닝 소켓 생성 실패! 에러 코드: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return 1;
    }

    SOCKADDR_IN serverAddr = {}; //서버 세팅 ip주소와 포트 번호
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY); //INADDR_ANY: 내 PC의 어떤 IP로 들어오든 다 받음 , htonl -> host byte order에서 network byte order로 변환
    serverAddr.sin_port = htons(7777);

    if (bind(listenSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr)) == SOCKET_ERROR) //소켓에 주소와 포트 결합 (Bind)
    {
        std::cerr << "Bind 실패! 에러 코드: " << WSAGetLastError() << std::endl;
        closesocket(listenSocket);
        WSACleanup();
        return 1;
    }

    if (listen(listenSocket, SOMAXCONN) == SOCKET_ERROR) //소켓 열어놓고 대기 , SOMAXCONN: OS가 허용하는 최대 접속 대기 큐 크기 적용
    {
        std::cerr << "Listen 실패! 에러 코드: " << WSAGetLastError() << std::endl;
        closesocket(listenSocket);
        WSACleanup();
        return 1;
    }

    VideoLog::Print("[SERVER] Listening" " | Port=7777" " | FixedTick=60Hz");

    ServerWorld world;
    world.SetNetworkCallbacks(SendRawToEntity, BroadcastRawExcept, MarkSessionEntered);
    world.SetAllocateEntityIdCallback([]() -> EntityId { return g_nextEntityId.fetch_add(1); });

    //서버 월드도 항상 백그라운드 쓰레드에서 돌아가며 클라이언트 요청에 대한 처리 및 서버 월드 업데이트(몬스터, 총알, 충돌처리 등...) 담당해야함
    std::thread worldThread(&ServerWorld::Run, &world); //어떤 ServerWorld 객체의 Run인지 객체주소도 보내야함

    while (true) //여러 클라이언트로 부터 accept를 받을 수 있도록 한다
    {
        SOCKADDR_IN clientAddr = {}; //클라이언트 접속 수락 (Accept)
        //  accept는 클라이언트가 들어올 때까지 이 라인에서 코드 실행을 멈추고 대기g한다
        int clientAddrLen = sizeof(clientAddr);

        SOCKET clientSocket = accept(listenSocket, (SOCKADDR*)&clientAddr, &clientAddrLen);

        if (clientSocket == INVALID_SOCKET)
        {
            std::cerr << "Accept 에러 " << std::endl;
            continue;
        }

        //std::cout << "새 클라이언트 접속 완료!IP: " << clientSocket << std::endl;

        EntityId newPlayerId = g_nextEntityId.fetch_add(1); //새로 들어오는 클라이언트 마다 고유 id 부여(서버 역할)
        AddSession(newPlayerId, clientSocket);

        VideoLog::Print("[NET] Connected" " | Entity=", newPlayerId);

        Pkt_S_AssignId assignPacket = {}; // 새 클라이언트에게 자기 ID 알려주기
        assignPacket.header.type = ePacketType::S_ASSIGN_ID;
        assignPacket.header.size = sizeof(Pkt_S_AssignId);
        assignPacket.entityId = newPlayerId;

        SendRawToEntity(newPlayerId, &assignPacket, assignPacket.header.size);

        std::thread clientThread(HandleClient, clientSocket, newPlayerId, std::ref(world)); //접속이 확인되면, 새로운 스레드를 파서 패킷 수신

        // detach(): 메인 스레드와 완전히 분리하여 스스로 독립 구동되게 만듦
        // -> 메인 스레드가 바로 위로 올라가서 다음 accept()를 호출할 수 있습니다.
        clientThread.detach();
    }

    world.Stop(); //서버 닫힐 시 서버 월드 종료후 백그라운드에서 재생되고 있는 쓰레드 종료시켜야함

    if (worldThread.joinable())
        worldThread.join();

    //리스닝 소켓 닫기 및 정리
    closesocket(listenSocket);
    WSACleanup();

    return 0;
}