
#include "MEInput.h"
#include "MEApplication.h"

extern ME::Application application;

namespace ME{

	std::vector <Input::Key> Input::Keys = {};
	math::Vector2 Input::mMousePosition = math::Vector2::One;

	int ASCII[(UINT)eKeyCode::End] =
	{
		'Q','W','E','R','T','Y','U','I','O','P'
		,'A','S','D','F','G','H','J','K','L'
		,'Z','X','C','V','B','N','M',
		VK_LEFT,VK_RIGHT,VK_DOWN,VK_UP
		,VK_SPACE,VK_SHIFT,VK_RETURN
		,VK_LBUTTON,VK_RBUTTON

	};

	void Input::CreateKeys()
	{
		for (size_t i = 0; i < (UINT)eKeyCode::End; i++)
		{
			Key key = {};
			key.bPressed = false;
			key.state = eKeystate::None;
			key.keyCode = (eKeyCode)i;

			Keys.push_back(key);
		}
	}
	void Input::UpdateKeys()
	{
		std::for_each(Keys.begin(), Keys.end(),
			[](Key& key) -> void
			{
				UpdateKey(key);
			}
		);
	}

	void Input::UpdateKey(Key& key)
	{

		if (GetFocus())//게임창이 활성화 돼 있는지
		{
			if (isKeyDown(key.keyCode))
			{
				UpdateKeyDown(key);
			}
			else
			{
				UpdateKeyUp(key);
			}


			getMousePositionByWindow();
		}
		else
		{
			clearKeys();
		}
	}



	bool Input::isKeyDown(eKeyCode code)
	{
		return GetAsyncKeyState(ASCII[(UINT)code]) & 0x8000;//폴링
	}

	void Input::UpdateKeyDown(Key& key)
	{
		if (key.bPressed == true)
		{
			key.state = eKeystate::Pressed;
		}
		else
		{
			key.state = eKeystate::Down;
		}
		key.bPressed = true;
	}

	void Input::UpdateKeyUp(Key& key)
	{
		if (key.bPressed == true)
		{
			key.state = eKeystate::Up;
		}
		else
		{
			key.state = eKeystate::None;
		}
		key.bPressed = false;
	}
	
	void Input::clearKeys()
	{
		for (Key& key : Keys)
		{
			if (key.state == eKeystate::Pressed || key.state == eKeystate::Down)
				key.state = eKeystate::Up;
			else if (key.state == eKeystate::Up)
				key.state = eKeystate::None;

			key.bPressed = false;
		}
	}

	void Input::getMousePositionByWindow()
	{
		POINT mousePos = {};
		GetCursorPos(&mousePos);

		ScreenToClient(application.GetHwnd(), &mousePos);

		UINT width = application.GetWidth();
		UINT height = application.GetHeight();

		mMousePosition.x = -1.0f;
		mMousePosition.y = -1.0f;

		if ((UINT)mousePos.x < width && (UINT)mousePos.x > 0)
		{
			mMousePosition.x = (float)mousePos.x;
		}
		

		if ((UINT)mousePos.y < height && (UINT)mousePos.y > 0)
		{
			mMousePosition.y = (float)mousePos.y;
		}
	
	}

	void Input::Initialize()
	{
		CreateKeys();
	}

	void Input::Update()
	{
		UpdateKeys();
	}


}
