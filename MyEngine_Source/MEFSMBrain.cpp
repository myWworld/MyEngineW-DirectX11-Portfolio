#include "MEFSMBrain.h"

#include "MEClientFSMContext.h"
#include "MENetworkManager.h"
#include "METime.h"

namespace ME
{
    FSMBrain::FSMBrain()
        : Component(enums::eComponentType::FSMBrain)
    {
    }

    FSMBrain::~FSMBrain() = default;

    void FSMBrain::Initialize()
    {
        mContext = std::make_unique<ClientFSMContext>(GetOwner());
    }

    void FSMBrain::Update()
    {
        if (!mContext)
            return;

        // 온라인 원격 몬스터는 FSMBrain 자체를 붙이지 않는다.
        // 기존 Host/오프라인 몬스터만 이 Wrapper를 실행한다.
      /*  if (!NetworkManager::IsConnected())
            return;*/

        mContext->SetDeltaTime(Time::DeltaTime());
        if(mCore)
            mCore->Update(*mContext);
    }

    void FSMBrain::LateUpdate()
    {
    }

    void FSMBrain::Render()
    {
    }

    FSMBrainCore* FSMBrain::GetCore()
    {
        return mCore.get();
    }

    const FSMBrainCore* FSMBrain::GetCore() const
    {
        return mCore.get();
    }

    BlackBoard* FSMBrain::GetBlackboard()
    {
        return mCore->GetBlackboard();
    }

    FSMState* FSMBrain::GetActiveState() const
    {
        return mCore->GetActiveState();
    }

    void FSMBrain::ChangeState(FSMState* nextState)
    {
        mCore->ChangeState(nextState);
    }

    void FSMBrain::SendFSMEvent(const std::string& eventName)
    {
        mCore->SendFSMEvent(eventName);
    }

    void FSMBrain::AddState(const std::string& name, std::unique_ptr<FSMState> state)
    {
        mCore->AddState(name, std::move(state));
    }

    bool FSMBrain::SetInitialState(const std::string& name)
    {
        return mCore->SetInitialState(name);
    }
}