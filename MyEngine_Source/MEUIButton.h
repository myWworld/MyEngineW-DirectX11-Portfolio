#pragma once
#include "MEUIBase.h"
#include "METexture.h"

namespace ME

{
	class UIButton :public UIBase
	{
	public:
		UIButton();
		~UIButton();
	

		void OnInit() override;
		void OnActive()override;
		void OnInActive()override;
		void OnUpdate()override;
		void OnLateUpdate()override;
		void OnRender()override;
		void OnClear()override;

		virtual void ButtonClick();

	private:
		graphics::Texture* mTexture;

		Event mOnClick;
		
	};

}
