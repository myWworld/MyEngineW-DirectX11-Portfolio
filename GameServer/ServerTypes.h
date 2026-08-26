#pragma once

#include "../MyEngine_Source/Protocol.h"

#include <cstdint>
#include <string>
#include <variant>

struct ServerVec3
{
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
};

struct AnimationActionMeta
{
    float duration = 0.0f;
    float hitNormalizedTime = 0.0f;
};

struct ServerAabb
{
    ServerVec3 center;
    ServerVec3 halfExtent;
};

enum class eServerHitKind : std::uint8_t
{
    None = 0,
    Player,
    Monster,
    World,
};

struct ProjectileHitResult
{
    bool hit = false;

    // 선분 시작점 0.0 ~ 끝점 1.0
    float hitT = 1.0f;

    eServerHitKind kind =
        eServerHitKind::None;

    EntityId entityId = 0;

    ServerVec3 hitPosition;
};

struct ServerStaticCollider
{
    std::uint32_t colliderId = 0;
    ServerAabb bounds;
};


struct ServerMeleeAttack //칼같은 무기용 앞에 특정 범위에 대하여
{
    bool active = false;
    bool hitProcessed = false;

    std::uint8_t attackIndex = 0;

    ServerVec3 direction;

    float elapsedTime = 0.0f;
    float duration = 0.0f;

    float hitNormalizedTime = 0.3f;

    float damage = 15.0f;
    float reach = 180.0f;

    // 선분 주위의 공격 두께
    float radius = 45.0f;
};


struct ServerPlayer
{
    EntityId entityId = 0;

    ServerVec3 position;
    float yaw = 0.0f;

    eModelType modelType =
        eModelType::Character;

    eWeaponType weaponType =
        eWeaponType::Gun;

    ePlayerState state =
        ePlayerState::IDLE;

    float hp = 100.0f;
    bool alive = true;

    float colliderRadius = 25.0f;
    float colliderHalfHeight = 125.0f;

    // Transform 원점과 Collider 중심이 같으면 0
     // 모델 원점이 발바닥이면 125 정도로 조정
    float colliderCenterOffsetY = 0.0f;

    float attackCooldown = 0.0f;

    ServerMeleeAttack meleeAttack;
};

struct ServerMonster
{
    EntityId entityId = 0;

    eModelType modelType =
        eModelType::Mutant;

    eWeaponType weaponType =
        eWeaponType::Gauntlet;

    eMonsterState state =
        eMonsterState::IDLE;

    ServerVec3 position;
    float yaw = 0.0f;

    float hp = 100.0f;
    float maxHp = 100.0f;

    bool alive = true;

    EntityId targetPlayerId = 0;

    float colliderRadius = 35.0f;
    float colliderHalfHeight = 125.0f;
    float colliderCenterOffsetY = 0.0f;

    ServerVec3 patrolOrigin;
    ServerVec3 patrolTarget;

    bool hasPatrolTarget = false;

    std::string currentActionAnimation;

    float actionElapsedTime = 0.0f;
    float actionDuration = 0.0f;

    float attackHitNormalizedTime = 0.35f;

    bool attackHitProcessed = true;
    bool actionIsAttack = false;

    bool transformDirty = false;
    bool stateDirty = false;

    float moveReplicationTimer = 0.0f;

    bool attackEventPending = false;

    std::uint8_t attackIndex = 0;

    EntityId attackTargetId = 0;
    ServerVec3 attackDirection;

    bool destroyRequested = false;
};

struct ServerProjectile
{
    ProjectileId projectileId = 0;
    EntityId ownerEntityId = 0;

    ServerVec3 previousPosition;
    ServerVec3 position;
    ServerVec3 velocity;

    float radius = 3.0f;
    float damage = 5.0f;
    float remainingLife = 8.0f;
};

struct EnterCommand
{
    EntityId entityId = 0;

    eModelType modelType =
        eModelType::Character;

    eWeaponType weaponType =
        eWeaponType::Gun;

    ServerVec3 position;
    float yaw = 0.0f;
};

struct LeaveCommand
{
    EntityId entityId = 0;
};

struct MoveCommand
{
    EntityId entityId = 0;

    ServerVec3 position;
    float yaw = 0.0f;
};

struct StateCommand
{
    EntityId entityId = 0;

    ePlayerState state =
        ePlayerState::IDLE;
};

struct WeaponChangeCommand
{
    EntityId entityId = 0;

    eWeaponType weaponType =
        eWeaponType::Gun;
};

struct AttackCommand
{
    EntityId entityId = 0;

    std::uint8_t attackIndex = 0;

    ServerVec3 origin;
    ServerVec3 direction;
};

using WorldCommand = std::variant<
    EnterCommand,
    LeaveCommand,
    MoveCommand,
    StateCommand,
    WeaponChangeCommand,
    AttackCommand
>;