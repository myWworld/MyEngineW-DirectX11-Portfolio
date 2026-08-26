
#include "CommonInclude.h"

namespace ME {

	enum class eKeystate
	{
		Down,
		Pressed,
		Up,
		None,

	};

	enum class eKeyCode
	{
		Q,W,E,R,T,Y,U,I,O,P
		,A,S,D,F,G,H,J,K,L
		,Z,X,C,V,B,N,M,Left,Right,Down,Up
		,Space,Shift,Enter
		,LeftMous,RightMouse
		,End,
	};

	class Input
	{

	public:
		struct Key
		{
			eKeyCode keyCode;
			eKeystate state;
			bool bPressed;

		};

		static void Initialize();
		static void Update();

		__forceinline static bool GetKeyDown(eKeyCode code) { return Keys[(UINT)code].state == eKeystate::Down; }
		__forceinline static bool GetKeyUp(eKeyCode code){ return Keys[(UINT)code].state == eKeystate::Up; }
		__forceinline static bool GetKey(eKeyCode code) {	return Keys[(UINT)code].state == eKeystate::Pressed;}
		__forceinline static math::Vector2 GetMousePos() { return mMousePosition; }

	private:

		static void CreateKeys();
		static void UpdateKeys();
		static void UpdateKey(Key& key);
		static bool isKeyDown(eKeyCode code);

		static void UpdateKeyDown(Key& key);
		static void UpdateKeyUp(Key& key);

		static void clearKeys();
		static void getMousePositionByWindow();

	private:

		static std::vector<Key> Keys;
		static math::Vector2 mMousePosition;
	};
}
