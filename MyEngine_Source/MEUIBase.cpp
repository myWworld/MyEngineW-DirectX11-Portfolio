#include "MEUIBase.h"
#include "METransform.h"
#include "MEGameObject.h"
#include "MECamera.h"
#include "MERenderer.h"
#include "MEApplication.h"

extern ME::Application application;

namespace ME
{
    UIBase::UIBase(enums::eUIType type)
        : mType(type)
        , mbMouseOn(false)
        , mbFullScreen(false)
        , mbFollowingOwner(false)
        , mbEnabled(false)
        , mOwner(nullptr)
        , mParent(nullptr)
    {
    }
    UIBase::~UIBase()
    {
    }
    void UIBase::Initialize()
    {
        OnInit();
    }
    void UIBase::Render()
    {
        if (mbEnabled)
            OnRender();
    }
    void UIBase::Active()
    {
        mbEnabled = true;
        OnActive();
    }
    void UIBase::InActive()
    {
        mbEnabled = false;
        OnInActive();
    }
    void UIBase::Update()
    {
        if (mbEnabled)
            OnUpdate();
    }
    void UIBase::LateUpdate()
    {

        if (mbEnabled)
            OnLateUpdate();
    }
    void UIBase::UIClear()
    {
        OnClear();
    }
    void UIBase::OnInit()
    {
    }
    void UIBase::OnActive()
    {
    }
    void UIBase::OnInActive()
    {
    }
    void UIBase::OnUpdate()
    {
        if (mbFollowingOwner == true)
        {
            if (mOwner != nullptr)
            {
                Transform* tr = mOwner->GetComponent<Transform >();
                Vector3 pos = tr->GetPosition();
                SetPos(Vector2(pos.x, pos.y));


            }
        }
    }
    void UIBase::OnLateUpdate()
    {
    }
    void UIBase::OnRender()
    {
    }
    void UIBase::OnClear()
    {
    }

    math::Vector2 UIBase::WolrdPosToScreenPos(math::Vector3& worldPos)
    {
        math::Matrix viewMatrix = renderer::mainCamera->GetGpuViewMatrix();
        math::Matrix projectionMatrix = renderer::mainCamera->GetGpuProjectionMatrix();

        math::Vector3 ndcPos = math::Vector3::Transform(worldPos, viewMatrix);
        ndcPos = math::Vector3::Transform(ndcPos, projectionMatrix);

        float screenWidth = application.GetWidth();   // 엔진에 구현된 해상도 
        float screenHeight = application.GetHeight();

        math::Vector2 screenPos;
        screenPos.x = (ndcPos.x + 1.0f) * 0.5f * screenWidth;
        screenPos.y = (1.0f - ndcPos.y) * 0.5f * screenHeight;

        return screenPos;
    }
}
