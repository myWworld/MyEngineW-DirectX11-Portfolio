#pragma once

#include "../MyEngine_Source/MEFSMDecision.h"

namespace ME
{
    class AnimFinishDecision final : public FSMDecision
    {
    public:

        AnimFinishDecision()
        {
            // 애니메이션 완료는 매 서버 틱 검사
            SetIntervalTime(0.0f);
        }

    protected:
        bool CheckDecision(
            FSMBrainCore* brain,
            IFSMContext& context) override;
    };
}