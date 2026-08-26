#include "MEAnimFinishDecision.h"

#include "../MyEngine_Source/IFSMContext.h"

namespace ME
{
    bool AnimFinishDecision::CheckDecision(
        FSMBrainCore* brain,
        IFSMContext& context)
    {
        return context.IsAnimationFinished();
    }
}