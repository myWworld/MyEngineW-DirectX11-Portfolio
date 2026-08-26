#include "MEActorScript.h"
#include "MEWeaponScript.h"
#include "../MyEngine_Source/MEAnimator3D.h"
#include "../MyEngine_Source/MEUIManager.h"

namespace ME
{

	ActorScript::ActorScript() {}

	ActorScript::~ActorScript()
	{
		if (mHpBar != nullptr)
		{
			UIManager::RemoveAttachedUI(mHpBar);
			mHpBar = nullptr;
		}
	}

	void ActorScript::Initialize()
	{
		if (mAnimator == nullptr)
		{
			mAnimator = GetOwner()->GetComponent<Animator3D>();
		}

		// HP Bar 생성/소유/렌더링은 UIManager가 담당
		if (mHpBar == nullptr)
		{
			mHpBar = UIManager::CreateAttachedUI(enums::eUIType::HpBar, GetOwner());
		}
	}

	void ActorScript::Render()
	{
		
	}


	void ActorScript::SetAnimator()
	{
		mAnimator = GetOwner()->GetComponent<Animator3D>();

	}

	void ActorScript::AddWeapon(WeaponScript* weapon)
	{
		if (weapon == nullptr) return;

		mInventoryWeapons.push_back(weapon);

		// 만약 처음 얻은 무기라면 바로 장착
		if (mInventoryWeapons.size() == 1)
		{
			EquipWeapon(0);
		}
		else
		{
			// 장착하지 않은 무기는 화면에서 안 보이게 함
			weapon->GetOwner()->SetActive(false);
		}
	}

	void ActorScript::EquipWeapon(int index)
	{
		if (mInventoryWeapons.empty() || index < 0 || index >= mInventoryWeapons.size()) return;

		// 기존 무기 비활성화 (숨기기)
		if (mCurrentWeaponIndex != -1 && mEquippedWeapon != nullptr)
		{
			mEquippedWeapon->GetOwner()->SetActive(false);
		}

		// 새 무기 장착
		mCurrentWeaponIndex = index;
		mEquippedWeapon = mInventoryWeapons[index];
		mEquippedWeapon->GetOwner()->SetActive(true); // 화면에 보이게 켬

		if (!mEquippedWeapon->IsEventsRegistered())
		{
			mEquippedWeapon->OnRegister(this);
		}

		mbHoldingWeapon = true;
	}


	void ActorScript::AttachActiveWeapon(WeaponScript* weapon)
	{
		if (weapon == nullptr) return;

		if (mAnimator == nullptr)
		{
			mAnimator = GetOwner()->GetComponent<Animator3D>();
		}

		mActiveWeapons.push_back(weapon);
		weapon->GetOwner()->SetActive(true);

		if (!weapon->IsEventsRegistered())
		{
			weapon->OnRegister(this);
		}

		mbHoldingWeapon = true;
	}

	void ActorScript::SetHP(float damage)
	{
		mHp -= damage;
		if (mHp <= 0.0f)
		{
			mHp = 0.0f;

			// 죽은 액터의 HP Bar는 매니저에서 제거한다.
			if (mHpBar != nullptr)
			{
				UIManager::RemoveAttachedUI(mHpBar);
				mHpBar = nullptr;
			}

			OnDeath();
		}
	}

	void ActorScript::SetCurrentHP(float hp)
	{
		mHp = (std::clamp)(
				hp,
				0.0f,
				mMaxHP
				);

		if (mHp <= 0.0f &&
			mHpBar != nullptr)
		{
			UIManager::RemoveAttachedUI(
				mHpBar
			);

			mHpBar = nullptr;
		}
	}


}
