#include "METitleScene.h"
#include "MEInput.h"
#include "MESceneManager.h"
#include "MEObject.h"
#include "MERenderer.h"
#include "MESpriteRenderer.h"
#include "METexture.h"
#include "MEResource.h"
#include "MEResources.h"
#include "METransform.h"
#include "MEApplication.h"

#include "MEGraphicDevice_DX11.h"
#include "MEUIManager.h"
#include "MEPlayer.h"
#include "MEModel.h"

#include "MEUIManager.h"

#include "MECameraScript.h"
#include "MEModelRenderer.h"
#include "MEAnimator3D.h"
#include "MERigidbody.h"
#include "MEBoxCollider3D.h"

#include "MEPlayerScript.h"
#include "MEEnemyScript.h"
#include "MEWeaponScript.h"
#include "MESwordScript.h"
#include "MEGunScript.h"
#include "MEGauntletScript.h"
#include "MEGun.h"
#include "MECollisionManager.h"
#include "../MyEngine_Source/MEFSMBrain.h"
#include "../MyEngine_Source/FSMFactory.h"
#include "../MyEngine_Source/MEUIHUD.h"
#include "../MyEngine_Source/MENetworkManager.h"
#include "../MyEngine_Source/Protocol.h"
#include "MEProjectileVisualManager.h"

extern ME::Application application;

namespace ME
{


	ME::TitleScene::TitleScene()
	{
	}

	ME::TitleScene::~TitleScene()
	{

	}

	void ME::TitleScene::Initialize()
	{

		Scene::Initialize();

		CollisionManager::CollisionLayerCheck(enums::eLayerType::Player, enums::eLayerType::Bullet, true);
		CollisionManager::CollisionLayerCheck(enums::eLayerType::Bullet, enums::eLayerType::Player, true);
		CollisionManager::CollisionLayerCheck(enums::eLayerType::Weapon, enums::eLayerType::Player, true);
		CollisionManager::CollisionLayerCheck(enums::eLayerType::Player, enums::eLayerType::Weapon, true);

	}

	void ME::TitleScene::Update()
	{

		Scene::Update();
	}

	void ME::TitleScene::LateUpdate()
	{
		Scene::LateUpdate();

	}

	void ME::TitleScene::Render()
	{
		Scene::Render();
	}

	void TitleScene::OnEnter()
	{

		
			GameObject* camera = object::Instantiate<GameObject>(enums::eLayerType::None, math::Vector3(0, 0, 0));

			Camera* cameraComp = camera->AddComponent<Camera>();
			cameraComp->SetProjectionMatrix(Camera::eProjectionType::Perspective);
			cameraComp->SetSize(1000.0f);

			CameraScript* cameraScript = camera->AddComponent<CameraScript>();
			renderer::mainCamera = cameraComp;
		
		if (!mPlayer)
		{

			mPlayer = object::Instantiate<Player>(enums::eLayerType::Player, math::Vector3(0, 0, 0));
			Transform* tr = mPlayer->GetComponent<Transform>();
			//tr->SetScale(Vector3(0.2f, 0.2f, 0.2f));
			renderer::mainCamera->SetTarget(mPlayer);


			MakeCharacter(mPlayer, L"CharacterModel");
			mPlayer->AddComponent<PlayerScript>();

			SetLocalPlayer(mPlayer);

			MakeWeapon(mPlayer, L"PistolModel",L"LeftHand", 5.0f);
			MakeWeapon(mPlayer, L"SwordModel", L"LeftHand", 15.0f);
		

			Pkt_C_Enter enterPkt = {};
			enterPkt.header.type = ePacketType::C_ENTER;
			enterPkt.modelType = eModelType::Character;
			enterPkt.weaponType = eWeaponType::Gun; 
			enterPkt.x = tr->GetPosition().x;
			enterPkt.y = tr->GetPosition().y;
			enterPkt.z = tr->GetPosition().z;

			ME::NetworkManager::SendPacket(&enterPkt);


			//GameObject* enemy = object::Instantiate<Player>(enums::eLayerType::Player,math::Vector3(500, 0, 0));
			//Transform* transform = enemy->GetComponent<Transform>();
			//
			//MakeCharacter(enemy, L"MutantModel");
			//EnemyScript* enemyScript = enemy->AddComponent<EnemyScript>();
			//
			//auto* rightWeapon = MakeWeapon(enemy, L"GauntletModel", L"RightHand", 10.0f);
			//rightWeapon->SetSocketOffsetAntRot(math::Vector3(-96.0f, 149.0f, 1.0f), math::Vector3::Zero);
			//
			//auto* leftWeapon = MakeWeapon(enemy, L"GauntletModel", L"LeftHand", 10.0f);
			//
			////enemyScript->SetRightWeapon(rightWeapon);
			//enemyScript->SetLeftWeapon(leftWeapon);
			//leftWeapon->SetSocketOffsetAntRot(math::Vector3(129.0f, 139.0f, -9.0f), math::Vector3::Zero); //129.0f, 139.0f, -9.0f
			//
			//
			//FSMBrainCore* brainCore = new FSMBrainCore();
			//FSMBrain* brain = enemy->AddComponent<FSMBrain>();
			//brain->SetCore(brainCore);
		
			//FSMFactory::MakeFSMWithJsonFile(brainCore, "..\\Resources\\EnemyFSMJson.json");

		}


		ProjectileVisualManager::Initialize(100);

		UIManager::Push(enums::eUIType::Button);

		Scene::OnEnter();

	}

	void TitleScene::OnExit()
	{

		SetLocalPlayer(nullptr);
		if (mPlayer)
		{
			object::Destroy(mPlayer);
			mPlayer = nullptr;
		}


		if (renderer::mainCamera != nullptr)
		{
			GameObject* obj = renderer::mainCamera->GetOwner();
			object::Destroy(obj);
			renderer::mainCamera = nullptr;
		}

		UIManager::Pop(enums::eUIType::Button);

		Scene::OnExit();

	}

	void TitleScene::MakeCharacter(GameObject* player, std::wstring_view modelName)
	{
		std::shared_ptr<Model> model = Resources::Find<Model>(std::wstring(modelName));

		if (model)
		{


			auto* animator = player->AddComponent<Animator3D>();

			auto* modelRenderer = player->AddComponent<ModelRenderer>();
			modelRenderer->SetMesh(model->GetMeshes());
		
			
			player->SetModel(model);

		//	Rigidbody* rb = mPlayer->AddComponent<Rigidbody>();
			Collider* col = player->AddComponent<BoxCollider3D>();
			col->SetSize(math::Vector3(50, 250, 50));


			//UIManager::Push(enums::eUIType::HpBar); //esc 메뉴, 환경 설정등 씬에서 유일하게 렌더할 것들 용도

			animator->SetSkeleton(&model->GetSkeleton());
	
			animator->PlayAnimation(L"PISTOLIDLE", true);
		}
		else
		{
			model = nullptr;
		}
	}

	WeaponScript* TitleScene::MakeWeapon(GameObject* player, std::wstring_view modelName, std::wstring socketName, float damage)
	{
		
		GameObject* obj = object::Instantiate<GameObject>(enums::eLayerType::Weapon, math::Vector3(0, 10, 35));
		Transform* tr = obj->GetComponent<Transform>();

		WeaponScript* weaponScript = nullptr;

		if (modelName == L"PistolModel")
		{
			weaponScript = obj->AddComponent<GunScript>();
		}
		else if (modelName == L"SwordModel")
		{
			weaponScript = obj->AddComponent<SwordScript>();
			tr->SetScale(math::Vector3(10.f, 10.f, 10.f));
			Collider* col = obj->AddComponent<BoxCollider3D>();
			col->SetOffset(math::Vector3(9.0f, 49.0f, 3.0f));
			col->SetSize(math::Vector3(10.f, 70.f, 10.f));


		}
		else if (modelName == L"GauntletModel")
		{
			weaponScript = obj->AddComponent<GauntletScript>();
			Collider* col = obj->AddComponent<BoxCollider3D>();
			col->SetOffset(math::Vector3(0,0,0));
			col->SetSize(math::Vector3(40.f, 40.f, 40.f));
		}

		weaponScript->SetOnwer(player);
		weaponScript->SetDamage(damage);
		weaponScript->SetSocketBoneName(socketName);
	
		std::shared_ptr<Model> model = Resources::Find<Model>(std::wstring(modelName));

		if (model)
		{

			ModelRenderer* modelRenderer = obj->AddComponent<ModelRenderer>();
			modelRenderer->SetMesh(model->GetMeshes());
			player->GetComponent<ActorScript>()->AddWeapon(weaponScript);

			if (modelName == L"SwordModel")
			{
				Material* material = model->GetMeshes()[0]->GetMaterial().get();
				material->SetAlbedoTexture(Resources::Find<graphics::Texture>(L"SWORD_DIFFUSE"));
				//material->SetNormalTexture(Resources::Find<graphics::Texture>(L"SWORD_NORMAL"));
			}
		}
		else
		{
	
			model = nullptr;
		}

		return weaponScript;
	}
}
		


