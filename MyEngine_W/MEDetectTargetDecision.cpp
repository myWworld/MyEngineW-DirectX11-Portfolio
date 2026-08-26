#include "MEDetectTargetDecision.h"

#include "../MyEngine_Source/IFSMContext.h"

namespace ME
{
    bool DetectTargetDecision::CheckDecision(
        FSMBrainCore* brain,
        IFSMContext& context)
    {
        return context.DetectTarget(
            mDetectRadius
        );
    }
}