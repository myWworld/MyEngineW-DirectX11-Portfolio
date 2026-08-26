#pragma once
#include "MEComponent.h"
#include "../MyEngine_W/MECameraScript.h"

namespace ME
{
	using namespace ME::math;

	class FrustumCulling;
	class Camera :public Component
	{
	public:
	
		enum class eProjectionType
		{
			Perspective,
			Orthographic,
		};



		static Matrix GetGpuViewMatrix() { return ViewMatrix; }
		static Matrix GetGpuProjectionMatrix() { return ProjectionMatrix; }
		static void SetGpuViewMatrix(Matrix matrix) { ViewMatrix = matrix; }
		static void SetGpuProjectionMatrix(Matrix matrix) { ProjectionMatrix = matrix; }

		Camera();
		~Camera();


		void Initialize() override;
		void Update()override;
		void LateUpdate()override;
		void Render()override;

		void CreateViewMatrix();
		void CreateProjectionMatrix(eProjectionType type);

		void SetProjectionMatrix(eProjectionType type) { mProjectionType = type; }
		void SetSize(float size) { mSize = size; }

		void SetTarget(GameObject* obj) { mtarget = obj; }
		GameObject* GetTarget() { return mtarget; }

		Vector3 GetForward() { return mForward; }
		Vector3 GetRight() { return mRight; }

		void SetPitch(float pitch) { mCameraPitch = pitch; }
		void SetRoll(float roll) { mCameraRoll = roll; }
		void SetYaw(float yaw) { mCameraYaw = yaw; }

		float GetYaw() { return mCameraYaw; }
		float GetPitch() { return mCameraPitch; }
		float GetRoll() { return mCameraRoll; }

		FrustumCulling* GetFrustumCulling() { return mFrustumCulling.get(); }

	private:

		//std::vector<GameObject*> mGameObjects;

		static Matrix ViewMatrix;
		static Matrix ProjectionMatrix;

		eProjectionType mProjectionType;

		Matrix mViewMatrix;
		Matrix mProjectionMatrix;
		
		float mAspectRatio;
		float mNear;
		float mFar;
		float mSize;

		float mFovY = 50.0f;

		Vector3 mTargetPos;
		GameObject* mtarget;

		float mCameraYaw;
		float mCameraPitch;
		float mCameraRoll;

		Vector3 mForward;
		Vector3 mRight;

		CameraScript* mCamScript;
		std::unique_ptr<FrustumCulling> mFrustumCulling;

	};

}
