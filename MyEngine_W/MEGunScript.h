#pragma once
#include "../MyEngine_Source/MEScript.h"
#include "MEMath.h"
#include "MECamera.h"

#include "MEWeaponScript.h"
#include "MEObjectPool.h"

namespace ME
{


	class GunScript :public WeaponScript
	{
	public:


		GunScript();
		~GunScript();

		void Initialize() override;
		void Update()override;
		void LateUpdate()override;
		void Render()override;

		bool Use(WeaponAttackInfo& outAttackInfo) override;
		bool Fire();

		bool BuildShotInfo(WeaponAttackInfo& outAttackInfo);
	private:



		GameObject* makeBullet();
		void shootBullet();
		void adjustGunPos();

	private:


	
		Transform* mGunTransform = nullptr;
	

		 float mCoolDownTime;
		 float mCoolDownTimer;
		 bool mbCanShoot;

		 std::unique_ptr<ObjectPool<GameObject>> mBulletPool;

	};

}
