#pragma once
#include "MEScenes.h"
#include "MELayer.h"

namespace ME
{

	class DonDestroyOnLoad : public Scene
	{
	public:

		DonDestroyOnLoad();
		~DonDestroyOnLoad();


		void Initialize()override;
		void Update()override;
		void LateUpdate()override;
		void Render() override;



		void OnEnter()override;
		void OnExit()override;

	private:

	};

}


