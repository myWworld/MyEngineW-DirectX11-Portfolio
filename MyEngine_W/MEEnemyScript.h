#pragma once
#include "MEActorScript.h"	
#include "MEDamageInfo.h"
#include "MEMath.h"
#include "MECamera.h"
#include "MEBone.h"

namespace ME
{

	class WeaponScript;
	class EnemyScript :public ActorScript
	{
	public:

	

		enum class State
		{
			Idle,
			Patrol,
			Run,
			Attack,

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
			End,

		};

		EnemyScript();
		~EnemyScript();

		void Initialize() override;
		void Update()override;
		void LateUpdate()override;
		void Render()override;


		void OnCollisionEnter(Collider* other) override;
		void OnCollisionStay(Collider* other) override ;
		void OnCollisionExit(Collider* other) override ;

		void OnPrimaryAction() override;
		void OnToggleWeapon() override;

		void OnDeath() override;
		void DamageProcess(DamageInfo damageInfo) override;
			  
		bool IsMoving() { return mbIsMoving; }

		Bone* GetLeftHandBone() { return mLeftHandBone; }
		Bone* GetRightHandBone() { return mRightHandBone; }

		Bone* GetWeaponSocketBone() override
		{
			return 	mLeftHandBone;
		}

		Bone* GetWeaponSocketBoneRight() override
		{
			return 	mRightHandBone;
		}

		Vector3 GetAimDirection() override;

		void SetLeftWeapon(WeaponScript* weapon);
		void SetRightWeapon(WeaponScript* weapon);

	private:

		void Idle();
		void Move();
		void Attack();
		void Die();

		void Translate();

		void directionChange(Vector3 newDir);

		void randomState();

	private:

		Vector2 mPrevMousePos;
		Vector2 mCurMousePos;
		float mMouseSpeed;


		bool mbIsMoving;

		bool mbUseHands = true;

		State mState = State::Idle;
		Direction mDirection = Direction::Forward;
		Direction mTargetDirection = Direction::Forward;

		Bone* mLeftHandBone;
		Bone* mRightHandBone;

		WeaponScript* mLeftHandWeapon;
		WeaponScript* mRightHandWeapon;

		float translateTimer = 0.0f;
		float translateTime = 2.0f;

		bool mbTurn = false;
	};

}
