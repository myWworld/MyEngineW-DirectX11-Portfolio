#pragma once

#include "MEFmod.h"
#include "MEResource.h"


namespace ME
{
	using namespace math;

	class AudioClip:public Resource
	{
	public:

		AudioClip();
		virtual ~AudioClip();

		HRESULT Load(const std::wstring& path) override;

		void Play();
		void Stop();
		void Set3DAttributes(const Vector3 pos);
		void SetLoop(bool loop) { mbLoop = loop; }



	private:

		FMOD::Sound* mSound;
		FMOD::Channel* mChannel;
		float mMinDistance;
		float mMaxDistance;
		bool mbLoop;
	};

}
