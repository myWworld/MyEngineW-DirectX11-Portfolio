#pragma once
#include <vector>
#include "../MyEngine_Source/Protocol.h"


namespace ME
{
	class ClientPacketHandler
	{
    public:
        static void Handle_S_AssignId(const std::vector<char>& packetData);
        static void Handle_S_State(const std::vector<char>& packetData);
        static void Handle_S_Enter(const std::vector<char>& packetData);
        static void Handle_S_Move(const std::vector<char>& packetData);
        static void Handle_S_Attack(const std::vector<char>& packetData);
        static void Handle_S_WeaponChange(const std::vector<char>& packetData);
        static void Handle_S_Leave(const std::vector<char>& packetData);

        static void Handle_S_MonsterSpawn(const std::vector<char>& packetData);
        static void Handle_S_MonsterMove(const std::vector<char>& packetData);
        static void Handle_S_MonsterState(const std::vector<char>& packetData);
        static void Handle_S_MonsterAttack(const std::vector<char>& packetData);
        static void Handle_S_MonsterDespawn(const std::vector<char>& packetData);

        static void Handle_S_ProjectileSpawn(const std::vector<char>& packetData);
        static void Handle_S_ProjectileEnd(const std::vector<char>& packetData);

        static void Handle_S_Damage(const std::vector<char>& packetData);
	};

}