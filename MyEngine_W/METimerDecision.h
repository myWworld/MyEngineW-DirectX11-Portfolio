#pragma once

#include "../MyEngine_Source/MEFSMDecision.h"

namespace ME
{
    class TimerDecision final
        : public FSMDecision
    {
    public:
        TimerDecision()
            : mCheckTime(2.0f)
        {
            BindProperty(
                "CheckTime",
                &mCheckTime
            );
        }

    protected:
        bool CheckDecision(
            FSMBrainCore* brain,
            IFSMContext& context) override;

    private:
        float mCheckTime;
    };
}