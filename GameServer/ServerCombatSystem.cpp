#include "ServerCombatSystem.h"

#include "ServerMath.h"
#include "VideoLog.h"

#include <algorithm>
#include <cstdint>
#include <utility>
#include <vector>

namespace
{
	struct SwordAttackConfig
	{
		float duration = 0.85f;
		float hitNormalizedTime = 0.5f;
		float damage = 15.0f;
		float reach = 180.0f;
		float radius = 20.0f;
		float cooldown = 0.45f;
	};

	bool IsValidAttackIndex(eWeaponType weaponType, std::uint8_t attackIndex)
	{
		switch (weaponType)
		{
		case eWeaponType::Gun:
			return attackIndex == 0;

		case eWeaponType::Sword:
			return attackIndex < 3;

		default:
			return false;
		}
	}

	bool GetSwordAttackConfig(std::uint8_t attackIndex, SwordAttackConfig& outConfig)
	{
		switch (attackIndex)
		{
		case 0:
			outConfig =
			{
				0.85f,
				0.50f,
				15.0f,
				180.0f,
				20.0f,
				0.45f
			};
			return true;

		case 1:
			outConfig =
			{
				0.95f,
				0.50f,
				18.0f,
				190.0f,
				20.0f,
				0.50f
			};
			return true;

		case 2:
			outConfig =
			{
				1.10f,
				0.50f,
				25.0f,
				210.0f,
				20.0f,
				0.65f
			};
			return true;

		default:
			return false;
		}
	}
}

ServerCombatSystem::ServerCombatSystem(
	ServerWorldState& state, ServerWorldReplicator& replicator)
	: mState(state)
	, mReplicator(replicator)
{
}

void ServerCombatSystem::SetMonsterDamageCallback(MonsterDamageCallback callback)
{
	mMonsterDamageCallback = std::move(callback);
}

void ServerCombatSystem::Handle(const AttackCommand& command)
{
	auto playerIter = mState.players.find(command.entityId);

	if (playerIter == mState.players.end())
		return;

	ServerPlayer& player = playerIter->second;

	if (!player.alive)
		return;

	if (player.attackCooldown > 0.0f)
		return;

	if (!IsValidAttackIndex(player.weaponType, command.attackIndex))
		return;

	ServerVec3 direction = command.direction;

	if (player.weaponType == eWeaponType::Sword)
	{
		if (!ServerMath::NormalizeXZ(direction))
			return;
	}
	else
	{
		if (!ServerMath::Normalize(direction))
			return;
	}

	AttackCommand normalizedCommand = command;
	normalizedCommand.direction = direction;

	if (player.weaponType == eWeaponType::Gun)
	{
		player.attackCooldown = 0.2f;
		SpawnProjectile(player, normalizedCommand);
	}
	else if (player.weaponType == eWeaponType::Sword)
	{
		if (!BeginPlayerMeleeAttack(player, normalizedCommand))
			return;
	}
	else
	{
		return;
	}

	VideoLog::Print("[COMMAND] Processed | Type=Attack | Entity=", player.entityId);

	// 다른 클라이언트에서 공격 애니메이션을 재생
	mReplicator.BroadcastPlayerAttack(player, normalizedCommand);
}

void ServerCombatSystem::TickPlayerCombat(float deltaTime)
{
	for (auto& [playerId, player] : mState.players)
	{
		player.attackCooldown =
			(std::max)(0.0f, player.attackCooldown - deltaTime);
	}

	UpdatePlayerMeleeAttacks(deltaTime);
}

void ServerCombatSystem::TickProjectiles(float deltaTime)
{
	std::vector<ProjectileId> removeProjectiles;

	for (auto& [projectileId, projectile] : mState.projectiles)
	{
		const ServerVec3 start = projectile.position;
		const ServerVec3 end =
		{
			projectile.position.x + projectile.velocity.x * deltaTime,
			projectile.position.y + projectile.velocity.y * deltaTime,
			projectile.position.z + projectile.velocity.z * deltaTime
		};

		projectile.previousPosition = start;
		ProjectileHitResult hit = {};

		if (FindClosestProjectileHit(projectile, start, end, hit))
		{
			projectile.position = hit.hitPosition;

			eProjectileEndReason endReason = eProjectileEndReason::HitWorld;

			if (hit.kind == eServerHitKind::Player)
			{
				endReason = eProjectileEndReason::HitPlayer;
			}
			else if (hit.kind == eServerHitKind::Monster)
			{
				endReason = eProjectileEndReason::HitMonster;
			}

			VideoLog::Print("[PROJECTILE] Hit | Id=", projectile.projectileId, " | Target=", hit.entityId, " | HitT=", hit.hitT);

			mReplicator.BroadcastProjectileEnd(projectile, hit, endReason);

			if (hit.kind == eServerHitKind::Player ||
				hit.kind == eServerHitKind::Monster)
			{
				ApplyDamage(
					eDamageCause::Projectile,
					projectile.ownerEntityId,
					hit.entityId,
					projectile.projectileId,
					projectile.damage,
					hit.hitPosition);
			}

			removeProjectiles.push_back(projectileId);
			continue;
		}

		projectile.position = end;
		projectile.remainingLife -= deltaTime;

		if (projectile.remainingLife <= 0.0f)
		{
			ProjectileHitResult expired = {};
			expired.hitPosition = projectile.position;

			mReplicator.BroadcastProjectileEnd(projectile, expired, eProjectileEndReason::Expired);

			removeProjectiles.push_back(projectileId);
		}
	}

	for (ProjectileId projectileId : removeProjectiles)
	{
		mState.projectiles.erase(projectileId);
	}
}

void ServerCombatSystem::UpdatePlayerMeleeAttacks(float deltaTime)
{
	for (auto& [playerId, player] : mState.players)
	{
		ServerMeleeAttack& attack = player.meleeAttack;

		if (!player.alive || !attack.active)
			continue;

		attack.elapsedTime += deltaTime;

		const float normalizedTime =
			attack.duration > 0.0f ? attack.elapsedTime / attack.duration : 1.0f;

		if (!attack.hitProcessed &&
			normalizedTime >= attack.hitNormalizedTime)
		{
			attack.hitProcessed = true;
			ResolvePlayerMeleeAttack(player, attack);
		}

		if (attack.elapsedTime >= attack.duration)
		{
			attack.active = false;
		}
	}
}

bool ServerCombatSystem::BeginPlayerMeleeAttack(ServerPlayer& player, const AttackCommand& command)
{
	SwordAttackConfig config = {};

	if (!GetSwordAttackConfig(command.attackIndex, config))
		return false;

	ServerVec3 direction = command.direction;

	if (!ServerMath::NormalizeXZ(direction))
		return false;

	ServerMeleeAttack attack = {};
	attack.active = true;
	attack.hitProcessed = false;
	attack.attackIndex = command.attackIndex;
	attack.direction = direction;
	attack.elapsedTime = 0.0f;
	attack.duration = config.duration;
	attack.hitNormalizedTime = config.hitNormalizedTime;
	attack.damage = config.damage;
	attack.reach = config.reach;
	attack.radius = config.radius;

	player.meleeAttack = attack;
	player.attackCooldown = config.cooldown;

	return true;
}

void ServerCombatSystem::ResolvePlayerMeleeAttack(const ServerPlayer& attacker, const ServerMeleeAttack& attack)//플레이어 히트박스 체크 단계 후 데미지 적용
{
	// 몸 중심보다 조금 위에서 검 Sweep을 시작
	ServerVec3 start = attacker.position;
	start.y += 80.0f;

	const ServerVec3 end = ServerMath::Add(start, ServerMath::Multiply(attack.direction, attack.reach));

	for (const auto& [monsterId, monster] : mState.monsters)
	{
		if (!monster.alive)
			continue;

		float hitT = 0.0f;

		if (!ServerMath::SegmentIntersectsAabb(
			start,
			end,
			ServerMath::MakeMonsterAabb(monster),
			attack.radius,
			hitT))
		{
			continue;
		}

		ApplyDamage(
			eDamageCause::PlayerMelee,
			attacker.entityId,
			monsterId,
			0,
			attack.damage,
			ServerMath::PointOnSegment(start, end, hitT));
	}

	if (!mState.friendlyFire)
		return;

	for (const auto& [playerId, player] : mState.players)
	{
		if (!player.alive || playerId == attacker.entityId)
			continue;

		float hitT = 0.0f;

		if (!ServerMath::SegmentIntersectsAabb(
			start,
			end,
			ServerMath::MakePlayerAabb(player),
			attack.radius,
			hitT))
		{
			continue;
		}

		ApplyDamage(
			eDamageCause::PlayerMelee,
			attacker.entityId,
			playerId,
			0,
			attack.damage,
			ServerMath::PointOnSegment(start, end, hitT));
	}
}

void ServerCombatSystem::ResolveMonsterMeleeAttack(ServerMonster& monster)//몬스터가 타겟을 때릴 때 히트박스 체크 후 데미지 적용
{
	if (!monster.alive)
		return;

	ServerPlayer* target = FindAlivePlayer(monster.attackTargetId);

	if (target == nullptr)
		return;

	ServerVec3 direction = monster.attackDirection;

	if (!ServerMath::NormalizeXZ(direction))
	{
		direction =
		{
			target->position.x - monster.position.x,
			0.0f,
			target->position.z - monster.position.z
		};

		if (!ServerMath::NormalizeXZ(direction))
			return;
	}

	ServerVec3 start = monster.position;
	start.y += 80.0f;

	constexpr float AttackReach = 150.0f;
	constexpr float AttackRadius = 25.0f;
	constexpr float AttackDamage = 10.0f;

	const ServerVec3 end = ServerMath::Add(start, ServerMath::Multiply(direction, AttackReach));

	float hitT = 0.0f;

	if (!ServerMath::SegmentIntersectsAabb(
		start,
		end,
		ServerMath::MakePlayerAabb(*target),
		AttackRadius,
		hitT))
	{
		return;
	}

	ApplyDamage(
		eDamageCause::MonsterMelee,
		monster.entityId,
		target->entityId,
		0,
		AttackDamage,
		ServerMath::PointOnSegment(start, end, hitT));
}

void ServerCombatSystem::SpawnProjectile(
	const ServerPlayer& player,
	const AttackCommand& command)
{
	constexpr float BulletSpeed = 1000.0f;
	constexpr float BulletLifeTime = 8.0f;

	ServerVec3 spawnPosition = command.origin;

	if (!IsValidProjectileOrigin(player, spawnPosition))
	{
		constexpr float MuzzleHeight = 100.0f;
		constexpr float ForwardOffset = 60.0f;

		spawnPosition =
		{
			player.position.x + command.direction.x * ForwardOffset,
			player.position.y + MuzzleHeight + command.direction.y * ForwardOffset,
			player.position.z + command.direction.z * ForwardOffset
		};
	}

	ServerProjectile projectile = {};
	projectile.projectileId = mState.nextProjectileId++;

	if (mState.nextProjectileId == 0)
	{
		mState.nextProjectileId = 1;
	}

	projectile.ownerEntityId = player.entityId;
	projectile.previousPosition = spawnPosition;
	projectile.position = spawnPosition;
	projectile.velocity =
	{
		command.direction.x * BulletSpeed,
		command.direction.y * BulletSpeed,
		command.direction.z * BulletSpeed
	};
	projectile.radius = 3.0f;
	projectile.damage = 5.0f;
	projectile.remainingLife = BulletLifeTime;

	mState.projectiles.emplace(projectile.projectileId, projectile);

	VideoLog::Print(
		"[PROJECTILE] Spawn | Id=",
		projectile.projectileId,
		" | Owner=",
		projectile.ownerEntityId);

	mReplicator.BroadcastProjectileSpawn(projectile);
}

bool ServerCombatSystem::FindClosestProjectileHit(
	const ServerProjectile& projectile,
	const ServerVec3& start,
	const ServerVec3& end,
	ProjectileHitResult& outHit) const
{
	outHit = {};
	float closestT = 1.0f;

	const auto testTarget =
		[&](const ServerAabb& box, eServerHitKind kind, EntityId entityId)
		{
			float hitT = 0.0f;

			if (!ServerMath::SegmentIntersectsAabb(
				start,
				end,
				box,
				projectile.radius,
				hitT))
			{
				return;
			}

			if (hitT >= closestT)
				return;

			closestT = hitT;
			outHit.hit = true;
			outHit.hitT = hitT;
			outHit.kind = kind;
			outHit.entityId = entityId;
			outHit.hitPosition = ServerMath::PointOnSegment(start, end, hitT);
		};

	for (const ServerStaticCollider& collider : mState.staticWorldColliders)//정적 물체에 닿았는지 먼저 체크
	{
		testTarget(collider.bounds, eServerHitKind::World, 0);
	}

	const bool ownerIsPlayer =
		mState.players.find(projectile.ownerEntityId) != mState.players.end();

	const bool ownerIsMonster =
		mState.monsters.find(projectile.ownerEntityId) != mState.monsters.end();

	if (ownerIsPlayer)
	{
		for (const auto& [monsterId, monster] : mState.monsters)
		{
			if (!monster.alive)
				continue;

			testTarget(ServerMath::MakeMonsterAabb(monster), eServerHitKind::Monster, monsterId);
		}

		if (mState.friendlyFire)
		{
			for (const auto& [playerId, player] : mState.players)
			{
				if (!player.alive ||
					playerId == projectile.ownerEntityId)
				{
					continue;
				}

				testTarget(ServerMath::MakePlayerAabb(player), eServerHitKind::Player, playerId);
			}
		}
	}
	else if (ownerIsMonster)
	{
		for (const auto& [playerId, player] : mState.players)
		{
			if (!player.alive)
				continue;

			testTarget(ServerMath::MakePlayerAabb(player), eServerHitKind::Player, playerId);
		}
	}

	return outHit.hit;
}

bool ServerCombatSystem::IsValidProjectileOrigin(
	const ServerPlayer& player,
	const ServerVec3& origin) const
{
	if (!ServerMath::IsFinite(origin))
		return false;

	const float dx = origin.x - player.position.x;
	const float dy = origin.y - player.position.y;
	const float dz = origin.z - player.position.z;
	const float distanceSquared = dx * dx + dy * dy + dz * dz;

	constexpr float MaxMuzzleDistance = 350.0f;

	return distanceSquared <=
		MaxMuzzleDistance * MaxMuzzleDistance;
}

ServerPlayer* ServerCombatSystem::FindAlivePlayer(EntityId entityId)
{
	auto iter = mState.players.find(entityId);

	if (iter == mState.players.end() || !iter->second.alive)
		return nullptr;

	return &iter->second;
}

void ServerCombatSystem::ApplyDamage(
	eDamageCause cause,
	EntityId attackerId,
	EntityId victimId,
	ProjectileId projectileId,
	float damage,
	const ServerVec3& hitPosition)
{
	if (damage <= 0.0f)
		return;

	float appliedDamage = 0.0f;
	float remainingHp = 0.0f;
	bool isDead = false;
	bool foundVictim = false;

	auto playerIter = mState.players.find(victimId);

	if (playerIter != mState.players.end())
	{
		ServerPlayer& victim = playerIter->second;

		if (!victim.alive)
			return;

		appliedDamage = (std::min)(damage, victim.hp);
		victim.hp = (std::max)(0.0f, victim.hp - damage);
		remainingHp = victim.hp;

		if (victim.hp <= 0.0f)
		{
			victim.alive = false;
			victim.state = ePlayerState::DEATH;
			isDead = true;
		}

		foundVictim = true;
	}
	else
	{
		auto monsterIter = mState.monsters.find(victimId);

		if (monsterIter == mState.monsters.end())
			return;

		ServerMonster& victim = monsterIter->second;

		if (!victim.alive)
			return;

		appliedDamage = (std::min)(damage, victim.hp);
		victim.hp = (std::max)(0.0f, victim.hp - damage);
		remainingHp = victim.hp;

		if (victim.hp <= 0.0f)
		{
			victim.alive = false;
			isDead = true;
		}

		if (mMonsterDamageCallback)
		{
			mMonsterDamageCallback(victimId, isDead);
		}

		foundVictim = true;
	}

	if (!foundVictim)
		return;

	VideoLog::Print(
		"[DAMAGE] Attacker=",
		attackerId,
		" | Victim=",
		victimId,
		" | Applied=",
		appliedDamage,
		" | HP=",
		remainingHp,
		" | Dead=",
		isDead ? "true" : "false");

	if (isDead)
	{
		VideoLog::Print(
			"[DEATH] Victim=",
			victimId,
			" | ConfirmedByServer");
	}

	mReplicator.BroadcastDamage(
		cause,
		attackerId,
		victimId,
		projectileId,
		appliedDamage,
		remainingHp,
		isDead,
		hitPosition);
}
