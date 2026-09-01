#include "MESwordScript.h"
#include "../MyEngine_Source/MEAnimator3D.h"
#include "../MyEngine_Source/MEInput.h"
#include "../MyEngine_Source/MEBoxCollider3D.h"
#include "../MyEngine_Source/METime.h"

namespace ME
{
	SwordScript::SwordScript()

	{
	}
	SwordScript::~SwordScript()
	{

	}
	void SwordScript::Initialize()
	{
		WeaponScript::Initialize();

		mOffsetPos = Vector3(70.0f,140.0f, -10.0f); //x +로 갈 시 아래쪽으로 감 , y축 +시 왼쪽으로 이동, z축은 앞쪽같음
		Vector3 offsetRotDegree = Vector3(90.0f, 180.0f, 15.0f);

		float pitch = XMConvertToRadians(offsetRotDegree.x);
		float yaw = XMConvertToRadians(offsetRotDegree.y);
		float roll = XMConvertToRadians(offsetRotDegree.z);

		 mOffsetQuat = Quaternion::CreateFromYawPitchRoll(yaw, pitch, roll);

		 mWeaponType = eWeaponType::Sword;
		 mIdleAnimName = L"SWORDIDLE1";
		 mWalkAnimName = L"SWORDWALK";

	
	}

	void SwordScript::OnRegister(ActorScript* ownerActor)
	{
		if (ownerActor == nullptr)
			return;

		mActorScript = ownerActor;
		Animator3D* animator = ownerActor->GetAnimator();

		if (animator == nullptr)
		{
			ownerActor->SetAnimator();
			animator = ownerActor->GetAnimator();
		}

		mComboAnimNames = { L"SWORDATTACK1", L"SWORDATTACK2", L"SWORDATTACK3" };

		for (int i = 0; i < 3; i++)
		{

			animator->AddEvent(mComboAnimNames[i], L"HitBox On", 0.3f, [this]() { this->BeginAttack(); });
			animator->AddEvent(mComboAnimNames[i], L"Combo Open", 0.7f, [this]() { this->OpenComboWindow(); });
			animator->AddEvent(mComboAnimNames[i], L"HitBox Off", 0.6f, [this]() { this->EndAttack(); });

			animator->AddEnterBehaviour(mComboAnimNames[i], [this]() { this->LockInput(); });
			animator->AddExitBehaviour(mComboAnimNames[i], [this]() { this->ResetAttackFlags(); });
		}

		WeaponScript::OnRegister(ownerActor);
	}

	void SwordScript::LockInput()
	{
		mbCanComboInput = false;
	}

	void SwordScript::ResetAttackFlags()
	{
		EndAttack();             // 켜져있을지 모르는 히트박스 무조건 끄기
		mbCanComboInput = true;  // 막아둔 입력 창 무조건 열기


	}

	void SwordScript::Update()
	{

		if (mWeaponTransform == nullptr)
		{
			mWeaponTransform = GetOwner()->GetComponent<Transform>();
		}

		if (mbIsComboActive)
		{
			mComboTimer += Time::DeltaTime();

			if(mComboTimer > mComboMaxTime)
			{
				mbIsComboActive = false;
				mComboTimer = 0.0f;
				mComboCount = 0;
			}
		}
	}
	void SwordScript::LateUpdate()
	{
		
		WeaponScript::UpdateWeaponTransform();
	}
	void SwordScript::Render()
	{
	}

	bool SwordScript::Use(WeaponAttackInfo& outAttackInfo)
	{
		// 칼을 휘두를 때 작동할 로직을 여기에 작성합니다.
		// (예: 칼의 콜라이더 판정 켜기, 검기 이펙트 생성, 효과음 재생 등)


		if (mbCanComboInput == false)
			return false;

		if (mActorScript == nullptr)
			return false;

		Animator3D* animator = mActorScript->GetAnimator();

		if (animator == nullptr)
			return false;

		if (mComboAnimNames.empty())
			return false;

		if (mbIsComboActive == false)
		{
			// 첫 공격 시작
			mbIsComboActive = true;
			mComboCount = 0;
		}
		else
		{
			// 콤보 이어나가기
			mComboCount++;

			if (mComboCount >= mComboAnimNames.size())
			{
				mComboCount = 0; // 3타를 다 쳤으면 다시 1타로 돌아감
			}
		}
		mComboTimer = 0.0f;
		const std::size_t selectedAttackIndex = mComboCount;

		animator->PlayAnimation(mComboAnimNames[mComboCount], false);


		outAttackInfo.attackIndex = static_cast<std::uint8_t>(selectedAttackIndex);

		return true;

	}
}