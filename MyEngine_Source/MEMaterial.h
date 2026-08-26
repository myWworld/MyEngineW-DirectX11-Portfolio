#pragma once
#include "MEResource.h"
#include "METexture.h"
#include "MEShader.h"


namespace ME
{
	class Material:public Resource
	{
	public:

		Material();
		virtual ~Material();

		struct Data 
		{
			std::wstring albedo;
		};

		virtual HRESULT Save(const std::wstring& path) override;
		virtual HRESULT Load(const std::wstring& path) override;

		void Bind();
		void BindShader();
		void BindTextures();

		//void SetShader(graphics::Shader* shader) { mShader = shader; }
		void SetShader(std::shared_ptr<graphics::Shader> shader) { mShader = shader; }

		void SetAlbedoTexture(std::shared_ptr<graphics::Texture> texture) { mAlbedoTexture = texture; }
		void SetNormalTexture(std::shared_ptr<graphics::Texture> texture) { mNormalTexture = texture; }
		void SetSpecularTexture(std::shared_ptr<graphics::Texture> texture) { mSpecularTexture = texture; }
		void SetRoughnessTexture(std::shared_ptr<graphics::Texture> texture) { mRoughnessTexture = texture; }
		void SetMetallicTexture(std::shared_ptr<graphics::Texture> texture) { mMetallicTexture = texture; }
		void SetSpriteTexture(std::shared_ptr<graphics::Texture> texture) { mSpriteTexture = texture; }

	private:
		
		std::shared_ptr<graphics::Texture> mAlbedoTexture;
		std::shared_ptr<graphics::Texture> mNormalTexture;
		std::shared_ptr<graphics::Texture> mSpecularTexture;
		std::shared_ptr<graphics::Texture> mRoughnessTexture;
		std::shared_ptr<graphics::Texture> mMetallicTexture;

		std::shared_ptr<graphics::Texture> mSpriteTexture;//ui로 사용
	
		XMFLOAT3 AmbientColor; //색깔들 여기선 사용 x
		XMFLOAT3 DiffuseColor;
		XMFLOAT3 SpecularColor;

		std::shared_ptr<graphics::Shader> mShader;

		graphics::eRenderingMode mMode;
		Material::Data mData;


	};

}
