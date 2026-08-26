#include "MEEnemyScript.h"
#include "METransform.h"
#include "MEInput.h"
#include "MEGameObject.h"
#include "METime.h"
#include "MEAnimator3D.h"
#include "MERenderer.h"
#include "MEWeaponScript.h"
#include "../MyEngine_Source/MERigidbody.h"
#include "../MyEngine_Source/MEFSMBrain.h"
#include "../MyEngine_Source/MEFSMState.h"
#include "MEBulletScript.h"
#include "../MyEngine_Source/MEUIHUD.h"

namespace ME
{
    EnemyScript::EnemyScript()
        :mPrevMousePos(Vector2(0, 0))
        , mMouseSpeed(0.5f)
        , mbIsMoving(false)
        ,translateTimer(2.5f)
		
    {
    }
    EnemyScript::~EnemyScript()
    {
    }
    void EnemyScript::Initialize()
    {

        ActorScript::Initialize();

        mbHoldingWeapon = true;
        mHeadBone = mAnimator->GetBone(L"Head");
       
    }
    void EnemyScript::Update()
    {
        if (mAnimator == nullptr)
        {
            mAnimator = GetOwner()->GetComponent< Animator3D>();
        }
        

   

    }
    void EnemyScript::LateUpdate()
    {
    }
    void EnemyScript::Render()
    {
        ActorScript::Render();
    }

    void EnemyScript::SetLeftWeapon(WeaponScript* weapon) 
    {
        mLeftHandWeapon = weapon;
        static_cast<ActorScript*>(this)->AttachActiveWeapon(weapon);
    }
    void EnemyScript::SetRightWeapon(WeaponScript* weapon)
    {
        mRightHandWeapon = weapon;
        static_cast<ActorScript*>(this)->AttachActiveWeapon(weapon);
    }

    void EnemyScript::OnPrimaryAction()
    {
        /*if (mbHoldingWeapon)
        {
            mEquippedWeapon->Use();
        }*/
    }

    void EnemyScript::OnToggleWeapon()
    {
   /*     if(!mbHoldingWeapon)
            ActorScript::OnToggleWeapon();*/


    }
  
    void  EnemyScript::Idle()
    {
        mAnimator->PlayAnimation(L"PISTOLIDLE");
    }
    void EnemyScript::Move()
    {
        if (mState == State::Patrol)
        {
            mAnimator->PlayAnimation(L"PISTOLWALK");
        }

        if (mState == State::Run)
        {

        }
    }


    Vector3 EnemyScript::GetAimDirection()
    {
        return GetOwner()->GetComponent<Transform>()->Forward();
    }

    void EnemyScript::OnDeath()
    {
        auto brain = GetOwner()->GetComponent<FSMBrain>();

        if (brain)
        {
            brain->SendFSMEvent("DEATH");
        }
    
    }

    void EnemyScript::OnCollisionEnter(Collider* other)
    {
        GameObject* owner = other->GetOwner();

        if (owner != nullptr &&
            (owner->GetLayerType() == enums::eLayerType::Bullet
                || owner->GetLayerType() == enums::eLayerType::Weapon))
        {
            // 공격자 객체 찾기 (Bullet이라면 주인을 찾음)
            GameObject* attacker = owner;
            if (owner->GetLayerType() == enums::eLayerType::Bullet)
            {
                BulletScript* bulletScript = owner->GetComponent<BulletScript>();
                bulletScript->BackToPool();
                attacker = bulletScript->GetGun(); 
            }


            // DamageInfo 패킷 채우기
            DamageInfo damageInfo;
            WeaponScript* weaponScript = attacker->GetComponent<WeaponScript>();

            if (weaponScript)
            {

                if(weaponScript->CanAttack() == false || weaponScript->GetOwnerActor() == GetOwner())
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

                auto brain = GetOwner()->GetComponent<FSMBrain>();
                if (brain)
                {
                    std::string currentState = brain->GetActiveState()->GetStateName();

                    if (currentState != "DAMAGE" && this->GetHP() > 0)
                    {
                        brain->SendFSMEvent("DAMAGE");
                    }
                }
            }
        }
    }

    void EnemyScript::OnCollisionStay(Collider* other)
    {
    }

    void EnemyScript::OnCollisionExit(Collider* other)
    {
    }

    void EnemyScript::DamageProcess(DamageInfo damageInfo)
    {

		this->SetHP(damageInfo.damage);

		if (damageInfo.knockbackDir != Vector3::Zero)
		{
			Rigidbody* rb = GetOwner()->GetComponent<Rigidbody>();

            if(rb)
	    		rb->AddForce(damageInfo.knockbackDir * damageInfo.knockbackForce, Rigidbody::eForceMode::Impulse);

		}
        
    }


}