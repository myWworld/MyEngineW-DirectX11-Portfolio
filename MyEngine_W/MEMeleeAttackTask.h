#pragma once

#include "../MyEngine_Source/MEFSMTask.h"

#include <string>
#include <vector>

namespace ME
{
    class MeleeAttackTask final : public FSMTask
    {
    public:
        MeleeAttackTask()
            : mAnimNames()
            , mbIsLoop(false)
        {
            BIND_PROP(mAnimNames);
            BIND_PROP(mbIsLoop);
        }

    protected:
        void OnEnter(
            FSMBrainCore* brain,
            IFSMContext& context) override;

        void OnExecute(
            FSMBrainCore* brain,
            IFSMContext& context) override;

    private:
        std::vector<std::string> mAnimNames;
        bool mbIsLoop;
    };
}