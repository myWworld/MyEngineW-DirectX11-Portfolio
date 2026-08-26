#include "METimerDecision.h"

#include "../MyEngine_Source/FSMBrainCore.h"

namespace ME
{
    bool TimerDecision::CheckDecision(
        FSMBrainCore* brain,
        IFSMContext& context)
    {
        if (brain == nullptr)
            return false;

        return brain->GetStateElapsedTime()
            >= mCheckTime;
    }
}