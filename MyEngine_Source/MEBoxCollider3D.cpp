#include "MEBoxCollider3D.h"
#include "METransform.h"
#include "MEGameObject.h"
#include "MERenderer.h"
#include "MECamera.h"
#include "MEResources.h"
#include "MEShader.h"
#include "MEMesh.h"

namespace ME
{
	BoxCollider3D::BoxCollider3D()
		:Collider(enums::eColliderType::Box3D)
		, mRot(0)
		, mbIsRotate(false)
	{

	}
	BoxCollider3D::~BoxCollider3D()
	{
	}
	void BoxCollider3D::Initialize()
	{
		mWireShader = Resources::Find<graphics::Shader>(L"WireFrameShader");
	}
	void BoxCollider3D::Update()
	{
		



	}
	void BoxCollider3D::LateUpdate()
	{
	}
	void BoxCollider3D::Render()
	{
		Transform* tr = GetOwner()->GetComponent<Transform>();
		Vector3 pos = tr->GetPosition();
		math::Quaternion rot = tr->GetRotationQuat();

		if (std::shared_ptr<graphics::Shader> shader = mWireShader.lock())
		{
			shader->Bind();

		//graphics::GetDevice()->BindRasterizerState(renderer::rasterizerStates[(UINT)eRasterizerState::WireFrame].Get());
			
			Transform debugTransform; //현재 구조는 최상위 부모에대한 트랜스폼만 있어서 그거를 콜라이더 위치로 맞춰버리면 부모 위치까지 왜곡되므로 임시 객체생성

			// ==========================================
		// 오프셋 벡터를 부모의 회전 방향에 맞게 회전 -> 기존은 회전 반영을 안해서 정해진 축에 대한 오프셋을 그대로 사용해 버렸기 때문에
			// 어디를 가든 같은 자리에 고정이 됐던점을 개선하기위해 부모의 쿼터니온을 가져와 오프셋에 적용하도록 변경
		
			Vector3 rotatedOffset = math::Vector3::Transform(mOffset, rot);

			// 부모 위치 + 회전된 오프셋
			debugTransform.SetPosition(pos + rotatedOffset);
			debugTransform.SetScale(mSize);

			// 박스 자체도 부모의 회전과 똑같이 (OBB)
			debugTransform.SetRotation(rot);

			debugTransform.LateUpdate();
			debugTransform.Bind();

			// 메쉬 렌더링
		/*	std::shared_ptr<Mesh> cubeMesh = ME::Resources::Find<Mesh>(L"CubeMesh");
			if (cubeMesh)
			{
				cubeMesh->Bind();
				graphics::GetDevice()->BindPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
				graphics::GetDevice()->DrawIndexed(cubeMesh->GetIndexCount(), 0, 0);
				graphics::GetDevice()->BindPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			}*/


			// 상태 복구

		//	graphics::GetDevice()->BindRasterizerState(renderer::rasterizerStates[(UINT)eRasterizerState::SolidBack].Get());
		}


	}

	Vector3 BoxCollider3D::RotateCollider(float rot, int x, int y, int width, int height)//, HDC hdc)
	{

		//float radian = rot * (3.14159265f / 180.0f);

		//POINT points[4];

		//points[0].x = x;
		//points[0].y = y;

		//// 오른쪽 위 꼭짓점
		//points[1].x = x + (int)(width * cos(radian));
		//points[1].y = y + (int)(width * sin(radian));

		//// 오른쪽 아래 꼭짓점
		//points[2].x = x + (int)(width * cos(radian) - height * sin(radian));
		//points[2].y = y + (int)(width * sin(radian) + height * cos(radian));

		//// 왼쪽 아래 꼭짓점
		//points[3].x = x - (int)(height * sin(radian));
		//points[3].y = y + (int)(height * cos(radian));

		//// 회전된 사각형 그리기
		////Polygon(hdc, points, 4);


		//float midx = (points[0].x + points[1].x + points[2].x + points[3].x) / 4;
		//float midy = (points[0].y + points[1].y + points[2].y + points[3].y) / 4;

		//mWidth = max(fabs(points[1].x - points[2].x), fabs(points[0].x - points[3].x));

		//mHeight = max(fabs(points[1].y - points[2].y), fabs(points[0].y - points[3].y));


		//mPoint = points;

		Vector3 centralPoint = Vector3(0.0f, 0.0f, 0.0f);

		return centralPoint;
	}

	bool BoxCollider3D::IntersectWith(BoxCollider3D* other)
	{
		if (this->GetColliderType() != enums::eColliderType::Box3D ||
			other->GetColliderType() != enums::eColliderType::Box3D)
		{
			return false;
		}


		Transform* leftTr = this->GetOwner()->GetComponent<Transform>();
		Transform* rightTr = other->GetOwner()->GetComponent<Transform>();

		Quaternion leftRot = leftTr->GetRotationQuat();
		Quaternion rightRot = rightTr->GetRotationQuat();

		Vector3 leftOffsetRotated = Vector3::Transform(this->GetOffset(), leftRot);
		Vector3 rightOffsetRotated = Vector3::Transform(other->GetOffset(), rightRot);

		Vector3 leftCenter = leftTr->GetPosition() + leftOffsetRotated;
		Vector3 rightCenter = rightTr->GetPosition() + rightOffsetRotated;

		Vector3 leftScale = leftTr->GetScale();
		Vector3 rightScale = rightTr->GetScale();

		Vector3 leftExtents = this->GetSize() / 2.0f;
		Vector3 rightExtents = other->GetSize()  / 2.0f;


			//AABB 충돌 rect-rect
		/*	if (fabs(leftCenterPos.x - rightCenterPos.x) < fabs(leftSize.x / 2.0f + rightSize.x / 2.0f)
				&& fabs(leftCenterPos.y - rightCenterPos.y) < fabs(leftSize.y / 2.0f + rightSize.y / 2.0f)
				&& fabs(leftCenterPos.z - rightCenterPos.z) < fabs(leftSize.z / 2.0f + rightSize.z / 2.0f))
			{
				return true;
			}*/
		
		return CheckOBBCollision(leftCenter, leftExtents, leftRot, rightCenter, rightExtents, rightRot);
	}

	bool BoxCollider3D::CheckOBBCollision(
		const Vector3& centerA, const Vector3& extentsA, const Quaternion& rotA,
		const Vector3& centerB, const Vector3& extentsB, const Quaternion& rotB)
	{
		math::Matrix matA = math::Matrix::CreateFromQuaternion(rotA);
		math::Matrix matB = math::Matrix::CreateFromQuaternion(rotB); //회전 행렬 생성

		math::Vector3 A[3] = { matA.Right(), matA.Up(), matA.Forward() };  //검사를 진행할 A의 축 -> 면은 서로 평행이므로 A의 축 3개와 B의 축 3개를 외적하여 나온 9개의 축을 검사하면 된다 
		math::Vector3 B[3] = { matB.Right(), matB.Up(), matB.Forward() }; //검사를 진행할 B의 축

		Vector3 distBetweenCenters = centerB - centerA; //두 OBB의 중심 사이의 거리

		auto SATTest = [&](const math::Vector3 axis) -> bool
			{
				if (axis.LengthSquared() < 1e-6f) return true; //축이 거의 0이면 충돌하지 않음

				Vector3 nAxis = axis;
				nAxis.Normalize(); //내적을 위해 정규화 -> SAT에서 기준이 되는 축이고 이 축을 기준으로 두 충돌체 A,B의 로컬 축을 투영하여 충돌 여부를 판단하기 때문에 정규화 필요

				// A 박스를 축에 투영한 반지름
				float rA = extentsA.x * fabs(A[0].Dot(nAxis)) + //-> 검사 기준인 축 위에 각 박스 길이의 반을 투영하고 더해서 중점까지거리(반지름)을 만들어낸다
					extentsA.y * fabs(A[1].Dot(nAxis)) +
					extentsA.z * fabs(A[2].Dot(nAxis));

				// B 박스를 축에 투영한 반지름
				float rB = extentsB.x * fabs(B[0].Dot(nAxis)) +
					extentsB.y * fabs(B[1].Dot(nAxis)) +
					extentsB.z * fabs(B[2].Dot(nAxis));
				
				return fabs(distBetweenCenters.Dot(nAxis)) <= (rA + rB); //투영된 거리와 반지름 합을 비교하여 충돌 여부 판단
			
			};

		for (int i = 0; i < 3; i++)//검사시 하나라도 충돌이 발생하지 않으면 false를 반환하고 종료
		{
			if (SATTest(A[i]) == false) return false; //A의 축 3개 검사 
			if (SATTest(B[i]) == false) return false; //B의 축 3개 검사
		}

		for (int i = 0; i < 3; i++)
		{
			for (int j = 0; j < 3; j++)
			{
				if (SATTest(A[i].Cross(B[j])) == false) return false; //A의 축과 B의 축을 외적하여 나온 9개의 축 검사
			}
		}

		return true; //모든 축에서 충돌이 발생했으므로 true 반환
	}
}