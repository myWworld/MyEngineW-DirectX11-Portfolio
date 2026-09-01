#include "MEFSMState.h"

#include "FSMBrainCore.h"
#include "IFSMContext.h"
#include "MEFSMDecision.h"
#include "MEFSMTask.h"

#include <utility>

namespace ME
{
    FSMTransition::FSMTransition(
        std::unique_ptr<FSMDecision> decision,
        FSMState* trueState,
        FSMState* falseState)
        : Decision(std::move(decision))
        , TrueState(trueState)
        , FalseState(falseState)
    {
    }

    FSMTransition::FSMTransition(FSMTransition&& other) noexcept
        : Decision(std::move(other.Decision))
        , TrueState(other.TrueState)
        , FalseState(other.FalseState)
    {
    }

    FSMTransition& FSMTransition::operator=(FSMTransition&& other) noexcept
    {
        if (this == &other)
            return *this;

        Decision = std::move(other.Decision);
        TrueState = other.TrueState;
        FalseState = other.FalseState;

        other.Decision = nullptr;

        return *this;
    }

    FSMState::FSMState() = default;
    FSMState::~FSMState() = default;

    void FSMState::EnterState(FSMBrainCore* brain, IFSMContext& context)
    {
        for (auto& transition : mTransitions)
        {
            if (transition.Decision)
            {
                transition.Decision->ResetRuntime();
            }
        }

        for (const auto& task : mTasks)
        {
            task->Enter(brain, context);
        }
    }

    void FSMState::ExitState(FSMBrainCore* brain, IFSMContext& context)
    {
        for (const auto& task : mTasks)
        {
            task->Exit(brain, context);
        }
    }

    void FSMState::UpdateTask(FSMBrainCore* brain, IFSMContext& context)
    {
        for (const auto& task : mTasks)
        {
            task->Execute(brain, context);
        }
    }

    void FSMState::CheckDecision(FSMBrainCore* brain, IFSMContext& context)
    {
        for (const auto& transition : mTransitions)
        {
            if (!transition.Decision)
                continue;

            const eDecisionResult result =
                transition.Decision->Decision(brain, context);

            if (result == eDecisionResult::True &&
                transition.TrueState != nullptr)
            {
                brain->ChangeState(transition.TrueState);

                break;
            }

            if (result == eDecisionResult::False &&
                transition.FalseState != nullptr)
            {
                brain->ChangeState(transition.FalseState);

                break;
            }
        }
    }

    void FSMState::AddTransition(
        std::unique_ptr<FSMDecision> decision,
        FSMState* trueState,
        FSMState* falseState)
    {
        if (!decision)
            return;

        mTransitions.emplace_back(std::move(decision), trueState, falseState);
    }

    void FSMState::AddTask(std::unique_ptr<FSMTask> task)
    {
        if (task)
        {
            mTasks.push_back(std::move(task));
        }
    }

    void FSMState::SetStateName(const std::string& name)
    {
        mStateName = name;
    }

    const std::string& FSMState::GetStateName() const
    {
        return mStateName;
    }
}