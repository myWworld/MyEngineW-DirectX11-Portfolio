#pragma once

#include "../MyEngine_Source/MEFSMTask.h"

#include <string>

namespace ME
{
    class PlayAnimTask final : public FSMTask
    {
    public:
        PlayAnimTask()
            : mAnimName()
            , mbIsAnimLoop(false)
        {
            BindProperty("AnimName", &mAnimName);

            BindProperty("IsLoop", &mbIsAnimLoop);
        }

    protected:
        void OnEnter(FSMBrainCore* brain, IFSMContext& context) override;

        void OnExecute(FSMBrainCore* brain, IFSMContext& context) override;

    private:
        std::string mAnimName;
        bool mbIsAnimLoop;
    };
}