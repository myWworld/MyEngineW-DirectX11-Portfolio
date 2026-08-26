#include "METexture.h"
#include "MEApplication.h"
#include "MEResources.h"

extern ME::Application application;

namespace ME::graphics
{
	

	Texture::Texture()
		:Resource(enums::eResourceType::Texture)
		,mDesc{}
	{

	}

	Texture::~Texture()
	{
	}

	HRESULT Texture::Save(const std::wstring& path)
	{
		return S_OK;
	 }

	HRESULT Texture::Load(const std::wstring& path)
	{
		
		std::wstring ext 
			= path.substr(path.find_last_of(L".") + 1);



		if (ext == L".dds" || ext == L".DDS")
		{
			if (FAILED(LoadFromDDSFile(path.c_str(), DDS_FLAGS::DDS_FLAGS_NONE, nullptr, mImage)))
				return S_FALSE;
		}
		else if (ext == L".tga" || ext == L".TGA")
		{
			if (FAILED(LoadFromTGAFile(path.c_str(), nullptr, mImage)))
				return S_FALSE;
		}
		else // WIC (png, jpg, jpeg, bmp )
		{
			if (FAILED(LoadFromWICFile(path.c_str(), WIC_FLAGS::WIC_FLAGS_NONE, nullptr, mImage)))
				return S_FALSE;
		}

		HRESULT hr = CreateShaderResourceView
		(
			graphics::GetDevice()->GetID3D11Device().Get()
			, mImage.GetImages()
			, mImage.GetImageCount()
			, mImage.GetMetadata()
			, mSRV.GetAddressOf()
		);

		if (hr == S_FALSE)
			assert(false);

		mSRV->GetResource((ID3D11Resource**)mTexture.GetAddressOf());


		return S_OK;
	}



	void Texture::Bind(eShaderStage stage, UINT startSlot)
	{
		graphics::GetDevice()->SetShaderResource(stage, startSlot, mSRV.GetAddressOf());
	}

  //
	//COLORREF Texture::GetPixel(int x, int y)
	//{
	//	return ::GetPixel(mHdc, x, y);
	//}
  //
}