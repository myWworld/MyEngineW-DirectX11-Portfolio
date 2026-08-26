#include "MEDestroyTask.h"

#include "../MyEngine_Source/IFSMContext.h"

namespace ME
{
    void DestroyTask::OnEnter(FSMBrainCore* brain, IFSMContext& context)
    {
        mElapsedTime = 0.0f;
        mbDestroyRequested = false;
    }

    void DestroyTask::OnExecute(FSMBrainCore* brain, IFSMContext& context)
    {
        if (mbDestroyRequested)
            return;

        const float deltaTime = context.GetDeltaTime();

        if (deltaTime > 0.0f)
        {
            mElapsedTime += deltaTime;
        }

        if (mElapsedTime < mDeathDelay)
        {
            return;
        }

        mbDestroyRequested = true;

        // 서버 Context에서는 즉시 map을 erase하지 않고
        // monster.destroyRequested = true만 설정하도록 한다
        context.DestroyOwner();

        // 더 이상 Task를 실행하지 않도록 완료 처리
        bIsFinished = true;
    }
}