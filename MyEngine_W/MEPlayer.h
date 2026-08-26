#pragma once
#include "MEGameObject.h"



namespace ME
{
	class Player:public GameObject
	{
	public:

		Player();
		~Player();

		void Initialize() override;
		void Update()override;
		void LateUpdate()override;
		void Render()override;


	
	public:

		Vector3 RightHand;
		Vector3 LeftHand;

		


	};
}


