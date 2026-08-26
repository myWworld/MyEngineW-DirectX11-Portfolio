#pragma once
#include "MEActorScript.h"
#include "MEMath.h"
#include "MECamera.h"
#include "MEBone.h"
#include "MEInputHandler.h"
#include "../MyEngine_Source/Protocol.h"

namespace ME
{

	class Transform;
	class InputHandler;
	class WeaponScript;

	class PlayerScript :public ActorScript
	{
	public:
		


		enum class PlayerType
		{
			Player,
			Enemy,
			End,
		};

		enum class State
		{
			Idle,
			Walk,
			Run,
			Attack,
			Hit,
			Death,
			
		};

		enum class Direction
		{
			Forward,
			ForwardLeft,
			ForwardRight,
			Left,
			Right,
			Back,
			BackLeft,
			BackRight,
			Up,

		};

		PlayerScript();
		~PlayerScript();

		void Initialize() override;
		void Update()override;
		void LateUpdate()override;
		void Render()override;

		void OnCollisionEnter(Collider* other) override;
		void OnCollisionStay(Collider* other) override;
		void OnCollisionExit(Collider* other) override;
		
		void OnPrimaryAction() override;
		void OnToggleWeapon() override;

		void OnDeath() override;
		void DamageProcess(DamageInfo damageInfo) override;

		bool IsMoving() { return mbIsMoving; }

		Bone* GetLeftHandBone() { return mLeftHandBone; }
		Bone* GetRightHandBone() { return mRightHandBone; }

		Bone* GetWeaponSocketBone() override;
		 Bone* GetWeaponSocketBoneRight() override;
		Vector3 GetAimDirection() override;

		void SetWeaponEquipment(WeaponScript* weapon)
		{
			mEquippedWeapon = weapon;
		}

		void ApplyServerDamage(
			float remainingHp,
			bool isDead,
			const Vector3& hitPosition);

	private:

		void Idle();
		void Move();
		void Attack();
		void Die();

		void Translate(math::Vector2 moveAxis);

		void SendTransformIfChanged();
		void SendStateIfChanged(ePlayerState newState);
	

		void directionChange();

	private:

		Vector2 mPrevMousePos;
		Vector2 mCurMousePos;
		float mMouseSpeed;

		bool mbIsMoving;

		bool mbUseHands = true;

		State mState = State::Idle;
		Direction mDirection = Direction::Forward;
		Direction mTargetDirection = Direction::Forward;

		Transform* mPlayerTransform;
	
		Vector3 mLastSentPosition = Vector3::Zero;
		float mLastSentYaw = 0.0f;

		float mTransformSendTimer = 0.0f;
		bool mHasSentTransform = false;

		ePlayerState mLastSentNetworkState = ePlayerState::IDLE;
		bool mHasSentNetworkState = false;

		Bone* mLeftHandBone;
		Bone* mRightHandBone;

		PlayerType mPlayerType;
		InputHandler mInputHandler;

		float mDeathTimer = 0.0f;
		float mDeathTime = 5.0f;
	};

}
