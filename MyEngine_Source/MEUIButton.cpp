#include "MEUIButton.h"
#include "MEInput.h"

#include "MESceneManager.h"
#include "MEAnimator.h"
#include "MEResources.h"

#include "MESpriteRenderer.h"

#include "../MyEngine_W/METitleScene.h"

namespace ME
{
	UIButton::UIButton()
		:UIBase(enums::eUIType::Button)
	{
	}
	UIButton::~UIButton()
	{
	}
	void UIButton::OnInit()
	{
		SetPos(Vector2(255, 120));
		SetSize(Vector2(70,20));

		mTexture = Resources::Find<graphics::Texture>(L"STARTBUTTON").get();

		mOnClick = std::bind(&UIButton::ButtonClick, this);

		

	}
	void UIButton::OnActive()
	{
	}
	void UIButton::OnInActive()
	{
	}
	void UIButton::OnUpdate()
	{
		Vector2 mousePos = Input::GetMousePos();




		if (mousePos.x >= mPosition.x && mousePos.x <= mPosition.x + mSize.x
			&& mousePos.y >= mPosition.y && mousePos.y <= mPosition.y + mSize.y)
		{
		
			mbMouseOn = true;
		}
		else
		{
			mbMouseOn = false;
		}

		if (Input::GetKeyDown(eKeyCode::LeftMous))
		{
			if(mbMouseOn)
				mOnClick();
		}
	}
	void UIButton::OnLateUpdate()
	{
	}
	void UIButton::OnRender()
	{
		
		
	}
	void UIButton::OnClear()
	{
	}
	void UIButton::ButtonClick()
	{
		if (SceneManager::GetActiveScene()->GetName() == L"TitleScene")
		{
		
		}
		
	}
}