#include "MEDistanceDecision.h"

#include "../MyEngine_Source/IFSMContext.h"

namespace ME
{
    bool DistanceDecision::CheckDecision(
        FSMBrainCore* brain,
        IFSMContext& context)
    {
        if (!context.HasTarget())
            return false;

        return context.GetTargetDistanceSquared()
            <= mTargetDist * mTargetDist;
    }
}