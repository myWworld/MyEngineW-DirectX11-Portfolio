#include "MEShader.h"
#include "MEResources.h"
#include "MERenderer.h"


namespace ME::graphics
{
	bool Shader::bWireFrame = false;

	ME::graphics::Shader::Shader()
		:Resource(enums::eResourceType::Shader)
		, mRasterizerState(eRasterizerState::SolidBack) 
		, mBlendState(eBlendState::Opaque)
		, mDepthStencilState(eDepthStencilState::LessEqual)
	{
	}

	ME::graphics::Shader::~Shader()
	{
	}

	HRESULT Shader::Save(const std::wstring& path)
	{
		return E_NOTIMPL;
	}

	HRESULT ME::graphics::Shader::Load(const std::wstring& path)
	{
		size_t fileNameBeginOffset = path.rfind(L"\\") + 1;
		size_t fileNameEndOffset = path.length() - fileNameBeginOffset;

		const std::wstring fileName(path.substr(fileNameBeginOffset, fileNameEndOffset));

		if (!Create(eShaderStage::VS, fileName))
			return S_FALSE;

		if (!Create(eShaderStage::PS, fileName))
			return S_FALSE;

		return S_OK;
	}

	bool Shader::Create(const eShaderStage stage, const std::wstring& fileName)
	{
		if (stage == eShaderStage::VS)
			CreateVertexShader(fileName);

		if (stage == eShaderStage::PS)
			CreatePixelShader(fileName);

		return true;
	}

	bool Shader::CreateVertexShader(const std::wstring& fileName)
	{
		if (!GetDevice()->CreateVertexShader(fileName, mVSBlob.GetAddressOf(), mVS.GetAddressOf()))
			return false;

		return true;
	}

	bool Shader::CreatePixelShader(const std::wstring& fileName)
	{
		if (!GetDevice()->CreatePixelShader(fileName, mPSBlob.GetAddressOf(), mPS.GetAddressOf()))
			return false;

		return true;
	}

	void Shader::Bind()
	{
		if (bWireFrame)
		{
			Shader* wireframeShader = Resources::Find<Shader>(L"WireFrameShader").get();
			Microsoft::WRL::ComPtr<ID3D11VertexShader> wireframeShaderVS = wireframeShader->GetVS();
			Microsoft::WRL::ComPtr<ID3D11PixelShader>wireframeShaderPS = wireframeShader->GetPS();
			Microsoft::WRL::ComPtr<ID3D11RasterizerState> wireframeRasterizerState = renderer::rasterizerStates[static_cast<UINT> (eRasterizerState::WireFrame)];

			GetDevice()->BindVS(wireframeShaderVS.Get());
			GetDevice()->BindPS(wireframeShaderPS.Get());

			GetDevice()->BindRasterizerState(wireframeRasterizerState.Get());
			GetDevice()->BindBlendState(renderer::blendStates[static_cast<UINT>(mBlendState)].Get(), nullptr, 0xffffff);
			GetDevice()->BindDepthStencilState(renderer::depthStencilStates[static_cast<UINT>(mDepthStencilState)].Get(), 0);

			return;

		}

		if (mVS)
			GetDevice()->BindVS(mVS.Get());

		if(mPS)
			GetDevice()->BindPS(mPS.Get());


		UINT idx = static_cast<UINT>(mRasterizerState);
		if (idx < _countof(renderer::rasterizerStates) && renderer::rasterizerStates[idx])
		{
			GetDevice()->BindRasterizerState(renderer::rasterizerStates[idx].Get());
		}
		else
		{
			// 안전 대체 동작 (예: 기본 상태 바인드 또는 무시)
			GetDevice()->BindRasterizerState(nullptr);
		}
		GetDevice()->BindBlendState(renderer::blendStates[static_cast<UINT>(mBlendState)].Get(), nullptr, 0xffffff);
		GetDevice()->BindDepthStencilState(renderer::depthStencilStates[static_cast<UINT>(mDepthStencilState)].Get(), 0);
	}

}