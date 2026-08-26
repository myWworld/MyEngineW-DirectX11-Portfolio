#include "MEMeleeAttackTask.h"

#include "../MyEngine_Source/IFSMContext.h"

namespace ME
{
    void MeleeAttackTask::OnEnter(
        FSMBrainCore* brain,
        IFSMContext& context)
    {
        context.BeginMeleeAttack(
            mAnimNames
        );
    }

    void MeleeAttackTask::OnExecute(
        FSMBrainCore* brain,
        IFSMContext& context)
    {
        // 공격 실행은 상태 진입 시 한 번만
    }
}