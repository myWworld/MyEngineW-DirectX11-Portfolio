#include "MEBulletScript.h"
#include "METime.h"
#include "MEGameObject.h"
#include "METransform.h"
#include "MERigidbody.h"

namespace ME
{
	BulletScript::BulletScript()
		:mDeadTime(8.0f)
		,mDeadTimer(0.0f)
	{
	}
	BulletScript::~BulletScript()
	{
	}
	void BulletScript::Initialize()
	{
	}
	void BulletScript::Update()
	{
		mDeadTimer += Time::DeltaTime();


		if (mDeadTime < mDeadTimer)
		{
			mDeadTimer = 0.0f;
			GetOwner()->SetActive(false);
			if (mPool)
			{
				mPool->Return(this->GetOwner());
			}
			resetBulletData();
		}
	}
	void BulletScript::LateUpdate()
	{
	}
	void BulletScript::Render()
	{
	}

	void BulletScript::resetBulletData()
	{
		mDeadTimer = 0.0f;
		
		auto owner = GetOwner();
		Rigidbody* rb = owner->GetComponent<Rigidbody>();

		rb->ClearForces();
	}
}
