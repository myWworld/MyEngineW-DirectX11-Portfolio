#include "MEUIManager.h"
#include "MEUIHUD.h"
#include "MEUIButton.h"
#include <algorithm>

namespace ME
{
	std::unordered_map<enums::eUIType, UIBase*> UIManager::mUIs = {};
	std::stack<UIBase*> UIManager::mUIBases = {};
	std::queue<enums::eUIType> UIManager::mRequestUIQueue = {};
	std::unordered_map<enums::eUIType, std::vector<std::unique_ptr<UIBase>>> UIManager::mAttachedUIs = {};
	UIBase* UIManager::mActiveUI = nullptr;

	void UIManager::Initailize()
	{
		// HP Bar는 액터마다 여러 개 생기는 UI이므로 mUIs에 싱글 인스턴스로 넣지 않는다.
		UIButton* button = new UIButton();
		mUIs.insert({ enums::eUIType::Button, button });
	}

	UIBase* UIManager::CreateUIInstance(enums::eUIType type)
	{
		switch (type)
		{
		case enums::eUIType::HpBar:
			return new UIHUD();
		case enums::eUIType::Button:
			return new UIButton();
		default:
			return nullptr;
		}
	}

	UIBase* UIManager::CreateAttachedUI(enums::eUIType type, GameObject* owner)
	{
		if (owner == nullptr)
			return nullptr;

		std::unique_ptr<UIBase> ui(CreateUIInstance(type));
		if (ui == nullptr)
			return nullptr;

		ui->SetOwner(owner);
		ui->Initialize();
		ui->Active();

		UIBase* rawPtr = ui.get();
		mAttachedUIs[type].push_back(std::move(ui));

		return rawPtr;
	}

	bool UIManager::RemoveAttachedUI(UIBase* targetUI)
	{
		if (targetUI == nullptr)
			return false;

		for (auto& pair : mAttachedUIs)
		{
			std::vector<std::unique_ptr<UIBase>>& uiList = pair.second;

			auto iter = std::find_if(uiList.begin(), uiList.end(),
				[targetUI](const std::unique_ptr<UIBase>& ui)
				{
					return ui.get() == targetUI;
				});

			if (iter != uiList.end())
			{
				(*iter)->UIClear();
				uiList.erase(iter);
				return true;
			}
		}

		return false;
	}

	void UIManager::RemoveAttachedUI(GameObject* owner)
	{
		if (owner == nullptr)
			return;

		for (auto& pair : mAttachedUIs)
		{
			std::vector<std::unique_ptr<UIBase>>& uiList = pair.second;

			uiList.erase(std::remove_if(uiList.begin(), uiList.end(),
				[owner](const std::unique_ptr<UIBase>& ui)
				{
					if (ui != nullptr && ui->GetOwner() == owner)
					{
						ui->UIClear();
						return true;
					}
					return false;
				}), uiList.end());
		}
	}

	void UIManager::Render()
	{
		std::stack<UIBase*> uiBases = mUIBases;

		while (!uiBases.empty())
		{
			UIBase* uiBase = uiBases.top();
			uiBases.pop();

			if (uiBase)
			{
				uiBase->Render();
			}
		}

		for (auto& pair : mAttachedUIs)
		{
			for (std::unique_ptr<UIBase>& ui : pair.second)
			{
				if (ui)
				{
					ui->Render();
				}
			}
		}
	}

	void UIManager::OnLoad(enums::eUIType type)
	{
		std::unordered_map<enums::eUIType, UIBase*>::iterator iter
			= mUIs.find(type);

		if (iter == mUIs.end())
		{
			OnFail();
			return;
		}

		OnComplete(iter->second);
	}

	void UIManager::Update()
	{
		std::stack<UIBase*> uiBases = mUIBases;

		while (!uiBases.empty())
		{
			UIBase* uiBase = uiBases.top();
			uiBases.pop();

			if (uiBase)
			{
				uiBase->Update();
			}
		}

		for (auto& pair : mAttachedUIs)
		{
			for (std::unique_ptr<UIBase>& ui : pair.second)
			{
				if (ui)
				{
					ui->Update();
				}
			}
		}

		if (mRequestUIQueue.size() > 0)
		{
			enums::eUIType requestUI = mRequestUIQueue.front();
			mRequestUIQueue.pop();
			OnLoad(requestUI);
		}

	}
	void UIManager::LateUpdate()
	{
		std::stack<UIBase*> uiBases = mUIBases;

		while (!uiBases.empty())
		{
			UIBase* uiBase = uiBases.top();
			uiBases.pop();

			if (uiBase)
			{
				uiBase->LateUpdate();
			}
		}

		for (auto& pair : mAttachedUIs)
		{
			for (std::unique_ptr<UIBase>& ui : pair.second)
			{
				if (ui)
				{
					ui->LateUpdate();
				}
			}
		}
	}
	void UIManager::OnComplete(UIBase* addUI)
	{
		if (addUI == nullptr)
			return;

		addUI->Initialize();
		addUI->Active();
		addUI->Update();

		//만약에 현재 추가된 ui가 전체화면이라면 그것말고 모두 비활성화

		if (addUI->IsFullScreen())
		{
			std::stack<UIBase*> uiBases = mUIBases;

			while (!uiBases.empty())
			{
				UIBase* uiBase = uiBases.top();
				uiBases.pop();

				if (uiBase)
				{
					uiBase->InActive();
				}
			}
		}

		mUIBases.push(addUI);
		mActiveUI = nullptr;

	}
	void UIManager::OnFail()
	{
		mActiveUI = nullptr;
	}
	void UIManager::Release()
	{
		for (auto& pair : mAttachedUIs)
		{
			for (std::unique_ptr<UIBase>& ui : pair.second)
			{
				if (ui)
				{
					ui->UIClear();
				}
			}
			pair.second.clear();
		}
		mAttachedUIs.clear();

		while (!mUIBases.empty())
		{
			mUIBases.pop();
		}

		for (auto iter : mUIs)
		{
			delete iter.second;
			iter.second = nullptr;
		}
		mUIs.clear();
	}

	void UIManager::Push(enums::eUIType type)
	{
		mRequestUIQueue.push(type);
	}

	void UIManager::Pop(enums::eUIType type)
	{
		if (mUIBases.size() <= 0)
			return;

		std::stack<UIBase*> tempStack;

		UIBase* uibase = nullptr;

		while (mUIBases.size() > 0)
		{
			uibase = mUIBases.top();
			mUIBases.pop();

			if (uibase->GetType() != type)
			{
				tempStack.push(uibase);
				continue;
			}

			if (uibase->IsFullScreen())
			{
				std::stack<UIBase*> uiBases = mUIBases;

				while (!uiBases.empty())
				{
					UIBase* uiBase = uiBases.top();
					uiBases.pop();

					if (uiBase)
					{
						uiBase->Active();
						break;
					}
				}
			}

			uibase->UIClear();
		}

		while (tempStack.size() > 0)
		{
			uibase = tempStack.top();
			tempStack.pop();
			mUIBases.push(uibase);
		}
	}




}
