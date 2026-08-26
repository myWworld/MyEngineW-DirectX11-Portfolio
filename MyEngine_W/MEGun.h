#pragma once
#include "MEGameObject.h"



namespace ME
{
	class Gun :public GameObject
	{
	public:

		Gun();
		~Gun();

		void Initialize() override;
		void Update()override;
		void LateUpdate()override;
		void Render()override;






	};
}


