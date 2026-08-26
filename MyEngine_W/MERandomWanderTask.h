#pragma once

#include "../MyEngine_Source/MEFSMTask.h"

namespace ME
{
    class RandomWanderTask final : public FSMTask
    {
    public:
        RandomWanderTask()
            : mSpeed(20.0f)
            , mPatrolRadius(40.0f)
            , mStoppingDistance(50.0f)
        {
            bIsLoop = true;

            BindProperty("Speed", &mSpeed);
            BindProperty(
                "PatrolRadius",
                &mPatrolRadius
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
        float mSpeed;
        float mPatrolRadius;
        float mStoppingDistance;
    };
}