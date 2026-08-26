#include "MEWeaponScript.h"
#include "MEBone.h"
#include "MEActorScript.h"
#include "MEAnimator3D.h"

namespace ME
{
	Bone* WeaponScript::GetSocketBone()
	{
		if (!mActorScript) return nullptr;

		return mActorScript->GetWeaponSocketBone();
	}

	void WeaponScript::SetSocketOffsetAntRot(math::Vector3 pos, math::Vector3 rot)
	{
		mOffsetPos = pos;

		float pitch = XMConvertToRadians(rot.x);
		float yaw = XMConvertToRadians(rot.y);
		float roll = XMConvertToRadians(rot.z);

		mOffsetQuat = Quaternion::CreateFromYawPitchRoll(yaw, pitch, roll);
	}

	void WeaponScript::UpdateWeaponTransform()
	{
		AttachToSocket();
	}

	void WeaponScript::AttachToSocket()
	{
		if (mOwner == nullptr || mWeaponTransform == nullptr) return;
		
		if(mSocketBone == nullptr)
			mSocketBone = mActorScript->GetAnimator()->GetBone(mSocketBoneName);

		if (mSocketBone == nullptr || !mActorScript->IsUsingWeapon()) return;
	
		Matrix handLocal = mSocketBone->FinalTransform;
		Matrix playerWorldMatrix = mOwnerTransform->GetWorldMatrix();
		Matrix handMatrix = handLocal * playerWorldMatrix;

		Matrix offsetMatrix = Matrix::CreateFromQuaternion(mOffsetQuat)
			* Matrix::CreateTranslation(mOffsetPos);

		Matrix finalMatrix = offsetMatrix * handMatrix;

		Vector3 pos, scale;
		Quaternion rot;
		finalMatrix.Decompose(scale, rot, pos);

		mWeaponTransform->SetPosition(pos);
		mWeaponTransform->SetRotation(rot);


	}
}