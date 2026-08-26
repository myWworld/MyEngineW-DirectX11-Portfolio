#pragma once

#include "../MyEngine_Source/MEFSMDecision.h"

namespace ME
{
    class DetectTargetDecision final
        : public FSMDecision
    {
    public:
        DetectTargetDecision()
            : mDetectRadius(50.0f)
        {
            BindProperty(
                "DetectRadius",
                &mDetectRadius
            );
        }

    protected:
        bool CheckDecision(
            FSMBrainCore* brain,
            IFSMContext& context) override;

    private:
        float mDetectRadius;
    };
}