#pragma once
#include "MEGameObject.h"



namespace ME
{
	class Bullet :public GameObject
	{
	public:

		Bullet();
		~Bullet();

		void Initialize() override;
		void Update()override;
		void LateUpdate()override;
		void Render()override;



	public:

	
	};
}


