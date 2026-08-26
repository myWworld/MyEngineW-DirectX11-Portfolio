#include "MERandomWanderTask.h"

#include "../MyEngine_Source/IFSMContext.h"

namespace ME
{
    void RandomWanderTask::OnEnter(
        FSMBrainCore* brain,
        IFSMContext& context)
    {
        context.SelectRandomPatrolTarget(
            mPatrolRadius
        );
    }

    void RandomWanderTask::OnExecute(
        FSMBrainCore* brain,
        IFSMContext& context)
    {
        const bool moving =
            context.MoveToPatrolTarget(
                mSpeed,
                mStoppingDistance
            );

        if (!moving)
        {
            bIsFinished = true;
        }
    }
}