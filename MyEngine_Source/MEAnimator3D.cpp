#include "MEAnimator3D.h"
#include "MEResources.h"
#include "METexture.h"
#include "MEConstantBuffer.h"
#include "MERenderer.h"
#include "METransform.h"
#include "MEGameObject.h"
#include "MEMesh.h"
#include "MEMaterial.h"
#include "METime.h"


namespace ME
{
	Animator3D::Animator3D()
		:Component(enums::eComponentType::Animator)
		, mActiveAnimation(nullptr)
		, mAnimations{}
		, mbLoop(false)
		//, mEvents{}
		, mModelType(enums::eModelType::StaticBone)
		, mSkeleton{}
		, mModelMatrix{}
		, mCurrentTime(0.0f)
		, mPrevRootPos(Vector3::Zero)
		, mRootMotionTotalOffset(Vector3::Zero)
		, mRootMotionBasePosition(Vector3::Zero)
	{
	}
	Animator3D::~Animator3D()
	{
		mAnimations.clear();
		//mEvents.clear();
	}
	void Animator3D::Initialize()
	{
	}
	void Animator3D::Update()
	{
		if (mActiveAnimation)
		{
			
			float duration = mActiveAnimation->GetDuration(); //총 몇 틱
			float ticksPerSec = mActiveAnimation->GetTickersPerSecond(); //1초에 몇 틱
			float totalTime = duration / ticksPerSec; //실제 시간

			float prevRatio = mCurrentTime / totalTime;

			mCurrentTime += Time::DeltaTime();
			float currRatio = mCurrentTime / totalTime;

			bool bLooped = false;
			if (currRatio > 1.0f)
			{
				mbComplete = true;
				if (mbLoop)
				{
					bLooped = true;
					mCurrentTime = fmod(mCurrentTime, totalTime); // 시간 초기화
					currRatio = mCurrentTime / totalTime;
					mbComplete = false;

					mPrevRootPos = Vector3::Zero;
				}
				else
				{
					currRatio = 1.0f; // 루프가 아니면 1.0에서 고정
				}
			}

			auto iter = mAnimEvents.find(mActiveAnimation->GetName());
			if (iter != mAnimEvents.end())
			{
				for (const auto& ev : iter->second)
				{
					if (bLooped)
					{
						// 루프가 돌았을 때: 
						// 1. 이전 프레임 ~ 끝(1.0)까지 남았던 이벤트 실행
						// 2. 처음(0.0) ~ 현재 프레임까지 지나친 이벤트 실행
						if (ev.normalizedTime >= prevRatio || ev.normalizedTime <= currRatio)
						{
							ev.callback();
						}
					}
					else
					{
						// 일반적인 상황: 이전 비율 < 이벤트 시간 <= 현재 비율 ->프레임 건너뛰어지는 현상 방지(sweep식으로)
						if (ev.normalizedTime > prevRatio && ev.normalizedTime <= currRatio)
						{
							ev.callback();
						}
					}
				}
			}

			mActiveAnimation->UpdateAnimation(mCurrentTime, &mSkeleton, this);

	
		//	if (mbComplete)
		//	{
		//		Events* events = FindEvents(mActiveAnimation->GetName());
		//		if (events) events->CompleteEvent();
		//	}

		}
	}

	void Animator3D::LateUpdate()
	{
	}

	void Animator3D::Render()
	{
		//if (mActiveAnimation)
		//{
		//	mActiveAnimation->Render(hdc);
		//}
		Bind();


	}

	void Animator3D::render()
	{
	
	

	}


	void Animator3D::Bind()
	{

		if (mModelType == enums::eModelType::SkinnedMesh)
		{
			graphics::AnimationCB cbData; // {} -> to none Zero-초기화를 매번하는 작업 스택에 할당되므로 오버헤드가 적음. 하지만 매번 초기화하는 작업이 필요함.
			// -> 지역캐시성을 높이기 위해 지역변수로 선언하는 것이 좋음.

			for (unsigned i = 0; i < mSkeleton.mBones.size(); i++)
			{
				cbData.BoneMatrices[i] = mSkeleton.mBones[i].FinalTransform;
		
			}

			graphics::ConstantBuffer* cb = renderer::constantBuffers[CBSLOT_ANIMATION];

			cb->SetData(&cbData);
			cb->Bind(graphics::eShaderStage::All);
		}

		
	
	}

	void Animator3D::boneTransformBind()
	{
		Transform* tr = GetOwner()->GetComponent<Transform>();
		Vector3 position = tr->GetPosition();
		tr->Bind();

	}


	void Animator3D::CreateAnimation(const std::wstring& name, const std::wstring& path)
	{
		if (FindAnimation(name) != nullptr)
			return;

		std::shared_ptr<Animation3D> animation = Resources::Load<Animation3D>(name, path);

		//auto events = std::make_unique<Events>();

		//mEvents.insert({ name, std::move(events)});
		mAnimations.insert({ name, animation.get()});

	}




	Animation3D* Animator3D::FindAnimation(const std::wstring& name)
	{
		auto iter = mAnimations.find(name);
		if (iter == mAnimations.end())
			return nullptr;

		return iter->second;


	}
	void Animator3D::PlayAnimation(const std::wstring& name, bool loop)
 	{
		std::shared_ptr<Animation3D> animation = Resources::Find<Animation3D>(name);

		if (animation == nullptr)
			return;

		if (mActiveAnimation && mActiveAnimation == animation.get())
			return;

		if (mActiveAnimation)
		{
			auto iter = mBehaviours.find(mActiveAnimation->GetName());
			if (iter != mBehaviours.end() && iter->second.Exit)
			{
				iter->second.Exit();
			}

			/*Events* currentEvents
				= FindEvents(mActiveAnimation->GetName());

			if (currentEvents)
			{
				currentEvents->EndEvent();
			}*/
		}

		/*Events* nextEvents
			= FindEvents(animation->GetName());

		if (nextEvents)
		{
			nextEvents->StartEvent();
		}*/

		auto nextIter = mBehaviours.find(animation->GetName());
		if (nextIter != mBehaviours.end() && nextIter->second.Enter)
		{
			nextIter->second.Enter(); 
		}

		mActiveAnimation = animation.get();
		this->mPrevRootPos = Vector3::Zero;
		mbLoop = loop;

		mCurrentTime = 0.0f;
		mbComplete = false;

	}

	void Animator3D::AddEvent(const std::wstring& animName, const std::wstring& eventName, float normalizedTime, std::function<void()> callback)
	{
		mAnimEvents[animName].push_back({ eventName, normalizedTime, callback });
		std::sort(mAnimEvents[animName].begin(), mAnimEvents[animName].end());
	}

	void Animator3D::AddEnterBehaviour(const std::wstring& animName, std::function<void()> callback)
	{
		mBehaviours[animName].Enter = std::move(callback);
	}
	void Animator3D::AddExitBehaviour(const std::wstring& animName, std::function<void()> callback)
	{
		mBehaviours[animName].Exit = std::move(callback);
	}


	//Animator3D::Events* Animator3D::FindEvents(const std::wstring& name)
	//{
	//	auto iter = mEvents.find(name);
	//	if (iter == mEvents.end())
	//		return nullptr;
	//
	//	return iter->second.get();
	//}
	//
	//std::function<void()>& Animator3D::GetStartEvent(const std::wstring& name)
	//{
	//	Events* events = FindEvents(name);
	//	return events->StartEvent.mEvent;
	//
	//}
	//std::function<void()>& Animator3D::GetCompleteEvent(const std::wstring& name)
	//{
	//	Events* events = FindEvents(name);
	//	return events->CompleteEvent.mEvent;
	//}
	//std::function<void()>& Animator3D::GetEndEvent(const std::wstring& name)
	//{
	//	Events* events = FindEvents(name);
	//	return events->EndEvent.mEvent;
	//}


	std::unordered_map<std::string, std::string> BoneNameManualMapping = {
	
		{"righttoe_end", "rightfoottoebase_end"},

	};
}