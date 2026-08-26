#include "MEPlayAnimTask.h"

#include "../MyEngine_Source/IFSMContext.h"

namespace ME
{
    void PlayAnimTask::OnEnter(
        FSMBrainCore* brain,
        IFSMContext& context)
    {
        context.PlayAnimation(
            mAnimName,
            mbIsLoop
        );
    }

    void PlayAnimTask::OnExecute(
        FSMBrainCore* brain,
        IFSMContext& context)
    {
        // 애니메이션 변경은 상태 진입 시 한 번만 실행
    }
}

