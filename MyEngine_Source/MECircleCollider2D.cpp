#include "MECircleCollider2D.h"
#include "MEBoxCollider2D.h"
#include "METransform.h"
#include "MEGameObject.h"
#include "MERenderer.h"
#include "MECamera.h"

namespace ME
{
	CircleCollider2D::CircleCollider2D()
		:Collider(enums::eColliderType::Circle2D)
		,mRadius(0.0f)
	{
	}
	CircleCollider2D::~CircleCollider2D()
	{
	}
	void CircleCollider2D::Initialize()
	{
	}
	void CircleCollider2D::Update()
	{
	}
	void CircleCollider2D::LateUpdate()
	{
	}
	void CircleCollider2D::Render()
	{
		Transform* tr = GetOwner()->GetComponent<Transform>();
		Vector3 pos = tr->GetPosition();

	
	}

	bool  CircleCollider2D::IntersectWith(BoxCollider2D* other)
	{
		Transform* leftTr = this->GetOwner()->GetComponent<Transform>();
		Transform* rightTr = other->GetOwner()->GetComponent<Transform>();

		Vector3 leftPos = leftTr->GetPosition() + this->GetOffset();
		Vector3 rightPos = rightTr->GetPosition() + other->GetOffset();

		Vector3 leftSize = this->GetSize(); //* 100.0f;
		Vector3 rightSize = other->GetSize(); //* 100.0f;


		CircleCollider2D* leftBC = this;
		BoxCollider2D* rightBC = other;


		enums::eColliderType leftType = this->GetColliderType();
		enums::eColliderType rightType = other->GetColliderType();


		if (leftType == enums::eColliderType::Circle2D && rightType == enums::eColliderType::Rect2D)
		{
			//circle - rect
			Vector3 leftCirclePos = leftPos + (leftSize / 2.0f);

			float r = leftSize.x / 2.0f;


			if ((leftCirclePos.x >= rightPos.x - r && leftCirclePos.x <= rightPos.x + r)
				&& (leftCirclePos.y >= rightPos.y - r && leftCirclePos.y <= rightPos.y + r))
			{
				return true;
			}

		}
	}

	bool CircleCollider2D::IntersectWith(CircleCollider2D* other)
	{
		Transform* leftTr = this->GetOwner()->GetComponent<Transform>();
		Transform* rightTr = other->GetOwner()->GetComponent<Transform>();

		Vector3 leftPos = leftTr->GetPosition() + this->GetOffset();
		Vector3 rightPos = rightTr->GetPosition() + other->GetOffset();

		Vector3 leftSize = this->GetSize(); //* 100.0f;
		Vector3 rightSize = other->GetSize(); //* 100.0f;


		CircleCollider2D* leftBC = this;
		CircleCollider2D* rightBC = other;


		enums::eColliderType leftType = this->GetColliderType();
		enums::eColliderType rightType = other->GetColliderType();

		if (leftType == enums::eColliderType::Circle2D
			&& rightType == enums::eColliderType::Circle2D)
		{
			//circle - circle
			Vector3 leftCirclePos = leftPos + (leftSize / 2.0f);
			Vector3 rightCirclePos = rightPos + (rightSize / 2.0f);

			float distance = (leftCirclePos - rightCirclePos).Length();

			if (distance <= (leftSize.x / 2.0f + rightSize.x / 2.0f))
			{
				return true;
			}

		}
	}
}