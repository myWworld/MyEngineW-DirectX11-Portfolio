#pragma once

#include "FSMBrainCore.h"
#include "MEComponent.h"

#include <memory>
#include <string>

namespace ME
{
    class BlackBoard;
    class ClientFSMContext;
    class FSMState;

    class FSMBrain : public Component
    {
    public:
        FSMBrain();
        ~FSMBrain();

        void Initialize() override;
        void Update() override;
        void LateUpdate() override;
        void Render() override;

        FSMBrainCore* GetCore();
        const FSMBrainCore* GetCore() const;
        
        void SetCore(std::unique_ptr<FSMBrainCore> core)
        {
            mCore = std::move(core);
        }


        BlackBoard* GetBlackboard();

        FSMState* GetActiveState() const;

        void ChangeState(FSMState* nextState);
        void SendFSMEvent(const std::string& eventName);

        void AddState(const std::string& name, std::unique_ptr<FSMState> state);

        bool SetInitialState(const std::string& name);

    private:
        std::unique_ptr<FSMBrainCore> mCore;

        std::unique_ptr<ClientFSMContext> mContext;
    };
}