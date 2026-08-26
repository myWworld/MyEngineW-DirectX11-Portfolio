#pragma once
#include "MEScript.h"
#include "MEDamageInfo.h"
#include <vector>

namespace ME
{
	class Bone;
	class WeaponScript;
	class Animator3D;
	class UIBase;
	class ActorScript : public Script
	{

	public:

		ActorScript();
		~ActorScript();

		void Initialize() override;
		void Render() override;

		virtual void OnPrimaryAction() = 0;
		virtual Bone* GetWeaponSocketBone() = 0;
		virtual Bone* GetWeaponSocketBoneRight() = 0;
		virtual Vector3 GetAimDirection() = 0;

		virtual void OnDeath() = 0;
		virtual void DamageProcess(DamageInfo info) = 0;

		Bone* GetHeadBone() { return mHeadBone; }

		bool IsUsingWeapon() { return mbHoldingWeapon; }

		virtual void AddWeapon(WeaponScript* weapon);
		virtual void EquipWeapon(int index);
		virtual void OnToggleWeapon()
		{
			if (mInventoryWeapons.size() <= 1) return;

			int nextIdx = (mCurrentWeaponIndex + 1) % mInventoryWeapons.size();
			EquipWeapon(nextIdx);
		}

		virtual void SetWeaponEquipment(WeaponScript* weapon)
		{
			mEquippedWeapon = weapon;
		}

		virtual void AttachActiveWeapon(WeaponScript* weapon);

		float GetHP() { return mHp; }
		void SetHP(float damage);

		void SetMaxHP(float maxHp) { mMaxHP = maxHp; }
		void SetCurrentHP(float hp);;
		float GetMaxHp() { return mMaxHP; }

		Animator3D* GetAnimator() { return mAnimator; }
		void SetAnimator();

	protected:
		bool mbHoldingWeapon = false;

		std::vector<WeaponScript*> mInventoryWeapons; // 가방에 있는 무기들 (플레이어용)
		std::vector<WeaponScript*> mActiveWeapons;
		int mCurrentWeaponIndex = -1;        // 현재 들고 있는 무기의 배열 번호
		WeaponScript* mEquippedWeapon = nullptr;       // 현재 들고 있는 무기에 빠르게 접근하기 위한 캐싱 포인터


		float mHp = 100.0f;
		float mMaxHP = 100.0f;

		// UIManager가 실제 UI 객체를 소유한다. ActorScript는 제거 요청용 포인터만 캐싱
		UIBase* mHpBar = nullptr;

	public:
		Animator3D* mAnimator = nullptr;
		Bone* mHeadBone = nullptr;

	};
}

