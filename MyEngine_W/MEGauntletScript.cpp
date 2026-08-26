#include "MEGauntletScript.h"
#include "../MyEngine_Source/MEAnimator3D.h"
#include "../MyEngine_Source/MEInput.h"
#include "../MyEngine_Source/MEBoxCollider3D.h"
#include "MEActorScript.h"

namespace ME
{
	GauntletScript::GauntletScript()

	{
	}
	GauntletScript::~GauntletScript()
	{

	}
	void GauntletScript::Initialize()
	{
		WeaponScript::Initialize();
		mWeaponType = eWeaponType::Gauntlet;
	}

	void GauntletScript::OnRegister(ActorScript* ownerActor)
	{
		if (ownerActor == nullptr)
		{
			return;
		}
		mActorScript = ownerActor;
		Animator3D* animator = ownerActor->GetAnimator();

		if (animator == nullptr)
		{
			ownerActor->SetAnimator();
			animator = ownerActor->GetAnimator();
		}


		animator->AddEvent(L"MONSTER_ATTACK", L"HitBox On", 0.1f, [this]() {
			this->BeginAttack();
			});
		animator->AddEvent(L"MONSTER_ATTACK", L"HitBox Off", 0.6f, [this]() {
			this->EndAttack();
			});

		animator->AddEvent(L"MONSTER_ATTACK2", L"HitBox On", 0.25f, [this]() {
			this->BeginAttack();
			});
		animator->AddEvent(L"MONSTER_ATTACK2", L"HitBox Off", 0.7f, [this]() {
			this->EndAttack();
			});

		animator->AddEvent(L"MONSTER_ATTACK3", L"HitBox On", 0.3f, [this]() {
			this->BeginAttack();
			});
		animator->AddEvent(L"MONSTER_ATTACK3", L"HitBox Off", 0.8f, [this]() {
			this->EndAttack();
			}); 
		
		WeaponScript::OnRegister(ownerActor);
	}

	void GauntletScript::Update()
	{

		if (mWeaponTransform == nullptr)
		{
			mWeaponTransform = GetOwner()->GetComponent<Transform>();
		}


	}
	void GauntletScript::LateUpdate()
	{

		GauntletScript::UpdateWeaponTransform();

	
	}
	void GauntletScript::Render()
	{
	}

	bool GauntletScript::Use(WeaponAttackInfo& outAttackInfo) 
	{
	
		return true;
	}
}