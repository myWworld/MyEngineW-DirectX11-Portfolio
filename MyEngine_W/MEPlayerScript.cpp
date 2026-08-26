#include "MEPlayerScript.h"
#include "METransform.h"
#include "MEInput.h"
#include "MEGameObject.h"
#include "METime.h"
#include "MEAnimator3D.h"
#include "MERenderer.h"
#include "MEInputHandler.h"
#include "MEWeaponScript.h"
#include "MESwordScript.h"
#include "../MyEngine_Source/MERigidbody.h"
#include "../MyEngine_Source/MEFSMBrain.h"
#include "../MyEngine_Source/MEFSMState.h"

#include "../MyEngine_Source/MENetworkManager.h"

namespace ME
{
    PlayerScript::PlayerScript()
        :mPrevMousePos(Vector2(0, 0))
        , mMouseSpeed(0.5f)
        , mbIsMoving(false)
        , mPlayerType(PlayerType::Player)
		, mPlayerTransform(nullptr)
		, mInputHandler()
		, mState(State::Idle)
    {
    }
    PlayerScript::~PlayerScript()
    {
    }
    void PlayerScript::Initialize()
    {
        ActorScript::Initialize();
        mHeadBone = mAnimator->GetBone(L"Head");

    }
    void PlayerScript::Update()
    {
        if(mState == State::Death)
        {
            mDeathTimer += Time::DeltaTime();

            if (mDeathTime < mDeathTimer)
            {
                this->GetOwner()->SetActive(false);
                this->mEquippedWeapon->GetOwner()->SetActive(false);
            }
            return;
		}

        if (mAnimator == nullptr)
        {
            mAnimator = GetOwner()->GetComponent< Animator3D>();
        }

        if (mPlayerTransform == nullptr)
        {
            mPlayerTransform = GetOwner()->GetComponent<Transform>();
        }

        if (mbUseHands)
        {
            if (mLeftHandBone == nullptr && mRightHandBone == nullptr)
            {
                Skeleton* skeleton  = mAnimator->GetSkeletonPtr();
				if (skeleton == nullptr) return;

                mLeftHandBone = skeleton->GetLeftHandTransform();
                mRightHandBone = skeleton->GetRightHandTransform();
            }
        }

        if (mState == State::Hit)
        {
            // 피격 애니메이션이 완전히 끝났다면 다시 Idle 상태로 복귀
            if (mAnimator->IsAnimationComplete())
            {
                mState = State::Idle;
            }
            return; 
        }

        ICommand* command = mInputHandler.HandleActionInput();
        if (command)
        {
            command->Execute(GetOwner());
        }

        if (mState == State::Attack)
        {
            Attack(); 
            return;   
        }



        if (mState != State::Attack)
        {
            Vector2 moveInput = mInputHandler.GetMovementAxis();
            Translate(moveInput);
        }

        switch (mState)
        {
        case ME::PlayerScript::State::Idle:
            Idle();
            break;
        case ME::PlayerScript::State::Walk:
            Move();
            break;
        default:
            break;
        }

        
        SendTransformIfChanged();
      
      
    }
    void PlayerScript::LateUpdate()
    {
    }
    void PlayerScript::Render()
    {
        ActorScript::Render();
    }


    void PlayerScript::OnPrimaryAction()
    {
        if (!mbHoldingWeapon ||
            mEquippedWeapon == nullptr)
        {
            return;
        }

        WeaponAttackInfo attackInfo = {};

        mEquippedWeapon->SetIsAttackEnd(false);

        if (!mEquippedWeapon->Use(attackInfo))
            return;

        mState = State::Attack;

        Pkt_C_Attack packet = {};

        packet.header.type = ePacketType::C_ATTACK;

        packet.attackIndex = attackInfo.attackIndex;

        if (attackInfo.hasProjectile)
        {
            packet.origin_x = attackInfo.projectileOrigin.x;
            packet.origin_y = attackInfo.projectileOrigin.y;
            packet.origin_z = attackInfo.projectileOrigin.z;

            packet.dir_x = attackInfo.projectileDirection.x;
            packet.dir_y = attackInfo.projectileDirection.y;
            packet.dir_z = attackInfo.projectileDirection.z;
        }
        else
        {
  
            math::Vector3 direction =  GetAimDirection();

            if (direction.LengthSquared() >
                0.0001f)
            {
                direction.Normalize();
            }

            packet.dir_x = direction.x;
            packet.dir_y = direction.y;
            packet.dir_z = direction.z;
        }

        NetworkManager::SendPacket(&packet);
    }

    void PlayerScript::OnToggleWeapon()
    {
		ActorScript::OnToggleWeapon();

        if (mEquippedWeapon == nullptr) return;

        Pkt_C_WeaponChange packet = {};
        packet.header.type = ePacketType::C_WEAPON_CHANGE;
        packet.weaponType = mEquippedWeapon->GetWeaponType();

        NetworkManager::SendPacket(&packet);
    }

    Bone* PlayerScript::GetWeaponSocketBone()
    {
        return mLeftHandBone;
    }

    Bone* PlayerScript::GetWeaponSocketBoneRight()
    {
        return mRightHandBone;
    }

    void  PlayerScript::Idle()
    {
        if (mEquippedWeapon)
            mAnimator->PlayAnimation(mEquippedWeapon->mIdleAnimName);
        else
            mAnimator->PlayAnimation(L"Idle");


    }
    void PlayerScript::Move()
    {
        if (mState == State::Walk)
        {
            if (mEquippedWeapon)
                mAnimator->PlayAnimation(mEquippedWeapon->mWalkAnimName);
            else
                mAnimator->PlayAnimation(L"FORWARDWALK");
        }
        
        if (mState == State::Run)
        {

        }
    }
    void PlayerScript::Attack()
    {

        if (mAnimator->IsAnimationComplete() || mEquippedWeapon->GetIsAttackEnd())
        {
            mState = State::Idle;
            mEquippedWeapon->SetIsAttackEnd(false);
        }
        else
        {
            return;
        }
    }
    void PlayerScript::Die()
    {

    }

    void PlayerScript::Translate(math::Vector2 moveAxis)
    {
        directionChange();
 
        if (moveAxis.x == 0 && moveAxis.y == 0)
        {
            mState = State::Idle;
            mbIsMoving = false;

            SendStateIfChanged(ePlayerState::IDLE);

            return;
        }

        if (mPlayerTransform == nullptr) return;

        Vector3 forwardDir = renderer::mainCamera->GetForward();
        forwardDir.y = 0.0f; 
        forwardDir.Normalize();

		Vector3 rightDir = renderer::mainCamera->GetRight();
        rightDir.y = 0.0f;
        rightDir.Normalize();

        Vector3 moveDir = (forwardDir * moveAxis.y) + (rightDir * moveAxis.x);

        if (moveDir.LengthSquared() > 0.0f)
        {
            moveDir.Normalize();
        }

        Vector3 pos = mPlayerTransform->GetPosition();
        pos += moveDir * 150.0f * Time::DeltaTime();
        mPlayerTransform->SetPosition(pos);

        mbIsMoving = true;
        mState = State::Walk;
        SendStateIfChanged(ePlayerState::WALK);
        
   
    }



    void PlayerScript::directionChange()
    {
		if (mPlayerTransform == nullptr)
        {
            mPlayerTransform = GetOwner()->GetComponent<Transform>();
        }
        
       

        Vector3 camForward = renderer::mainCamera->GetForward();
        camForward.y = 0.0f;
        camForward.Normalize();

        if (camForward.LengthSquared() > 0.0001f)
        {
            float yawRad = atan2f(camForward.x, camForward.z);
            float yawDeg = XMConvertToDegrees(yawRad);
            yawDeg += 180.0f;

            Vector3 rt = mPlayerTransform->GetRotation();
            rt.y = yawDeg;
            mPlayerTransform->SetRotation(rt);
        }


    
        
    }


    void PlayerScript::SendTransformIfChanged()
    {
        if (mPlayerTransform == nullptr)
            return;

        mTransformSendTimer += Time::DeltaTime();

        // 초당 최대 20회 전송
        if (mTransformSendTimer < 0.05f)
            return;

        Vector3 position = mPlayerTransform->GetPosition();
        Vector3 rotation = mPlayerTransform->GetRotation();

        const float yaw = rotation.y;

        Vector3 positionDelta = position - mLastSentPosition;

        bool positionChanged =
            positionDelta.LengthSquared() > 0.25f;

        float yawDelta = yaw - mLastSentYaw;

        // 0도/360도 경계 처리
        while (yawDelta > 180.0f)
            yawDelta -= 360.0f;

        while (yawDelta < -180.0f)
            yawDelta += 360.0f;

        bool rotationChanged =
            std::abs(yawDelta) > 0.5f;

        if (!mHasSentTransform ||
            positionChanged ||
            rotationChanged)
        {
            Pkt_C_Move packet = {};

            packet.header.type = ePacketType::C_MOVE;

            packet.x = position.x;
            packet.y = position.y;
            packet.z = position.z;

            packet.yaw = yaw;

            NetworkManager::SendPacket(&packet);

            mLastSentPosition = position;
            mLastSentYaw = yaw;
            mHasSentTransform = true;
        }

        mTransformSendTimer = 0.0f;
    }

    void PlayerScript::SendStateIfChanged(ePlayerState newState)
    {
        if (mHasSentNetworkState && mLastSentNetworkState == newState) return;

        Pkt_C_State packet = {};

        packet.header.type = ePacketType::C_STATE;
        packet.state = newState;

        NetworkManager::SendPacket(&packet);

        mLastSentNetworkState = newState;
        mHasSentNetworkState = true;
    }

    Vector3 PlayerScript::GetAimDirection()
    {
        return renderer::mainCamera->GetForward();
    }

    void PlayerScript::ApplyServerDamage(
        float remainingHp,
        bool isDead,
        const Vector3& hitPosition)
    {
        SetCurrentHP(remainingHp);

        if (isDead)
        {
            if (mState != State::Death)
            {
                OnDeath();
            }

            return;
        }

        if (mState == State::Death)
            return;

        mState = State::Hit;

        if (mEquippedWeapon)
        {
            mEquippedWeapon->SetIsAttackEnd(true);
        }

        if (mAnimator &&
            (mAnimator->GetActiveAnimation() == nullptr ||
                mAnimator->GetActiveAnimation()->GetName() != L"SWORDHIT"))
        {
            mAnimator->PlayAnimation(
                L"SWORDHIT",
                false
            );
        }
    }

    void PlayerScript::OnDeath()
    {
        mAnimator->PlayAnimation(L"SWORDADEATH",false);
		mState = State::Death;
    }

    void PlayerScript::OnCollisionEnter(Collider* other)
    {
        if (NetworkManager::IsConnected())
        {
            return;
        }

        if (mState == State::Death)
        {
            return;
        }

        GameObject* owner = other->GetOwner();

        if (owner != nullptr &&
            (owner->GetLayerType() == enums::eLayerType::Weapon))
        {

            GameObject* attacker = owner;
            // DamageInfo 패킷 채우기
            DamageInfo damageInfo;
            WeaponScript* weaponScript = attacker->GetComponent<WeaponScript>();

            if (weaponScript)
            {

                if (weaponScript->CanAttack() == false 
                    || weaponScript->GetOwnerActor() == GetOwner()
                     || mState == State::Hit)
                {
                    return;
                }

                damageInfo.damage = weaponScript->GetDamage();
                damageInfo.attacker = attacker;

                Vector3 attackerPos = attacker->GetComponent<Transform>()->GetPosition();
                Vector3 ownerPos = this->GetOwner()->GetComponent<Transform>()->GetPosition();
                Vector3 dir = (ownerPos - attackerPos);
                dir.Normalize();

                damageInfo.hitPoint = Vector3::Zero; // 피격 위치 raycast로 hit point 확장가능
                damageInfo.knockbackDir = dir; // 공격 반대 방향
                damageInfo.knockbackForce = 10.0f; // 기본값 세팅


                DamageProcess(damageInfo);

                if (mState == State::Death)
                {
                    return;
                }

                mState = State::Hit;

                if (mEquippedWeapon)
                {
                    mEquippedWeapon->SetIsAttackEnd(true);
                }

				if (mAnimator->GetActiveAnimation()->GetName() != L"SWORDHIT")
                    mAnimator->PlayAnimation(L"SWORDHIT", false);
            }
        }
    }

    void PlayerScript::OnCollisionStay(Collider* other)
    {
    }

    void PlayerScript::OnCollisionExit(Collider* other)
    {
    }

    void PlayerScript::DamageProcess(DamageInfo damageInfo)
    {

        this->SetHP(damageInfo.damage);

        if (damageInfo.knockbackDir != Vector3::Zero)
        {
            Rigidbody* rb = GetOwner()->GetComponent<Rigidbody>();

            if (rb)
                rb->AddForce(damageInfo.knockbackDir * damageInfo.knockbackForce, Rigidbody::eForceMode::Impulse);

        }

    }

    
}