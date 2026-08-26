#include "METransform.h"
#include "MEGraphics.h"
#include "MECamera.h"
#include "MEConstantBuffer.h"
#include "MERenderer.h"

namespace ME
{

	ME::Transform::Transform()
		:Component(enums::eComponentType::Transform)
		,mWorldMatrix(Matrix::Identity)
		,mScale(Vector3::One)
		,mPosition(Vector3::Zero)
		,mRotation(Vector3::Zero)
		,mParent(nullptr)
	{
	}

	ME::Transform::~Transform()
	{
	}

	void ME::Transform::Initialize()
	{

	}

	void ME::Transform::Update()
	{
	}

	void ME::Transform::LateUpdate()
	{
		if (!mbSetWorldMatrix)
		{
		
		Matrix scale = Matrix::CreateScale(mScale.x, mScale.y, mScale.z);
		Matrix rotation = Matrix::CreateFromQuaternion(mRotationQuat);

		Matrix translation = Matrix::CreateTranslation(mPosition);

		Matrix localMatrix = scale * rotation * translation;

		// 부모가 있다면, 부모의 월드 행렬을 곱해 진짜 월드 행렬을 만든다.
		if (mParent != nullptr)
		{
			mWorldMatrix = localMatrix * mParent->GetWorldMatrix();
		}
		else
		{
			mWorldMatrix = localMatrix;
		}

		mForward = Vector3::TransformNormal(Vector3::Forward, rotation);
		mForward.Normalize();

		mRight = Vector3::TransformNormal(Vector3::Right, rotation);
		mRight.Normalize();

		mUp = Vector3::TransformNormal(Vector3::Up, rotation);
		mUp.Normalize();

		}

	}

	void ME::Transform::Render( )
	{
	}

	void Transform::Bind()
	{
		graphics::TransformCB cbData = {};
		cbData.world = GetWorldMatrix();
		cbData.view = Camera::GetGpuViewMatrix();
		cbData.projection = Camera::GetGpuProjectionMatrix();

		graphics::ConstantBuffer* cb = renderer::constantBuffers[CBSLOT_TRANSFORM];

		cb->SetData(&cbData);
		cb->Bind(eShaderStage::All);
	}

}