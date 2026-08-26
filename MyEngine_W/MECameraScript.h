#pragma once
#include "../MyEngine_Source/MEScript.h"
#include "MEMath.h"

namespace ME
{


	class CameraScript:public Script
	{
	public:
		enum class perspective
		{
			FirstPerson,
			ThirdPerson,
			TopDown,
			FreeCamera
		};
	

		CameraScript();
		~CameraScript();

		void Initialize() override;
		void Update()override;
		void LateUpdate()override;
		void Render()override;


		void SetTarget(GameObject* obj) { mtarget = obj; }

		perspective GetPerspective() const { return mPerspective; }


	private:

		Vector2 mPrevMousePos;
		Vector2 mCurMousePos;
		float mMouseSpeed;

		Vector3 mTargetPos;
		GameObject* mtarget;

		float cameraYaw = 0.0f;
		float cameraPitch = 0.0f;

		perspective mPerspective = perspective::ThirdPerson;
	};

}
