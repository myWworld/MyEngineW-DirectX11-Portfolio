#include "MECollider.h"
#include "MEScript.h"
#include "MEGameObject.h"

namespace ME
{
    UINT32 Collider::mCollisionID = 1;

    Collider::Collider(enums::eColliderType type)
        :Component(enums::eComponentType::Collider)
        ,mType(type)
        ,mID(mCollisionID++)
        ,mSize(Vector3::One)
        ,mOffset(Vector3::Zero)
		, mWireShader()
    {

    }
    Collider::~Collider()
    {
    }
    void Collider::Initialize()
    {
    }
    void Collider::Update()
    {
    }
    void Collider::LateUpdate()
    {
    }
    void Collider::Render()
    {
    }
    void Collider::OnCollisionEnter(Collider* other)
    {
        Script* script = GetOwner()->GetComponent<Script>();
        if (script != nullptr)
        {
            script->OnCollisionEnter(other);
        }

    }
    void Collider::OnCollisionStay(Collider* other)

    {
        Script* script = GetOwner()->GetComponent<Script>();
        if (script != nullptr) 
        {
            script->OnCollisionStay(other);
        }

    }
    void Collider::OnCollisionExit(Collider* other)
    {
        Script* script = GetOwner()->GetComponent<Script>();
        if (script != nullptr) 
        {
            script->OnCollisionExit(other);
        }

    }
}