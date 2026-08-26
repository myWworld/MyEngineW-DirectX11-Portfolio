#pragma once

#include <memory>
#include <string>
#include <vector>

namespace ME
{
    class FSMBrainCore;
    class IFSMContext;
    class FSMDecision;
    class FSMTask;

    class FSMState;

    struct FSMTransition
    {
        std::unique_ptr<FSMDecision> Decision;

        FSMState* TrueState = nullptr;
        FSMState* FalseState = nullptr;

        FSMTransition(
            std::unique_ptr<FSMDecision> decision,
            FSMState* trueState,
            FSMState* falseState = nullptr);

        FSMTransition(
            FSMTransition&& other) noexcept;

        FSMTransition& operator=(
            FSMTransition&& other) noexcept;

        FSMTransition(const FSMTransition&) = delete;
        FSMTransition& operator=(const FSMTransition&) = delete;
    };

    class FSMState
    {
    public:
        FSMState();
        ~FSMState();

        void EnterState(
            FSMBrainCore* brain,
            IFSMContext& context);

        void ExitState(
            FSMBrainCore* brain,
            IFSMContext& context);

        void UpdateTask(
            FSMBrainCore* brain,
            IFSMContext& context);

        void CheckDecision(
            FSMBrainCore* brain,
            IFSMContext& context);

        void AddTransition(
            std::unique_ptr<FSMDecision> decision,
            FSMState* trueState,
            FSMState* falseState = nullptr);

        void AddTask(
            std::unique_ptr<FSMTask> task);

        void SetStateName(
            const std::string& name);

        const std::string& GetStateName() const;

    private:
        std::vector<FSMTransition> mTransitions;
        std::vector<std::unique_ptr<FSMTask>> mTasks;

        std::string mStateName;
    };
}