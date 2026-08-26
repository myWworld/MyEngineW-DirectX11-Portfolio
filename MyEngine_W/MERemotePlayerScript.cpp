#include "MERemotePlayerScript.h"

#include "MEGameObject.h"
#include "METransform.h"
#include "MEAnimator3D.h"
#include "MEWeaponScript.h"

namespace ME
{
    RemotePlayerScript::RemotePlayerScript()
    {
    }

    RemotePlayerScript::~RemotePlayerScript()
    {
    }

    void RemotePlayerScript::Initialize()
    {
        ActorScript::Initialize();

        CacheComponents();
        CacheBones();
    }

    void RemotePlayerScript::Update()
    {
        // 원격 플레이어는 입력 처리 X
        // 원격 플레이어는 NetworkManager::SendPacket X
        // 원격 플레이어는 카메라 기준 회전 X

        CacheComponents();
        CacheBones();
    }

    void RemotePlayerScript::LateUpdate()
    {
    }

    void RemotePlayerScript::Render()
    {
        ActorScript::Render();
    }

    void RemotePlayerScript::OnCollisionEnter(Collider* other)
    {
        // 원격 플레이어는 로컬 충돌로 데미지 처리하지 않는 걸 추천.
        // 데미지는 S_DAMAGE, S_STATE 같은 서버 패킷으로 반영.
    }

    void RemotePlayerScript::OnCollisionStay(Collider* other)
    {
    }

    void RemotePlayerScript::OnCollisionExit(Collider* other)
    {
    }

    void RemotePlayerScript::OnPrimaryAction()
    {
        // 원격 플레이어는 내 입력으로 공격하지 않음.
    }

    void RemotePlayerScript::OnToggleWeapon()
    {
        // 원격 플레이어는 내 입력으로 무기 전환하지 않음.
        // 무기 전환도 나중에 S_WEAPON_CHANGE 같은 패킷으로 처리.
    }

    void RemotePlayerScript::OnDeath()
    {
        ApplyState(ePlayerState::DEATH);
    }

    void RemotePlayerScript::DamageProcess(DamageInfo damageInfo)
    {
        // 원격 플레이어의 HP/데미지는 로컬 충돌로 계산하지 않는 게 좋음.
        // 나중에 서버가 S_DAMAGE 또는 S_STATE로 알려주는 값만 반영.
    }

    Bone* RemotePlayerScript::GetWeaponSocketBone()
    {
        CacheBones();
        return mLeftHandBone;
    }
    
    Bone* RemotePlayerScript::GetWeaponSocketBoneRight()
    {
        CacheBones();
        return mRightHandBone;
    }

    Vector3 RemotePlayerScript::GetAimDirection()
    {
        // 원격 플레이어는 renderer::mainCamera를 쓰면 안 됨.
        // 일단 기본 전방값 반환.
        // 엔진에 Transform::GetForward()가 있으면 그걸 쓰는 게 더 좋음.
        return Vector3::Forward;
    }

    void RemotePlayerScript::ApplyMove(float x, float y, float z, float yaw)
    {
        CacheComponents();

        if (mTransform == nullptr)
            return;

        mTransform->SetPosition(x, y, z);
        
        Vector3 rot = mTransform->GetRotation();
        rot.y = yaw;
        mTransform->SetRotation(rot);
    }



    void RemotePlayerScript::ApplyAttack(
        eWeaponType weaponType,
        std::uint8_t attackIndex,
        const Vector3& direction)
    {
        // IDLE/WALK는 같은 상태면 계속 PlayAnimation 호출하지 않기.
        // 계속 호출하면 애니메이션이 매 프레임 첫 프레임으로 리셋될 수 있음.

        if (weaponType == eWeaponType::Sword)
        {
            switch (attackIndex)
            {

                case 0:

                    mAnimator->PlayAnimation(L"SWORDATTACK1", false);
                    break;

                case 1:
                    mAnimator->PlayAnimation(L"SWORDATTACK2", false);
                    break;

                case 2:
                    mAnimator->PlayAnimation(L"SWORDATTACK3", false);
                    break;

                default:
                    break;
        
            }
        }
        else if (weaponType == eWeaponType::Gun)
        {
            switch (attackIndex)
            {
            case 0:

                mAnimator->PlayAnimation(L"PISTOLWALK", false);
                break;
            }
        }
    
    }

    void RemotePlayerScript::RegisterWeapon(
        eWeaponType type,
        WeaponScript* weapon)
    {
        if (weapon == nullptr)
            return;

        mWeaponMap[type] = weapon;

        weapon->WeaponOnOff(false);
    }

    void RemotePlayerScript::ApplyWeaponChange(
        eWeaponType weaponType)
    {
        auto iter = mWeaponMap.find(weaponType);

        if (iter == mWeaponMap.end() || iter->second == nullptr)
            return;

        WeaponScript* nextWeapon = iter->second;

        for (auto& [type, weapon] : mWeaponMap)
        {
            if (weapon != nullptr)
            {
                weapon->WeaponOnOff(weapon == nextWeapon);
            }
        }

        mEquippedWeapon = nextWeapon;
        mCurrentWeaponType = weaponType;

        if (!mbPlayingAction)
        {
            ApplyState(mCurrentRemoteState, true);
        }
    }

    void RemotePlayerScript::ApplyState(ePlayerState state, bool forced)
    {
        CacheComponents();

        if (mAnimator == nullptr)
            return;

        if (mbPlayingAction)
            return;

        mCurrentRemoteState = state;

        switch (state)
        {
        case ePlayerState::IDLE:
            if (mEquippedWeapon)
                mAnimator->PlayAnimation(mEquippedWeapon->mIdleAnimName, true);
            else
                mAnimator->PlayAnimation(L"Idle", true);
            break;

        case ePlayerState::WALK:
            if (mEquippedWeapon)
                mAnimator->PlayAnimation(mEquippedWeapon->mWalkAnimName, true);
            else
                mAnimator->PlayAnimation(L"FORWARDWALK", true);
            break;

        case ePlayerState::HIT:
            mAnimator->PlayAnimation(L"SWORDHIT", false);
            break;

        case ePlayerState::DEATH:
            mAnimator->PlayAnimation(L"SWORDADEATH", false);
            break;

        default:
            break;
        }
    }

    void RemotePlayerScript::ApplyServerDamage(
        float remainingHp,
        bool isDead,
        const Vector3& hitPosition)
    {
        SetCurrentHP(remainingHp);

        if (isDead)
        {
            if (mCurrentRemoteState != ePlayerState::DEATH)
            {
                OnDeath();
            }

            return;
        }

        if (mCurrentRemoteState == ePlayerState::DEATH)
            return;

        mCurrentRemoteState = ePlayerState::HIT;

        if (mEquippedWeapon)
        {
            mEquippedWeapon
                ->SetIsAttackEnd(true);
        }

        if (mAnimator &&
            (mAnimator->GetActiveAnimation() ==
                nullptr ||
                mAnimator->GetActiveAnimation()
                ->GetName() != L"SWORDHIT"))
        {
            mAnimator->PlayAnimation(
                L"SWORDHIT",
                false
            );
        }
    }
  

    void RemotePlayerScript::CacheComponents()
    {
        if (mAnimator == nullptr)
        {
            mAnimator = GetOwner()->GetComponent<Animator3D>();
        }

        if (mTransform == nullptr)
        {
            mTransform = GetOwner()->GetComponent<Transform>();
        }
    }

    void RemotePlayerScript::CacheBones()
    {
        CacheComponents();

       if (mAnimator == nullptr)
           return;
      
       if (mLeftHandBone != nullptr && mRightHandBone != nullptr)
           return;
      
       Skeleton* skeleton = mAnimator->GetSkeletonPtr();
      
       if (skeleton == nullptr)
           return;
      
       mLeftHandBone = skeleton->GetLeftHandTransform();
       mRightHandBone = skeleton->GetRightHandTransform();
    }
}