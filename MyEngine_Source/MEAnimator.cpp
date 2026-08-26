#include "MEAnimator.h"
#include "MEResources.h"
#include "METexture.h"


namespace ME
{
	Animator::Animator()
		:Component(enums::eComponentType::Animator)
		,mActiveAnimation(nullptr)
		,mAnimations{}
		,mbLoop(false)
		,mEvents {}
	{
	}
	Animator::~Animator()
	{
		for (auto& iter : mAnimations)
		{
			delete iter.second;
			iter.second = nullptr;
		}

		for (auto &iter: mEvents )
		{
			delete iter.second;
			iter.second = nullptr;
		}
	}
	void Animator::Initialize()
	{
	}
	void Animator::Update()
	{
		if (mActiveAnimation)
		{
			mActiveAnimation->Update();

			Events* events = FindEvents(mActiveAnimation->GetName());


			if (mActiveAnimation->IsComplete() == true)
			{	
				if (mbLoop == true)
					mActiveAnimation->Reset();

				if (events)
				{
					events->CompleteEvent();
				}

			}

		}
	}

	void Animator::LateUpdate()
	{
	}

	void Animator::Render()
	{
		//if (mActiveAnimation)
		//{
		//	mActiveAnimation->Render(hdc);
		//}

	}


	void Animator::CreateAnimation(const std::wstring& name,
		graphics::Texture* spriteSheet,
		Vector2 leftTop, Vector2 size, 
		Vector2 offset
		,float duration
		,UINT spriteLength, UINT spriteLength2 )
	{
		Animation* animation = nullptr;

		animation = FindAnimation(name);

		if (animation != nullptr)
			return;

		animation = new Animation();
		animation->SetName(name);
		animation->CreateAnimation(name, spriteSheet, leftTop,
			size, offset
			, duration, spriteLength,spriteLength2);

		animation->SetAnimator(this);

		Events* events = new Events();
		mEvents.insert({ name, events });

		mAnimations.insert({ name, animation });

	}


	void Animator::CreateAnimationByFolder(/*const std::wstring& name
		, const std::wstring& path
		, Vector2 offset
		, float duration*/)
	{
		//Animation* animation = nullptr;
		//animation = FindAnimation(name);
		//
		//if (animation != nullptr)
		//{
		//	return;
		//}
		//
		//int filecount = 0;
		//std::filesystem::path fs(path);
		//std::vector<graphics::Texture*> images = {};
		//
		//for (auto& p : std::filesystem::recursive_directory_iterator(fs))
		//{
		//	std::wstring fileName = p.path().filename();
		//	std::wstring fullName = p.path();
		//
		//	graphics::Texture* texture = Resources::Load<graphics::Texture>(fileName, fullName);
		//	images.push_back(texture);
		//	filecount++;
		//}
		//
		//UINT sheetWidth = images[0]->GetWidth() * filecount;
		//UINT sheetHeight = images[0]->GetHeight();
		//graphics::Texture* spriteSheet = graphics::Texture::Create(name, sheetWidth, sheetHeight);
		//
		//UINT imageWidth = images[0]->GetWidth();
		//UINT imageHeight = images[0]->GetHeight();
		//
		//for (size_t i = 0; i < images.size(); i++)
		//{
		//	BitBlt(spriteSheet->GedHdc()
		//		, i * imageWidth, 0
		//		, imageWidth, imageHeight
		//		, images[i]->GedHdc(), 0, 0, SRCCOPY);
		//}
		//
		//CreateAnimation(name, spriteSheet
		//	, Vector2::Zero, Vector2(imageWidth, imageHeight)
		//	, offset, filecount, duration);
	}

	Animation* Animator::FindAnimation(const std::wstring& name)
	{
		auto iter = mAnimations.find(name);
		if (iter == mAnimations.end())
			return nullptr;

		return iter->second;


	}
	void Animator::PlayAnimation(const std::wstring& name, bool loop)
	{
		Animation* animation = FindAnimation(name);
		if (animation == nullptr)
			return;


		if (mActiveAnimation)
		{
			Events* currentEvents
				= FindEvents(mActiveAnimation->GetName());
			
			if (currentEvents)
			{
				currentEvents->EndEvent();
			}
		}

		Events* nextEvents
			= FindEvents(animation->GetName());

		if (nextEvents)
		{
			nextEvents->StartEvent();
		}

		mActiveAnimation = animation;
		mActiveAnimation->Reset();
		mbLoop = loop;

	}

	Animator::Events* Animator::FindEvents(const std::wstring & name)
	{
		auto iter = mEvents.find(name);
		if (iter == mEvents.end())
			return nullptr;

		return iter->second;
	}

	std::function<void()>& Animator::GetStartEvent(const std::wstring& name)
	{
		Events* events = FindEvents(name);
		return events->StartEvent.mEvent;

	}
	std::function<void()>& Animator::GetCompleteEvent(const std::wstring& name)
	{
		Events* events = FindEvents(name);
		return events->CompleteEvent.mEvent;
	}
	std::function<void()>& Animator::GetEndEvent(const std::wstring& name)
	{
		Events* events = FindEvents(name);
		return events->EndEvent.mEvent;
	}
}