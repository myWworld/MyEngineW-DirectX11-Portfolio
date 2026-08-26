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
            , mbIsLoop(false)
        {
            BindProperty(
                "AnimName",
                &mAnimName
            );

            BindProperty(
                "IsLoop",
                &mbIsLoop
            );
        }

    protected:
        void OnEnter(
            FSMBrainCore* brain,
            IFSMContext& context) override;

        void OnExecute(
            FSMBrainCore* brain,
            IFSMContext& context) override;

    private:
        std::string mAnimName;
        bool mbIsLoop;
    };
}