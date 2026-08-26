#include "MEGunScript.h"
#include "METransform.h"
#include "MEInput.h"
#include "MEGameObject.h"
#include "METime.h"
#include "MEAnimator3D.h"
#include "MERenderer.h"
#include "Bullet.h"
#include "MEObject.h"
#include "MEBulletScript.h"
#include "MEModel.h"
#include "MEModelRenderer.h"
#include "MEResources.h"
#include "MEMaterial.h"
#include "METexture.h"
#include "MERigidbody.h"
#include "MEBoxCollider3D.h"
#include "MEActorScript.h"


namespace ME
{
	GunScript::GunScript()
		:mCoolDownTime(0.2f)
		, mCoolDownTimer(0.0f)
		, mbCanShoot(false)

	{
		mbIsActive = true;
	}
	GunScript::~GunScript()
	{
		
	}
	void GunScript::Initialize()
	{
		WeaponScript::Initialize();
		mOffsetPos = Vector3(110.0f, 140.0f, -12.0f);
		Vector3 offsetRotDegree = Vector3(180.0f, 0.0f, -98.0f);

		float pitch = XMConvertToRadians(offsetRotDegree.x);
		float yaw = XMConvertToRadians(offsetRotDegree.y);
		float roll = XMConvertToRadians(offsetRotDegree.z);

		mOffsetQuat = Quaternion::CreateFromYawPitchRoll(yaw, pitch, roll);

		mWeaponType = eWeaponType::Gun;


		mIdleAnimName = L"PISTOLIDLE";
		mWalkAnimName = L"PISTOLWALK";
		mAttackAnimName = L"PISTOLWALK";

		if (mWeaponTransform == nullptr)
		{
			mWeaponTransform = GetOwner()->GetComponent<Transform>();
		}

		//mBulletPool = std::make_unique<ObjectPool<GameObject>>(50, [this]()->GameObject*
		//{
		//	return this->makeBullet();
		//});
	}
	void GunScript::Update()
	{
	
		if (mWeaponTransform == nullptr)
		{
			mWeaponTransform = GetOwner()->GetComponent<Transform>();
		}



		if (!mbCanShoot) {
			mCoolDownTimer += Time::DeltaTime();
			if (mCoolDownTimer > mCoolDownTime) {
				mbCanShoot = true;
				mCoolDownTimer = 0.0f;
			}
		}

		//if (mOwnerType == OwnerType::Enemy && mbCanShoot) {
		//	Fire();
	
		//	mCoolDownTime = (float)(rand() % 3 + 2);
		//}

	}
	void GunScript::LateUpdate()
	{
		WeaponScript::UpdateWeaponTransform();

	}
	void GunScript::Render()
	{
	}

	bool GunScript::Use(WeaponAttackInfo& outAttackInfo)
	{

		if (!mbCanShoot)
		{
			mbIsAttackEnd = true;
			return false;
		}

		if (!BuildShotInfo(outAttackInfo))
		{
			mbIsAttackEnd = true;
			return false;
		}

		mbCanShoot = false;
		mCoolDownTimer = 0.0f;
		mbIsAttackEnd = true;

		return true;
	}

	bool GunScript::Fire()
	{
	
		return true;
	}

	void GunScript::adjustGunPos()
	{
		WeaponScript::UpdateWeaponTransform();

	
	}

	bool GunScript::BuildShotInfo(WeaponAttackInfo& outAttackInfo) //기존 총알 발사는 클라이언트전용 -> 네트웨크는 방향, 위치등만 계산
	{
		if (mWeaponTransform == nullptr)
		{
			mWeaponTransform =
				GetOwner()->GetComponent<Transform>();
		}

		if (mWeaponTransform == nullptr)
			return false;

		if (mActorScript == nullptr)
			return false;

		math::Vector3 direction = mActorScript->GetAimDirection();

		direction.y = 0.0f;

		if (direction.LengthSquared() < 0.0001f)
		{
			return false;
		}

		direction.Normalize();

		math::Vector3 origin = mWeaponTransform->GetPosition();
		constexpr float MuzzleForwardOffset = 20.0f;

		origin += direction * MuzzleForwardOffset;

		outAttackInfo.attackIndex = 0;
		outAttackInfo.hasProjectile = true;

		outAttackInfo.projectileOrigin = origin;

		outAttackInfo.projectileDirection = direction;

		return true;
	}

	GameObject* GunScript::makeBullet()
	{
		if (mWeaponTransform == nullptr)
			return nullptr;

		Transform* tr = mWeaponTransform;
		Vector3 gunPos = tr->GetPosition();
	

		Vector3 spawnPos = gunPos;
		Bullet* bullet = object::Instantiate<Bullet>(enums::eLayerType::Bullet, spawnPos);
		BulletScript* bulletScript = bullet->AddComponent<BulletScript>();
		bulletScript->SetPool(mBulletPool.get());
		bulletScript->SetGun(GetOwner());

		BoxCollider3D* col = bullet->AddComponent<BoxCollider3D>();
		col->SetSize(Vector3(3, 2, 2));
		
		Rigidbody* rb = bullet->AddComponent<Rigidbody>();
		rb->SetMass(2.0f);

		Transform* bulletTr = bullet->GetComponent<Transform>();
		bulletTr->SetScale(.1f, .1f, .1f);
		bulletTr->SetRotation(tr->GetRotation());


		std::shared_ptr<Model> bullet_model = Resources::Find<Model>(L"BulletModel");

		if (bullet_model !=  nullptr)
		{

			ModelRenderer* modelRenderer = bullet->AddComponent<ModelRenderer>();
			modelRenderer->SetMesh(bullet_model->GetMeshes());

			bullet->SetModel(bullet_model);

		}
		else
		{
			bullet_model = nullptr;
		}

		return static_cast<GameObject*>(bullet);

	}

	void GunScript::shootBullet()
	{
		auto bullet = mBulletPool->Get();

		if (bullet == nullptr) return;
		
		if (mWeaponTransform == nullptr)
			mWeaponTransform = GetOwner()->GetComponent<Transform>();

		
		Vector3 gunPos = mWeaponTransform->GetPosition();

		Transform* bulletTr = bullet->GetComponent<Transform>();


		Rigidbody* rb = bullet->GetComponent<Rigidbody>();

		bulletTr->SetPosition(gunPos);
	
		
		Vector3 trueDir = mActorScript->GetAimDirection();
		trueDir.y = 0.0f;
		trueDir.Normalize();

		float yawRad = atan2f(trueDir.x, trueDir.z);
		bulletTr->SetRotation(Quaternion::CreateFromYawPitchRoll(yawRad, 0.0f, 0.0f));

		rb->ClearForces();
		rb->SetLimitVelocity(Vector3(5000.0f, 1000.0f, 5000.0f));
		rb->AddForce(trueDir * 2000.0f, Rigidbody::eForceMode::Impulse);
	}	
}