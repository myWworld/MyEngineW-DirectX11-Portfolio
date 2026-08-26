#include "MEMoveToTargetTask.h"

#include "../MyEngine_Source/IFSMContext.h"
#include "../MyEngine_Source/FSMBrainCore.h"

namespace ME
{
    void MoveToTargetTask::OnExecute(
        FSMBrainCore* brain,
        IFSMContext& context)
    {
        if (!context.HasTarget())
        {
            brain->SendFSMEvent("IDLE");
            return;
        }

        const bool moving =
            context.MoveToTarget(
                mSpeed,
                mStoppingDistance
            );

        if (!moving)
        {
            bIsFinished = true;
        }
    }
}