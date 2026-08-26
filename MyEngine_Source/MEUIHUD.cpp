#include "MEUIHUD.h"
#include "MEResources.h"
#include "MEMaterial.h"
#include "MEMesh.h"
#include "../MyEngine_W/MEActorScript.h"
#include "MERenderer.h"
#include "MECamera.h"
#include "METransform.h"

namespace ME
{
	using namespace Gdiplus;

	ME::UIHUD::UIHUD()
		:UIBase(enums::eUIType::HpBar)
		, mTexture(nullptr)
	{
	}

	ME::UIHUD::~UIHUD()
	{
	}

	void ME::UIHUD::OnInit()
	{
		SetPos(Vector2(10, 10));
		SetSize(Vector2(3.f, 3.0f));


		mTexture = Resources::Find<graphics::Texture>(L"HPBAR").get();
	}

	void ME::UIHUD::OnActive()
	{
	}

	void ME::UIHUD::OnInActive()
	{
	}

	void ME::UIHUD::OnUpdate()
	{
	}

	void ME::UIHUD::OnLateUpdate()
	{
	}

	void ME::UIHUD::OnRender()
	{

		GameObject* owner = GetOwner();
		if (owner == nullptr) return;


		std::shared_ptr<Material> material = Resources::Find<Material>(L"SpriteMaterial");
		std::shared_ptr<Mesh> mesh = Resources::Find<Mesh>(L"RectMesh");


		if (material == nullptr || mesh == nullptr) return;

		ActorScript* actor = owner->GetComponent<ActorScript>();

		float hpRatio = 1.0f;
		if (actor != nullptr)
		{
			hpRatio = actor->GetHP() / actor->GetMaxHp();
			if (hpRatio <= 0.0f) return; //사망시 그리지 않음
		}

		Transform* ownerTr = owner->GetComponent<Transform>();
		Vector3 worldPos = ownerTr->GetPosition();
		worldPos.y += 100.0f;


		Transform* camTr = renderer::mainCamera->GetOwner()->GetComponent<Transform>();
		Vector3 camPos = camTr->GetPosition();
		Vector3 camForward = camTr->Forward();

		Vector3 lookDir = camPos - worldPos;
		lookDir.y = 0.0f; //체력바는 항상 카메라의 위치를 봄 이때 y값은 0으로 둬서 잘못된 왜곡이 없게 해야함

		if (lookDir.LengthSquared() < 0.000001f)
		{
			return;
		}

		lookDir.Normalize();

		Vector3 forward = Vector3::Forward; // ui는 임시 Transform을 쓰므로 월드 +Z축을 처음에 항상 바라보게 한 후에 -> 카메라 방향으로 회전 / 만약 회전 정보가 계속 저장됐다면 로컬 축 forward를 월드좌표로 변환 필요
		math::Quaternion rotQuat;

		float dot = forward.Dot(lookDir);
		dot = std::clamp(dot, -1.0f, 1.0f);

		if (dot > 0.999f)
		{
			// forward와 lookDir이 거의 같은 방향
			// 회전할 필요 없음
			rotQuat = math::Quaternion::Identity;
		}
		else if (dot < -0.999f)
		{
			// forward와 lookDir이 거의 반대 방향
			// 회전축을 임의로 Up으로 잡고 180도 회전
			rotQuat = math::Quaternion::CreateFromAxisAngle(Vector3::Up, 3.141592f);
		}
		else
		{
			Vector3 axis = forward.Cross(lookDir);

			if (axis.LengthSquared() < 0.000001f)
			{
				rotQuat = math::Quaternion::Identity;
			}
			else
			{
				axis.Normalize();
				float angle = acos(dot); //외적으로 ui forward와 카메라 방향으로 회전시키기 위한 축을 구함
				rotQuat = math::Quaternion::CreateFromAxisAngle(axis, angle);
			}
		}

		//math::Quaternion rotQuat = camTr->GetRotationQuat();

		// rotQuat = rotQuat * math::Quaternion::CreateFromAxisAngle(Vector3::Up, 3.141592f);

		Vector3 scale = Vector3(30.0f, 8.0f, 1.0f);

		Transform uiTransform;
		uiTransform.SetPosition(worldPos);
		uiTransform.SetRotation(rotQuat);
		uiTransform.SetScale(scale);

		uiTransform.LateUpdate();
		uiTransform.Bind();


		graphics::ConstantBuffer* cb = renderer::constantBuffers[CBSLOT_UI];
		UICB uiData = {};


		std::shared_ptr<graphics::Texture> hpFillTex = Resources::Find<graphics::Texture>(L"HPFILLTEX");
		if (hpFillTex)
		{
			uiData.hpRatio = hpRatio; // 체력 비율만큼만 그림
			cb->SetData(&uiData);
			cb->Bind(graphics::eShaderStage::PS);

			material->SetSpriteTexture(hpFillTex);
			material->Bind();
			mesh->Bind();

			graphics::GetDevice()->DrawIndexed(mesh->GetIndexCount(), 0, 0);
		}


		std::shared_ptr<graphics::Texture> hpFrameTex = Resources::Find<graphics::Texture>(L"HPFRAMETEX");
		if (hpFrameTex)
		{
			//Vector3 framePos = worldPos - (camForward * 0.1f);
			//uiTransform.SetPosition(framePos);
			//uiTransform.LateUpdate(); // 위치가 변했으니 Matrix 다시 계산
			//uiTransform.Bind();

			uiData.hpRatio = 1.0f;
			cb->SetData(&uiData);
			cb->Bind(graphics::eShaderStage::PS);

			material->SetSpriteTexture(hpFrameTex);
			material->Bind();


			graphics::GetDevice()->DrawIndexed(mesh->GetIndexCount(), 0, 0);
		}

	}

	void ME::UIHUD::OnClear()
	{
	}

}
