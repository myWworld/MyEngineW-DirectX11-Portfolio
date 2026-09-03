#include "ServerWorld.h"

#include <chrono>
#include <thread>
#include <utility>

ServerWorld::ServerWorld()
	: mPlayerSystem(mState, mReplicator)
	, mCombatSystem(mState, mReplicator)
	, mMonsterSystem(mState, mReplicator)
{
	// CombatSystem이 Monster FSM runtime의 내부 구조를 알지 않도록
	// 데미지 결과만 콜백으로 전달
	mCombatSystem.SetMonsterDamageCallback(
		[this](EntityId monsterId, bool isDead)
		{
			mMonsterSystem.NotifyDamage(monsterId, isDead);
		});
}

void ServerWorld::SetNetworkCallbacks(
	SendToCallback sendTo,
	BroadcastExceptCallback broadcastExcept,
	MarkEnteredCallback markEntered)
{
	mReplicator.SetCallbacks(
		std::move(sendTo),
		std::move(broadcastExcept),
		std::move(markEntered));
}

void ServerWorld::SetAllocateEntityIdCallback(AllocateEntityIdCallback allocator)
{
	mMonsterSystem.SetAllocateEntityIdCallback(std::move(allocator));
}

void ServerWorld::EnqueueCommand(WorldCommand command)
{
	std::lock_guard<std::mutex> lock(mCommandMutex);
	mCommandQueue.push(std::move(command));
}

void ServerWorld::EnsureWorldInitialization()
{
	bool expected = false;

	if (!mbInitialized.compare_exchange_strong(expected, true))
		return;

	// 첫 플레이어의 EntityId가 배정된 뒤 C_ENTER 처리 중 실행
	SpawnMonster(eModelType::Mutant, eWeaponType::Gauntlet, { 1500.0f, 0.0f, 0.0f }, 0.0f, true);
}

void ServerWorld::Run()//백그라운드 쓰레드에서 계속 실행됨
{
	if (mbRunning.exchange(true))
	{
		return;
	}

	using Clock = std::chrono::steady_clock;

	constexpr float FixedDeltaTime = 1.0f / 60.0f;
	constexpr auto TickDuration = std::chrono::microseconds(16667);

	auto nextTick = Clock::now();

	while (mbRunning.load())
	{
		nextTick += TickDuration;//다음 고정 주기 시간

		ProcessCommands();
		Tick(FixedDeltaTime);

		const auto now = Clock::now();

		if (now < nextTick)//고정 주기 이내일 경우 대기
		{
			std::this_thread::sleep_until(nextTick);
		}
		else
		{
			// 서버 처리가 늦어진 경우 무한 따라잡기를 방지한다.
			nextTick = now;
		}
	}
}

void ServerWorld::Stop()
{
	mbRunning.store(false);
}

void ServerWorld::ProcessCommands()//클라이언트로부터 온 정보 처리
{
	std::queue<WorldCommand> localCommands;

	{
		std::lock_guard<std::mutex> lock(mCommandMutex);

		// 잠금 상태에서 실제 게임 로직을 처리하지 않고
		// 로컬 큐로 한 번에 옮긴다.
		std::swap(localCommands, mCommandQueue);
	}

	while (!localCommands.empty())
	{
		WorldCommand command = std::move(localCommands.front());

		localCommands.pop();

		std::visit(
			[this](const auto& concreteCommand)
			{
				HandleCommand(concreteCommand);
			},
			command);
	}
}

void ServerWorld::Tick(float deltaTime)//충돌, 몬스터, 투사체 처리
{

	mCombatSystem.TickPlayerCombat(deltaTime);

	mMonsterSystem.TickActions(deltaTime, mCombatSystem);
	mMonsterSystem.TickAI(deltaTime);

	mCombatSystem.TickProjectiles(deltaTime);

	mReplicator.FlushMonsterReplication(mState, deltaTime);

	mMonsterSystem.DespawnRequestedMonsters();
}

void ServerWorld::HandleCommand(const EnterCommand& command)
{
	//잘못된/중복 C_ENTER로는 월드를 초기화하지 않는다
	if (!mPlayerSystem.CanEnter(command))
		return;

	EnsureWorldInitialization();
	mPlayerSystem.Handle(command);
}

void ServerWorld::HandleCommand(const LeaveCommand& command)
{
	mPlayerSystem.Handle(command);
}

void ServerWorld::HandleCommand(const MoveCommand& command)
{
	mPlayerSystem.Handle(command);
}

void ServerWorld::HandleCommand(const StateCommand& command)
{
	mPlayerSystem.Handle(command);
}

void ServerWorld::HandleCommand(const WeaponChangeCommand& command)
{
	mPlayerSystem.Handle(command);
}

void ServerWorld::HandleCommand(const AttackCommand& command)//공격은 서버쪽에서 담당
{
	mCombatSystem.Handle(command);
}

EntityId ServerWorld::SpawnMonster(
	eModelType modelType,
	eWeaponType weaponType,
	const ServerVec3& position,
	float yaw,
	bool broadcast)
{
	return mMonsterSystem.SpawnMonster(modelType, weaponType, position, yaw, broadcast);
}
