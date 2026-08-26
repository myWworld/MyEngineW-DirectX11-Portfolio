#include "MEFmod.h"

namespace ME
{
	FMOD::Studio::System* Fmod::mSystem = nullptr;
	FMOD::System* Fmod::mCoreSystem = nullptr;


	void Fmod::Initialize()
	{
		void* extraDiverDate = nullptr;

		FMOD::Studio::System::create(&mSystem);

		if (mSystem == nullptr)
			return;

		mSystem->getCoreSystem(&mCoreSystem);
		mCoreSystem->setSoftwareFormat(0, FMOD_SPEAKERMODE_5POINT1, 0);

		mSystem->initialize(1024, FMOD_STUDIO_INIT_NORMAL, FMOD_INIT_NORMAL, extraDiverDate);

	}

	bool Fmod::CreateSound(const std::string& path, FMOD::Sound** sound)
	{
		if (FMOD_OK != mCoreSystem->createSound(path.c_str(), FMOD_3D, 0, sound));
			return false;

		return true;	
	}

	void Fmod::SoundPlay(FMOD::Sound* sound, FMOD::Channel** channel)
	{
		mCoreSystem->playSound(sound, 0, false, channel);

	}

	void Fmod::Set3DListenerAttributes(const Vector3& pos)
	{
		FMOD_VECTOR fmodPos(pos.x, pos.y, pos.z);
		FMOD_VECTOR fmodVel(0.0f, 0.0f, 0.3f);
		FMOD_VECTOR fmodForward(0.0f, 0.0f, 1.0f);
		FMOD_VECTOR fmodUp(0.0f, 1.0f, 0.0f);

		mCoreSystem->set3DListenerAttributes(0, &fmodPos, &fmodVel, &fmodForward, &fmodUp);
	}

	void Fmod::Release()
	{
		mSystem->release();
		mSystem = nullptr;
	}
}
