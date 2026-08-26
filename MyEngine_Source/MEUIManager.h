#pragma once
#include "CommonInclude.h"
#include "../MyEngine_Source/MEUIBase.h"

namespace ME
{
	class UIManager
	{
	public:
		static void Initailize();
		static void Render();
		static void OnLoad(enums::eUIType type);
		static void Update();
		static void LateUpdate();
		static void OnComplete(UIBase* addUI);
		static void OnFail();
		static void Release();

		static void Push(enums::eUIType type);
		static void Pop(enums::eUIType type);

		// 액터/월드 오브젝트에 붙는 UI. 예: HP Bar, 이름표, 상태이상 아이콘 등.
		static UIBase* CreateAttachedUI(enums::eUIType type, GameObject* owner);
		static bool RemoveAttachedUI(UIBase* targetUI);
		static void RemoveAttachedUI(GameObject* owner);

	private:

		static UIBase* CreateUIInstance(enums::eUIType type);

		// 메뉴/팝업처럼 타입당 하나만 써도 되는 UI.
		static std::unordered_map<enums::eUIType, UIBase*> mUIs;
		static std::stack<UIBase*> mUIBases;
		static std::queue<enums::eUIType> mRequestUIQueue;

		// HP Bar처럼 같은 타입이 여러 개 존재할 수 있는 UI.
		static std::unordered_map<enums::eUIType, std::vector<std::unique_ptr<UIBase>>> mAttachedUIs;

		static UIBase* mActiveUI;

	};

}
