#pragma once
#include "MEMath.h"
#include <algorithm>
#include "MEEntity.h"
#include "MEComponent.h"

namespace ME
{	
	using namespace math;

	class Transform: public Component
	{
	public:

		Transform();
		~Transform();
		
		void Initialize() override;
		void Update()  override;
		void LateUpdate()  override;
		void Render()  override;
		
		void Bind();

		Transform* GetParent() { return mParent; }
		 Matrix GetWorldMatrix() { return mWorldMatrix; }

		 math::Vector3 GetPosition() { return mPosition; }
		 math::Vector3 GetRotation() 
		{
		
			// 쿼터니언 값 추출
			float x = mRotationQuat.x;
			float y = mRotationQuat.y;
			float z = mRotationQuat.z;
			float w = mRotationQuat.w;

			// 쿼터니언 -> 오일러(Radian) 변환 (짐벌락 방어를 위해 std::clamp 사용)
			float pitchRad = std::asin((std::max)(-1.0f, (std::min)(1.0f, 2.0f * (w * x - y * z))));
			float yawRad = std::atan2(2.0f * (w * y + x * z), 1.0f - 2.0f * (x * x + y * y));
			float rollRad = std::atan2(2.0f * (w * z + x * y), 1.0f - 2.0f * (x * x + z * z));

			// 라디안을 디그리로 변환해서 반환
			return math::Vector3(
				XMConvertToDegrees(pitchRad),
				XMConvertToDegrees(yawRad),
				XMConvertToDegrees(rollRad)
			);
		}
		 math::Vector3 GetScale() { return mScale; }
		 math::Quaternion GetRotationQuat() { return mRotationQuat; }

		void SetPosition(math::Vector3 position)
		{
			mPosition = position;
			mbSetWorldMatrix = false; // Reset world matrix flag when position changes
		}
		void SetPosition(float x, float y, float z) { SetPosition(math::Vector3(x, y, z)); }
		void SetRotation(math::Vector3 eulerDegrees)
		{
			// 사람이 입력한 각도(Degree)를 라디안으로 바꾸고, 그걸 쿼터니언으로 변환해서 저장
			float pitch = math::Radian(eulerDegrees.x);
			float yaw = math::Radian(eulerDegrees.y);
			float roll = math::Radian(eulerDegrees.z);

			mRotationQuat = Quaternion::CreateFromYawPitchRoll(yaw, pitch, roll);
			mbSetWorldMatrix = false;
		}	
		void SetRotation(float x, float y, float z) {  SetRotation(math::Vector3(x, y, z)); }
		void SetRotation(Quaternion quat)
		{
			mRotationQuat = quat;
			mbSetWorldMatrix = false;
		}
		void SetScale(math::Vector3 scale) { mScale = scale; }
		void SetScale(float x, float y, float z) { SetScale(math::Vector3(x, y, z)); }

		void SetWorldMatrix(Matrix worldMatrix)
		{
			mWorldMatrix = worldMatrix;
			mbSetWorldMatrix = true;
		}

		 math::Vector3 Forward() { return mForward; };
		 math::Vector3 Right() { return mRight; };
		 math::Vector3 Up() { return mUp; };


	private:
		
		bool mbSetWorldMatrix = false;

		Transform* mParent;
		Matrix mWorldMatrix;

		math::Vector3 mPosition;
		math::Vector3 mRotation;
		Quaternion mRotationQuat;
		math::Vector3 mScale;

		math::Vector3 mForward;
		math::Vector3 mRight;
		math::Vector3 mUp;

	};

}
