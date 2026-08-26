#pragma once

#include "../MyEngine_Source/MEFSMTask.h"

namespace ME
{
    class MoveToTargetTask final : public FSMTask
    {
    public:
        MoveToTargetTask()
            : mSpeed(100.0f)
            , mStoppingDistance(50.0f)
        {
            bIsLoop = true;

            BindProperty("Speed", &mSpeed);
            BindProperty(
                "StoppingDistance",
                &mStoppingDistance
            );
        }

    protected:
        void OnExecute(
            FSMBrainCore* brain,
            IFSMContext& context) override;

    private:
        float mSpeed;
        float mStoppingDistance;
    };
}