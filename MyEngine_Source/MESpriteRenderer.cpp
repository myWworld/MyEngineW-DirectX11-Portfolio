#include "MESpriteRenderer.h"
#include "MEGameObject.h"
#include "METransform.h"
#include "CommonInclude.h"
#include "METexture.h"
#include "MERenderer.h"
#include "MECamera.h"

#include "METransform.h"

#include "MEResources.h"


namespace ME {

	ME::SpriteRenderer::SpriteRenderer() 
		:mMesh(nullptr)
		,mTextures(nullptr)
		,mMaterial(nullptr)
		,Component(enums::eComponentType::SpriteRenderer)
	{
	}

	ME::SpriteRenderer::~SpriteRenderer()
	{
	}

	void ME::SpriteRenderer::Initialize()
	{
		mMesh = Resources::Find<Mesh>(L"RectMesh");
	}

	void ME::SpriteRenderer::Update()
	{
	}

	void ME::SpriteRenderer::LateUpdate()
	{
	}

	void ME::SpriteRenderer::Render()
	{
		Transform* tr = GetOwner()->GetComponent<Transform>();

		if (tr)
			tr->Bind();

		if (mMesh)
			mMesh->Bind();

		if (mMaterial)
			mMaterial->BindShader();

		if (mTextures)
			mTextures->Bind(graphics::eShaderStage::PS, (UINT)graphics::eTextureType::Sprite);

		if (mMesh)
			graphics::GetDevice()->DrawIndexed(mMesh->GetIndexCount(), 0, 0);
	
	}

	
}