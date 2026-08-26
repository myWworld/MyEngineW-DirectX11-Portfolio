#pragma once
#include "../MyEngine_Source/MEGameObject.h"

namespace ME
{


	class Tile :public GameObject
	{
	public:

		Tile();
		~Tile();

		void Initialize() override;
		void Update()override;
		void LateUpdate()override;
		void Render()override;

		void SetIndexPosition(int x, int y);
		


	private:


	};

}
