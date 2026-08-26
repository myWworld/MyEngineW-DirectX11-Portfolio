#pragma once
#include <cstdint>


using EntityId = std::uint32_t;
using ProjectileId = std::uint32_t;

#pragma pack(push, 1) //1바이트 단위로 메모리를 압축 (네트워크 낭비 방지 및 직렬화 필수)

//C_ 패킷: 클라이언트가 서버에게 요청/보고하는 패킷
//S_ 패킷 : 서버가 검증 / 확정해서 클라이언트들에게 뿌리는 패킷

enum class ePacketType : std::uint16_t
{
    C_ENTER = 1,
    S_ASSIGN_ID = 2,
    S_ENTER = 3,
    S_LEAVE = 4,

    C_MOVE = 5,
    S_MOVE = 6,

    C_STATE = 7,
    S_STATE = 8,

    C_ATTACK = 9,
    S_ATTACK = 10,

    S_DAMAGE = 11,

    C_WEAPON_CHANGE = 12,
    S_WEAPON_CHANGE = 13,

    S_PROJECTILE_SPAWN = 14,
    S_PROJECTILE_END = 15,

    S_MONSTER_STATE = 20,
    S_MONSTER_SPAWN = 21,
    S_MONSTER_MOVE = 22,
    S_MONSTER_DESPAWN = 23,
    S_MONSTER_ATTACK = 24,
};

enum class ePlayerState : std::uint32_t //공격제외 애니메이션
{
    IDLE = 0,
    WALK = 1,
    HIT = 2,
    DEATH = 3
};

enum class eAttackType : std::uint32_t
{
    ATTACK_1 = 1,
    ATTACK_2 = 2,
    ATTACK_3 = 3,
};

enum class eDamageCause : std::uint8_t
{
    Projectile = 0,
    PlayerMelee = 1,
    MonsterMelee = 2,
};

enum class eMonsterState : std::uint32_t
{
    IDLE = 0,
    WALK = 1,
    ATTACK_1 = 2,
    ATTACK_2 = 3,
    ATTACK_3 = 4,
    HIT = 5,
    DEATH = 6,

    RUN = 7
};

enum class eModelType : std::uint32_t
{
    Character = 1,
    Mutant = 2,
    Alien = 3
};

enum class eWeaponType : std::uint32_t
{
    Sword = 1,
    Gun = 2,
    Gauntlet = 3,
    None = 4,
};

enum class eProjectileEndReason : std::uint8_t
{
    Expired = 0,
    HitWorld = 1,
    HitPlayer = 2,
    HitMonster = 3
};

struct PacketHeader
{
    std::uint16_t size;
    ePacketType type;
};

// 서버가 이 클라이언트에게 id가 무엇인지 알려주는 패킷
struct Pkt_S_AssignId
{
    PacketHeader header;
    EntityId entityId;
};

// 클라이언트 -> 서버: 입장
struct Pkt_C_Enter
{
    PacketHeader header;

    eModelType modelType;
    eWeaponType weaponType;
    ePlayerState state;

    float x;
    float y;
    float z;
    float yaw;
};

// 서버 -> 클라이언트: 몇 번 플레이어가 입장
struct Pkt_S_Enter
{
    PacketHeader header;

    EntityId entityId;

    eModelType modelType;
    eWeaponType weaponType;
    ePlayerState state;

    float x;
    float y;
    float z;
    float yaw;
};

// 클라이언트 -> 서버: 나 이 위치로 움직였음
// entityId 없다 -> 서버는 소켓/스레드로 누가 보냈는지 이미 알고 있기 때문
struct Pkt_C_Move
{
    PacketHeader header;

    float x;
    float y;
    float z;
    float yaw;
};

// 서버 -> 클라이언트: 몇 번 플레이어가 이 위치로 움직였다
struct Pkt_S_Move
{
    PacketHeader header;

    EntityId entityId;

    float x;
    float y;
    float z;
    float yaw;
};

// 클라이언트 -> 서버: 상태 바뀜
struct Pkt_C_State
{
    PacketHeader header;

    ePlayerState state;
};

// 서버 -> 클라이언트: 몇 번 플레이어 상태가 바뀜
struct Pkt_S_State
{
    PacketHeader header;

    EntityId entityId;
    ePlayerState state;
};

// 클라이언트 -> 서버: 무기 바뀜
struct Pkt_C_WeaponChange
{
    PacketHeader header;

    eWeaponType weaponType;
};

// 서버 -> 클라이언트: 몇 번 플레이어가 무기 바꿈
struct Pkt_S_WeaponChange
{
    PacketHeader header;

    EntityId entityId;
    eWeaponType weaponType;
};

// 클라이언트 -> 서버: 공격함 
struct Pkt_C_Attack
{
    PacketHeader header;

    std::uint8_t attackIndex;

    float origin_x;//총구용 방향
    float origin_y;
    float origin_z;

    float dir_x; //넉백 방향
    float dir_y;
    float dir_z;
};

// 서버 -> 클라이언트: 몇 번 플레이어가 몇 번 플레이어를 공격했는지
struct Pkt_S_Attack
{
    PacketHeader header;

    EntityId entityId;

    eWeaponType weaponType;
    std::uint8_t attackIndex;

    float dir_x; //넉백 방향
    float dir_y;
    float dir_z;
};

struct Pkt_S_Damage
{
    PacketHeader header;

    eDamageCause cause =
        eDamageCause::Projectile;

    // 근접 공격이면 0
    ProjectileId projectileId = 0;

    EntityId attackerId = 0;
    EntityId victimId = 0;

    float damage = 0.0f;
    float remainingHp = 0.0f;

    std::uint8_t isDead = 0;

    float hit_x = 0.0f;
    float hit_y = 0.0f;
    float hit_z = 0.0f;
};


// 서버 -> 클라이언트: 몇 번 플레이어가 나감
struct Pkt_S_Leave
{
    PacketHeader header;

    EntityId entityId;
};

// 서버 -> 클라이언트: 몬스터 스폰
struct Pkt_S_MonsterSpawn
{
    PacketHeader header;

    EntityId entityId;

    eModelType modelType;
    eWeaponType weaponType;
    eMonsterState state;

    float x;
    float y;
    float z;
    float yaw;

    float hp;
    float maxHp;
};


struct Pkt_S_MonsterMove
{
    PacketHeader header;

    EntityId entityId;

    float x;
    float y;
    float z;
    float yaw;
};
// 서버 -> 클라이언트: 몬스터 상태
struct Pkt_S_MonsterState
{
    PacketHeader header;

    EntityId entityId;
    eMonsterState state;
};

struct Pkt_S_MonsterAttack
{
    PacketHeader header;

    EntityId entityId;
    EntityId targetEntityId;

    std::uint8_t attackIndex;

    float dir_x;
    float dir_y;
    float dir_z;
};

struct Pkt_S_MonsterDespawn
{
    PacketHeader header;

    EntityId entityId;
};


// 서버 -> 클라이언트: 발사체 스폰 // 클라이언트 요청은 C_ATTACK으로 들어올때 서버가 받아서 무기 종류에 따라 처리
struct Pkt_S_ProjectileSpawn
{
    PacketHeader header;

    ProjectileId projectileId;
    EntityId ownerEntityId;

    float start_x;
    float start_y;
    float start_z;

    float velocity_x;
    float velocity_y;
    float velocity_z;

    float lifeTime;
};


struct Pkt_S_ProjectileEnd
{
    PacketHeader header;

    ProjectileId projectileId;

    eProjectileEndReason reason =
        eProjectileEndReason::Expired;

    // 맞은 대상이 없다면 0
    EntityId hitEntityId = 0;


    float end_x = 0.0f;
    float end_y = 0.0f;
    float end_z = 0.0f;
};

#pragma pack(pop)