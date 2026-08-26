#pragma once

#include "../MyEngine_Source/MEFSMDecision.h"

namespace ME
{
    class DistanceDecision final
        : public FSMDecision
    {
    public:
        DistanceDecision()
            : mTargetDist(100.0f)
        {
            BindProperty(
                "TargetDist",
                &mTargetDist
            );
        }

    protected:
        bool CheckDecision(
            FSMBrainCore* brain,
            IFSMContext& context) override;

    private:
        float mTargetDist;
    };
}