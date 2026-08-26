#include "MEAnimation.h"
#include "METime.h"
#include "MEAnimator.h"
#include "METransform.h"
#include "MESpriteRenderer.h"
#include "MEGameObject.h"
#include "MERenderer.h"
#include "MECamera.h"
#include "MESceneManager.h"

namespace ME
{
	Animation::Animation()
		:Resource(enums::eResourceType::Animation)
		,mAnimator(nullptr)
		,mTexture(nullptr)
		,mAnimationSheet{}
		,mIndex(-1)
		,mTime(0.0f)
		,mbComplete(false)
	{
	}

	Animation::~Animation()
	{
	}
	HRESULT Animation::Save(const std::wstring& path)
	{
		return E_NOTIMPL;
	}
	HRESULT Animation::Load(const std::wstring& path)
	{
		return E_NOTIMPL;
	}
	void Animation::Update()
	{

		if (mbComplete)
			return;

		mTime += Time::DeltaTime();

		if (mAnimationSheet[mIndex].duration < mTime)
		{
			mTime = 0;

			if (mIndex < mAnimationSheet.size()-1)
			{
				
				mIndex++;
			}
			else
			{
				mbComplete = true;
			}
		}
	}

	void Animation::Render()
	{
			if (mTexture == nullptr)
				return;

		
	}

	void Animation::CreateAnimation(const std::wstring& name, graphics::Texture* spriteSheet, Vector2 leftTop, Vector2 size
		, Vector2 offset,float duration, UINT spriteLength1, UINT spriteLength2)
	{
		mTexture = spriteSheet;
		
		SetName(name);

		std::wstring Direction = name.substr(name.size() - 1);

		if (Direction == L"R")
		{
			InsertRightAnimationSheets(leftTop, size, offset, duration, spriteLength1, spriteLength2);
		}
		else if (Direction == L"L")
		{
			InsertLeftAnimationSheets(leftTop, size, offset, duration, spriteLength1, spriteLength2);
		}
		
		
	}
	void Animation::Reset()
	{

		mTime = 0;
		mIndex = 0;
		mbComplete = false;
	}

	void Animation::InsertRightAnimationSheets(Vector2 leftTop, Vector2 size
		, Vector2 offset, float duration
		, UINT spriteLength1
		, UINT spriteLength2)
	{

		int count = (spriteLength2 > 0) ? 2 : 1;
		UINT Length = spriteLength1;
		Vector2 pos = leftTop;


		for (size_t j = 0; j < count; j++)
		{

			if (j == 1)
			{
				Length = spriteLength2;
				pos.y += size.y;

				if (pos.x > 0)
				{
					pos.x = 0;
				}

			}

			for (size_t i = 0; i < Length; i++)
			{
				Sprite sprite = {};
				sprite.leftTop.x = pos.x + (size.x * i);
				sprite.leftTop.y = pos.y;
				sprite.size = size;
				sprite.offset = offset;
				sprite.duration = duration;

				mAnimationSheet.push_back(sprite);
			}
		}
	}


	void Animation::InsertLeftAnimationSheets(Vector2 leftTop, Vector2 size
		, Vector2 offset, float duration
		, UINT spriteLength1
		, UINT spriteLength2)
	{
		int count = (spriteLength2 > 0) ? 2 : 1;
		UINT Length = spriteLength1;
		Vector2 pos = leftTop;
		


	//for (size_t j = 0; j < count; j++)
	//{
	//
	//	if (j == 1)
	//	{
	//		Length = spriteLength2;
	//		pos.y += size.y;
	//
	//		if (pos.x < width - size.x)
	//		{
	//			pos.x = width - size.x;
	//		}
	//
	//	}
	//
	//	for (size_t i = 0; i < Length; i++)
	//	{
	//		Sprite sprite = {};
	//		sprite.leftTop.x = pos.x - (size.x * i);
	//		sprite.leftTop.y = pos.y;
	//		sprite.size = size;
	//		sprite.offset = offset;
	//		sprite.duration = duration;
	//
	//		mAnimationSheet.push_back(sprite);
	//	}
	//}
	}


}