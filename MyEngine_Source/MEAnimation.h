#pragma once

#include "METexture.h"
#include "MEResource.h"
#include "MESpriteRenderer.h"

namespace ME
{


	class Animation:public Resource
	{
	public:

		struct Sprite
		{
			Vector2 leftTop;
			Vector2 size;
			Vector2 offset;
			float duration;

			Sprite()
				:leftTop(Vector2::Zero)
				, size(Vector2::Zero)
				, offset(Vector2::Zero)
				, duration(0.0f)
			{

			}
		};

		Animation();
		~Animation();

		HRESULT Save(const std::wstring& path) override;
		HRESULT Load(const std::wstring& path) override;

		void Update();
		void Render();

		void CreateAnimation(const std::wstring& name, graphics::Texture* spriteSheet
			, Vector2 leftTop
			, Vector2 size
			, Vector2 offset
			, float duration
			, UINT spriteLength
			, UINT spriteLength2 = 0);

		void Reset();

		bool IsComplete() const{ return mbComplete;}
		void SetAnimator(class Animator* animator) { mAnimator = animator; }

	private:
		
		void InsertRightAnimationSheets(Vector2 leftTop, Vector2 size
			, Vector2 offset, float duration
			, UINT spriteLength1
			, UINT spriteLength2);

		void InsertLeftAnimationSheets(Vector2 leftTop, Vector2 size
				, Vector2 offset, float duration
				, UINT spriteLength1
				, UINT spriteLength2);

	private:

		class Animator* mAnimator;
		graphics::Texture* mTexture;

		std::vector<Sprite> mAnimationSheet;
		int mIndex;
		float mTime;
		bool mbComplete;

	};

}
