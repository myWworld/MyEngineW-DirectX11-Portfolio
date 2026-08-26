#include "MEAudioListener.h"
#include "MEGameObject.h"
#include "MEFmod.h"
#include "METransform.h"


namespace ME
{
    AudioListener::AudioListener()
        :Component(enums::eComponentType::AudioListener)
    {
    }
    AudioListener::~AudioListener()
    {
    }
    void AudioListener::Initialize()
    {
    }
    void AudioListener::Update()
    {
    }
    void AudioListener::LateUpdate()
    {
        Transform* tr = nullptr;
        tr = GetOwner()->GetComponent<Transform>();

        Vector3 pos;

        Fmod::Set3DListenerAttributes(pos);
    }
    void AudioListener::Render()
    {
    }
}