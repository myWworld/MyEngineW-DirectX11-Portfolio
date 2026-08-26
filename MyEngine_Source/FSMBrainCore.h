#pragma once

#include "MEFSMState.h"
#include <memory>
#include <string>
#include <unordered_map>

namespace ME
{
    class BlackBoard;
    class IFSMContext;

    class FSMBrainCore
    {
    public:
        FSMBrainCore();
        ~FSMBrainCore();

        FSMBrainCore(const FSMBrainCore&) = delete;
        FSMBrainCore& operator=(const FSMBrainCore&) = delete;

        void Update(IFSMContext& context);

        void ChangeState(FSMState* nextState);
        void SendFSMEvent(const std::string& eventName);

        void AddState(
            const std::string& name,
            std::unique_ptr<FSMState> state);

        bool SetInitialState(const std::string& name);

        FSMState* FindState(const std::string& name) const;
        FSMState* GetActiveState() const;

        std::string GetActiveStateName() const;
        float GetStateElapsedTime() const;

        BlackBoard* GetBlackboard() const;

        void Clear();

    private:
        // 반드시 유효한 Context를 전달받은 상태에서만 호출 context가 먼저 사라지는 문제발생
        void ApplyStateChange(
            FSMState* nextState,
            IFSMContext& context);

    private:
        std::unordered_map<std::string, std::unique_ptr<FSMState>> mStates;

        FSMState* mActiveState = nullptr;
        // Update() 실행 중에만 유효하다
       // 프레임/서버 Tick 사이에는 반드시 nullptr이어야 한다
        IFSMContext* mCurrentContext = nullptr;

        // Update 밖에서 요청된 상태 전환
        // 다음 Update에서 유효한 Context와 함께 처리한다
        FSMState* mPendingState = nullptr;
        std::unique_ptr<BlackBoard> mBlackboard;

        float mStateElapsedTime = 0.0f;
        bool mbStarted = false;
    };
}