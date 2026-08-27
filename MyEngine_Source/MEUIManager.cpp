#include "MEUIManager.h"
#include "MEUIHUD.h"
#include "MEUIButton.h"
#include <algorithm>

namespace ME
{
	std::unordered_map<enums::eUIType, UIBase*> UIManager::mUIs = {};
	std::vector<UIBase*> UIManager::mUIBases = {};
	std::queue<enums::eUIType> UIManager::mRequestUIQueue = {};
	std::unordered_map<enums::eUIType, std::vector<std::unique_ptr<UIBase>>> UIManager::mAttachedUIs = {};
	UIBase* UIManager::mActiveUI = nullptr;

	void UIManager::Initailize()
	{
		// HP Bar는 액터마다 여러 개 생기는 UI이므로 mUIs에 싱글 인스턴스로 넣지 않는다.

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
		// 밑(Bottom)부터 위(Top)로 차례대로 그려서 Z-Order 역전 방지
		for (UIBase* uiBase : mUIBases)
		{
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
		auto iter = mUIs.find(type);

		if (iter == mUIs.end())
		{
			UIBase* newUI = CreateUIInstance(type);

			if (newUI == nullptr)
			{
				OnFail();
				return;
			}

			mUIs.insert({ type, newUI });
			iter = mUIs.find(type);
		}

		OnComplete(iter->second);
	}

	void UIManager::Update()
	{
		// 가장 위(Top)에 있는 UI가 입력을 먼저 처리하도록 뒤에서부터 순회
		for (auto it = mUIBases.rbegin(); it != mUIBases.rend(); ++it)
		{
			if (*it)
			{
				(*it)->Update();
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

		// 큐에 쌓인 요청을 한 프레임에 전부 처리
		while (!mRequestUIQueue.empty())
		{
			enums::eUIType requestUI = mRequestUIQueue.front();
			mRequestUIQueue.pop();
			OnLoad(requestUI);
		}

	}
	void UIManager::LateUpdate()
	{
		for (auto it = mUIBases.rbegin(); it != mUIBases.rend(); ++it)
		{
			if (*it)
			{
				(*it)->LateUpdate();
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
			// 복사 오버헤드 없이 순회하며 InActive 처리
			for (UIBase* uiBase : mUIBases)
			{
				if (uiBase)
				{
					uiBase->InActive();
				}
			}
		}

		mUIBases.push_back(addUI);
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

		mUIBases.clear();

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
		if (mUIBases.empty())
			return;

		// vector를 스택처럼 다루기 위해 뒤에서부터 검색
		for (auto it = mUIBases.rbegin(); it != mUIBases.rend(); ++it)
		{
			if ((*it)->GetType() == type)
			{
				UIBase* targetUI = *it;

				// 역방향 반복자를 정방향 반복자로 변환 (erase를 위해)
				auto eraseIt = std::prev(it.base());

				if (targetUI->IsFullScreen())
				{
					// 전체화면 창이 꺼지는 경우, 그 바로 아래(밑)에 있는 유효한 UI를 찾아 켠다.
					for (auto belowIt = std::make_reverse_iterator(eraseIt); belowIt != mUIBases.rend(); ++belowIt)
					{
						if (*belowIt)
						{
							(*belowIt)->Active();
							break;
						}
					}
				}

				targetUI->UIClear();
				mUIBases.erase(eraseIt); // 임시 스택 없이 깔끔하게 중간/끝 요소 삭제
				return;
			}
		}
	}




}
