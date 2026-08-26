#pragma once

#include "../MyEngine_Source/MEFSMTask.h"

namespace ME
{
    class DestroyTask final : public FSMTask
    {
    public:
        DestroyTask()
        {
            bIsLoop = true;
        }
    protected:

        void OnEnter(FSMBrainCore* brain, IFSMContext& context) override;

        void OnExecute(FSMBrainCore* brain, IFSMContext& context) override;
    private:

        float mElapsedTime = 0.0f;

        // DESTROY 상태에 들어온 후 유지할 시간
        float mDeathDelay = 5.0f;

        // DestroyOwner 중복 호출 방지
        bool mbDestroyRequested = false;
    };
}