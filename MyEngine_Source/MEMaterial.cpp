#include "MEMaterial.h"


namespace ME
{
	Material::Material()
		:Resource(enums::eResourceType::Material)
		, mMode(graphics::eRenderingMode::Opaque)
	{
	}
	Material::~Material()
	{
	}
	HRESULT Material::Save(const std::wstring& path)
	{
		return E_NOTIMPL;
	}
	HRESULT Material::Load(const std::wstring& path)
	{
		return E_NOTIMPL;
	}
	void Material::Bind()
	{
		BindShader();
		BindTextures();

	}
	void Material::BindShader()
	{
		if (mShader)
			mShader->Bind();
	}
	void Material::BindTextures()
	{
		if (mAlbedoTexture)
			mAlbedoTexture->Bind(graphics::eShaderStage::PS, (UINT)graphics::eTextureType::Albedo);

		if(mNormalTexture)
			mNormalTexture->Bind(graphics::eShaderStage::PS, (UINT)graphics::eTextureType::Normal);

		if (mSpecularTexture)
			mSpecularTexture->Bind(graphics::eShaderStage::PS, (UINT)graphics::eTextureType::Specular);

		if(mRoughnessTexture)
			mRoughnessTexture->Bind(graphics::eShaderStage::PS, (UINT)graphics::eTextureType::Smoothness);

		if(mMetallicTexture)
			mMetallicTexture->Bind(graphics::eShaderStage::PS, (UINT)graphics::eTextureType::Metalic);

		if(mSpriteTexture)
			mSpriteTexture->Bind(graphics::eShaderStage::PS, (UINT)graphics::eTextureType::Sprite);
	}
}