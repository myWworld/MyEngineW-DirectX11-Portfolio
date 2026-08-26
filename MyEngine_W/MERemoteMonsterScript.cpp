#include "MERemoteMonsterScript.h"

#include "MEAnimator3D.h"
#include "MEGameObject.h"
#include "METransform.h"
#include "MEWeaponScript.h"

namespace ME
{
    RemoteMonsterScript::RemoteMonsterScript()
    {
    }

    RemoteMonsterScript::~RemoteMonsterScript()
    {
    }

    void RemoteMonsterScript::Initialize()
    {
        ActorScript::Initialize();

        CacheComponents();
        CacheBones();
    }

    void RemoteMonsterScript::Update()
    {
        CacheComponents();
        CacheBones();

        if (!mbPlayingAction ||
            mAnimator == nullptr)
        {
            return;
        }

        if (!mAnimator->IsAnimationComplete())
            return;

        if (mCurrentRemoteState ==
            eMonsterState::DEATH)
        {
            return;
        }

        mbPlayingAction = false;

        PlayPersistentState();
    }

    void RemoteMonsterScript::LateUpdate()
    {
    }

    void RemoteMonsterScript::Render()
    {
        ActorScript::Render();
    }

    void RemoteMonsterScript::OnCollisionEnter(
        Collider* other)
    {
        // 서버 S_DAMAGE 적용 전까지 로컬 데미지 처리하지 않음
    }

    void RemoteMonsterScript::OnCollisionStay(
        Collider* other)
    {
    }

    void RemoteMonsterScript::OnCollisionExit(
        Collider* other)
    {
    }

    void RemoteMonsterScript::OnPrimaryAction()
    {
    }

    void RemoteMonsterScript::OnToggleWeapon()
    {
    }

    void RemoteMonsterScript::OnDeath()
    {
        ApplyState(eMonsterState::DEATH,true);
    }

    void RemoteMonsterScript::DamageProcess(DamageInfo damageInfo)
    {
        // S_DAMAGE 단계에서 구현
    }

    Bone* RemoteMonsterScript::GetWeaponSocketBone()
    {
        CacheBones();
        return mLeftHandBone;
    }

    Bone* RemoteMonsterScript:: GetWeaponSocketBoneRight()
    {
        CacheBones();
        return mRightHandBone;
    }

    Vector3 RemoteMonsterScript::GetAimDirection()
    {
        return mAttackDirection;
    }

    void RemoteMonsterScript::SetLeftWeapon(WeaponScript* weapon)
    {
        mLeftWeapon = weapon;

        if (mLeftWeapon)
        {
            mLeftWeapon->WeaponOnOff(true);
        }
    }

    void RemoteMonsterScript::SetRightWeapon(WeaponScript* weapon)
    {
        mRightWeapon = weapon;

        if (mRightWeapon)
        {
            mRightWeapon->WeaponOnOff(true);
        }
    }

    void RemoteMonsterScript::ApplyMove(
        float x,
        float y,
        float z,
        float yaw)
    {
        CacheComponents();

        if (mTransform == nullptr)
            return;

        mTransform->SetPosition(
            x,
            y,
            z
        );

        Vector3 rotation =
            mTransform->GetRotation();

        rotation.y = yaw;

        mTransform->SetRotation(
            rotation
        );
    }

    void RemoteMonsterScript::ApplyState(
        eMonsterState state,
        bool forced)
    {
        CacheComponents();

        if (mAnimator == nullptr)
            return;

        switch (state)
        {
        case eMonsterState::ATTACK_1:
            ApplyAttack(0,mAttackDirection);
            return;

        case eMonsterState::ATTACK_2:
            ApplyAttack(1, mAttackDirection);
            return;

        case eMonsterState::ATTACK_3:
            ApplyAttack(2, mAttackDirection);
            return;

        case eMonsterState::HIT:
            mbPlayingAction = true;

            mAnimator->PlayAnimation( L"MONSTER_HIT", false);
            return;

        case eMonsterState::DEATH:
            mCurrentRemoteState = eMonsterState::DEATH;

            mbPlayingAction = true;

            mAnimator->PlayAnimation( L"MONSTER_DEATH", false);
            return;

        default:
            break;
        }

        mCurrentRemoteState = state;

        if (mbPlayingAction)
            return;

        if (!forced &&
            mbHasPlayedState &&
            mLastPlayedState == state)
        {
            return;
        }

        PlayPersistentState();
    }

    void RemoteMonsterScript::ApplyAttack(
        std::uint8_t attackIndex,
        const Vector3& direction)
    {
        CacheComponents();

        if (mAnimator == nullptr)
            return;

        mAttackDirection = direction;
        mbPlayingAction = true;

        switch (attackIndex)
        {
        case 0:
            mAnimator->PlayAnimation(L"MONSTER_ATTACK", false);
            break;

        case 1:
            mAnimator->PlayAnimation(L"MONSTER_ATTACK2", false);
            break;

        case 2:
            mAnimator->PlayAnimation(L"MONSTER_ATTACK3", false);
            break;

        default:
            mbPlayingAction = false;
            break;
        }
    }

    void RemoteMonsterScript::PlayPersistentState()
    {
        if (mAnimator == nullptr)
            return;

        switch (mCurrentRemoteState)
        {
        case eMonsterState::IDLE:
            mAnimator->PlayAnimation(
                L"MONSTER_IDLE",
                true
            );
            break;

        case eMonsterState::WALK:
            mAnimator->PlayAnimation(
                L"MONSTER_WALK",
                true
            );
            break;

        case eMonsterState::RUN:
            mAnimator->PlayAnimation(
                L"MONSTER_RUN",
                true
            );
            break;

        default:
            return;
        }

        mLastPlayedState =
            mCurrentRemoteState;

        mbHasPlayedState = true;
    }

    void RemoteMonsterScript::ApplyServerDamage(
            float remainingHp,
            bool isDead,
            const Vector3& hitPosition)
    {
        SetCurrentHP(remainingHp);

        if (isDead)
        {
            ApplyState(
                eMonsterState::DEATH,
                true
            );

            return;
        }

        ApplyState(
            eMonsterState::HIT,
            true
        );
    }

    void RemoteMonsterScript::CacheComponents()
    {
        if (mAnimator == nullptr)
        {
            mAnimator =
                GetOwner()
                ->GetComponent<Animator3D>();
        }

        if (mTransform == nullptr)
        {
            mTransform =
                GetOwner()
                ->GetComponent<Transform>();
        }
    }

    void RemoteMonsterScript::CacheBones()
    {
        CacheComponents();

        if (mAnimator == nullptr)
            return;

        Skeleton* skeleton =
            mAnimator->GetSkeletonPtr();

        if (skeleton == nullptr)
            return;

        if (mLeftHandBone == nullptr)
        {
            mLeftHandBone =
                skeleton->GetLeftHandTransform();
        }

        if (mRightHandBone == nullptr)
        {
            mRightHandBone =
                skeleton->GetRightHandTransform();
        }
    }

}