#pragma once
#include "MEScript.h"
#include "MEGameObject.h"
#include "METransform.h"
#include "MEComponent.h"
#include "MEActorScript.h"
#include <unordered_set>
#include "../MyEngine_Source/Protocol.h"

namespace ME
{
	struct WeaponAttackInfo
	{
		// 검이면 0, 1, 2가 각각
		// SWORDATTACK1, SWORDATTACK2, SWORDATTACK3
		std::uint8_t attackIndex = 0;

		bool hasProjectile = false;

		math::Vector3 projectileOrigin =
			math::Vector3::Zero;

		math::Vector3 projectileDirection =
			math::Vector3::Forward;
	};

	class ActorScript;
	class Bone;
	class WeaponScript : public Script	
	{
	public:

		enum class OwnerType
		{
			Player,
			Enemy,
			End,
		};
		

		WeaponScript(){}
		virtual ~WeaponScript() = default;


		void SetDamage(float damage) { mDamageAmount = damage; }
		float GetDamage() { return mDamageAmount; }
		void SetSocketBoneName(std::wstring boneName) { mSocketBoneName = boneName; }

		void SetSocketOffsetAntRot(math::Vector3 pos, math::Vector3 rot);

		virtual bool Use(WeaponAttackInfo& outAttackInfo) = 0;

		void WeaponOnOff(bool on) { 
			
			
				this->GetOwner()->SetActive(on);
			
		}

		virtual void SetOnwer(GameObject* owner)
		{
			mOwner = owner;

			if (mOwner)
			{
				mOwnerTransform = mOwner->GetComponent<Transform>();
				mWeaponTransform = GetOwner()->GetComponent<Transform>();
				mActorScript = mOwner->GetComponent<ActorScript>();
			}
		}

		virtual void OnRegister(ActorScript* ownerActor) { mbIsRegistered = true; }
		bool IsEventsRegistered()
		{
			return mbIsRegistered;
		}

		virtual Bone* GetSocketBone();


		GameObject* GetOwnerActor() { return mOwner; }

		void SetIsAttackEnd(bool end) { mbIsAttackEnd = end; }
		bool GetIsAttackEnd() { return mbIsAttackEnd; }

		// 애니메이션 이벤트(Start)에서 호출할 함수
		void BeginAttack()
		{
			mbIsActive = true;
			mHitTargets.clear(); 
		}

		// 애니메이션 이벤트(End)에서 호출할 함수
		void EndAttack()
		{
			mbIsActive = false;
		}

		bool CanAttack() { return mbIsActive; } // 데미지 처리에서 체크

		eWeaponType GetWeaponType() const { return mWeaponType; }

	protected:

		virtual void UpdateWeaponTransform();

	private:		
		void AttachToSocket();

	protected :

		GameObject* mOwner;
		Transform* mWeaponTransform;
		Transform* mOwnerTransform;
		ActorScript* mActorScript;
		OwnerType mOwnerType;

		bool mbIsAttackEnd = false;
		bool mbIsActive = false;

		float mDamageAmount = 10.0f;

		eWeaponType mWeaponType = eWeaponType::Gun;

		bool mbIsRegistered = false;

	public :
		math::Quaternion mOffsetQuat;
		math::Vector3 mOffsetPos;

		std::wstring mIdleAnimName;
		std::wstring mWalkAnimName;
		std::wstring mAttackAnimName;

		std::wstring mSocketBoneName;

	private:

		Bone* mSocketBone = nullptr;

		std::unordered_set<GameObject*> mHitTargets;//중복 히트 방지

	
	};

}
