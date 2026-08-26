#include "MEGraphicDevice_DX11.h"
#include "MEApplication.h"
#include "MERenderer.h"
#include "METexture.h"	

#include "MEResources.h"
#include "MEShader.h"

#include "MEMesh.h"
#include "MEMaterial.h"

extern ME::Application application;

namespace ME::graphics
{
	GraphicDevice_DX11::GraphicDevice_DX11()
	{
		ME::graphics::GetDevice() = this;

		if (!CreateDevice())
			assert(NULL && "Create Device Failed!");
	}
	GraphicDevice_DX11::~GraphicDevice_DX11()
	{

	}

	bool GraphicDevice_DX11::CreateDevice()
	{
		D3D_FEATURE_LEVEL featureLevels[] = { D3D_FEATURE_LEVEL_11_0 };
		UINT creationFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
#if defined(DEBUG) || defined(_DEBUG)
		creationFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif
		 if(FAILED(D3D11CreateDevice(0, D3D_DRIVER_TYPE_HARDWARE,
			0, creationFlags,
			featureLevels, ARRAYSIZE(featureLevels),
			D3D11_SDK_VERSION, mDevice.GetAddressOf(),
			0, mContext.GetAddressOf())))
			 return false;

		return true;
	}

	bool GraphicDevice_DX11::CreateSwapChain(DXGI_SWAP_CHAIN_DESC desc)
	{
		Microsoft::WRL::ComPtr<IDXGIDevice>     pDXGIDevice = nullptr;
		Microsoft::WRL::ComPtr<IDXGIAdapter>    pAdapter = nullptr;
		Microsoft::WRL::ComPtr<IDXGIFactory>    pFactory = nullptr;

		if (FAILED(mDevice->QueryInterface(__uuidof(IDXGIDevice), (void**)pDXGIDevice.GetAddressOf())))
			return false;

		if (FAILED(pDXGIDevice->GetParent(__uuidof(IDXGIAdapter), (void**)pAdapter.GetAddressOf())))
			return false;

		if (FAILED(pAdapter->GetParent(__uuidof(IDXGIFactory), (void**)pFactory.GetAddressOf())))
			return false;

		if (FAILED(pFactory->CreateSwapChain(mDevice.Get(), &desc, mSwapChain.GetAddressOf())))
			return false;

		return true;
	}

	bool GraphicDevice_DX11::GetBuffer(UINT buffer, REFIID riid, void** ppSurface)//백버퍼 텍스쳐를 가져옴
	{
		if (FAILED(mSwapChain->GetBuffer(buffer, riid, ppSurface)))
			return false;

		return true;
	}

	bool GraphicDevice_DX11::CreateRenderTargetView(ID3D11Resource* pResource, const D3D11_RENDER_TARGET_VIEW_DESC* pDesc, ID3D11RenderTargetView** ppRTView)
	{
		if (FAILED(mDevice->CreateRenderTargetView(pResource, pDesc, ppRTView)))
			return false;

		return true;
	}

	bool GraphicDevice_DX11::CreateDepthStencilView(ID3D11Resource* pResource, const D3D11_DEPTH_STENCIL_VIEW_DESC* pDesc, ID3D11DepthStencilView** ppDepthStencilView)
	{
		if (FAILED(mDevice->CreateDepthStencilView(pResource, pDesc, ppDepthStencilView)))
			return false;

		return true;
	}

	bool GraphicDevice_DX11::CreateTexture2D(const D3D11_TEXTURE2D_DESC* pDesc, const D3D11_SUBRESOURCE_DATA* pInitialData, ID3D11Texture2D** ppTexture2D)
	{
		if (FAILED(mDevice->CreateTexture2D(pDesc, pInitialData, ppTexture2D)))
			return false;

		return true;
	}

	bool GraphicDevice_DX11::CreateSamplerState(const D3D11_SAMPLER_DESC* pSamplerDesc, ID3D11SamplerState** ppSamplerState)
	{
		if (FAILED(mDevice->CreateSamplerState(pSamplerDesc, ppSamplerState)))
			return false;
		
		return true;
	}


	bool GraphicDevice_DX11::CreateVertexShader(const std::wstring& fileName, ID3DBlob** ppCode, ID3D11VertexShader** ppVertexShader)
	{
		DWORD shaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
		shaderFlags |= D3DCOMPILE_DEBUG;
		shaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
		//vertex shader
		
			ID3DBlob* errorBlob = nullptr;
			const std::wstring shaderFilepath = L"..\\Shaders_SOURCE\\";

			 D3DCompileFromFile((shaderFilepath + fileName + L"VS.hlsl").c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE
				, "main", "vs_5_0", shaderFlags, 0, ppCode, &errorBlob);



			std::wstring fp = (shaderFilepath + fileName + L"VS.hlsl");
		
			if (errorBlob)
			{

				OutputDebugStringA((char*)errorBlob->GetBufferPointer());
				errorBlob->Release();
				assert(NULL && "hlsl file have problem check message! VS");
				return false;
			}

			if (FAILED(mDevice->CreateVertexShader((*ppCode)->GetBufferPointer()
				, (*ppCode)->GetBufferSize(), nullptr, ppVertexShader)))
				return false;

			return true;
		
		
	}

	bool GraphicDevice_DX11::CreatePixelShader(const std::wstring& fileName, ID3DBlob** ppCode, ID3D11PixelShader** ppPixelShader)
	{
		DWORD shaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
		shaderFlags |= D3DCOMPILE_DEBUG;
		shaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
		//pixel shader
		{
			ID3DBlob* errorBlob = nullptr;
			const std::wstring shaderFilpath = L"..\\Shaders_SOURCE\\";

			D3DCompileFromFile((shaderFilpath + fileName+ L"PS.hlsl").c_str(), nullptr, D3D_COMPILE_STANDARD_FILE_INCLUDE
				, "main", "ps_5_0", shaderFlags, 0, ppCode, &errorBlob);

			if (errorBlob)
			{
				OutputDebugStringA((char*)errorBlob->GetBufferPointer());
				errorBlob->Release();
				assert(NULL && "hlsl file have problem check message! PS");
				return false;
			}

			if (FAILED(mDevice->CreatePixelShader((*ppCode)->GetBufferPointer()
				, (*ppCode)->GetBufferSize(), nullptr, ppPixelShader)))
				return false;

			return true;

		}
	}

	bool GraphicDevice_DX11::CreateInputLayout(const D3D11_INPUT_ELEMENT_DESC* pInputElementDescs, UINT NumElements
		, const void* pShaderBytecodeWithInputSignature, SIZE_T BytecodeLength, ID3D11InputLayout** ppInputLayout)
	{
		if (FAILED(mDevice->CreateInputLayout(pInputElementDescs, NumElements
			, pShaderBytecodeWithInputSignature
			, BytecodeLength
			, ppInputLayout)))
			return false;

		return true;
	}

	bool GraphicDevice_DX11::CreateBuffer(const D3D11_BUFFER_DESC* pDesc, const D3D11_SUBRESOURCE_DATA* pInitialData, ID3D11Buffer** ppBuffer)
	{
		if(FAILED(mDevice->CreateBuffer(pDesc, pInitialData, ppBuffer)))
			return false;

		return true;
	}

	bool GraphicDevice_DX11::CreateShaderResourceView(ID3D11Resource* pResource, const D3D11_SHADER_RESOURCE_VIEW_DESC* pDesc, ID3D11ShaderResourceView** ppSRView)
	{
		if (FAILED(mDevice->CreateShaderResourceView(pResource, pDesc, ppSRView)))
			return false;

		return true;
	}

	bool GraphicDevice_DX11::CreateRasterizerState(const D3D11_RASTERIZER_DESC* pRasterizerDesc, ID3D11RasterizerState** pRasterizerState)
	{
		if(FAILED(mDevice->CreateRasterizerState(pRasterizerDesc, pRasterizerState)))
			return false;

		return true;
	}

	bool GraphicDevice_DX11::CreateBlendState(const D3D11_BLEND_DESC* pBlendDesc, ID3D11BlendState** pBlendState)
	{
		if (FAILED(mDevice->CreateBlendState(pBlendDesc, pBlendState)))
			return false;

		return true;
	}

	bool GraphicDevice_DX11::CreateDepthStencilState(const D3D11_DEPTH_STENCIL_DESC* pDepthStencilDesc, ID3D11DepthStencilState** pDepthStencilState)
	{
		if (FAILED(mDevice->CreateDepthStencilState(pDepthStencilDesc, pDepthStencilState)))
			return false;

		return true;
	}


	void GraphicDevice_DX11::SetShaderResource(eShaderStage stage, UINT startSlot, ID3D11ShaderResourceView** ppSRV)
	{
		if ((UINT)eShaderStage::VS & (UINT)stage)
			mContext->VSSetShaderResources(startSlot, 1, ppSRV);
		if ((UINT)eShaderStage::HS & (UINT)stage)
			mContext->HSSetShaderResources(startSlot, 1, ppSRV);
		if ((UINT)eShaderStage::DS & (UINT)stage)
			mContext->DSSetShaderResources(startSlot, 1, ppSRV);
		if ((UINT)eShaderStage::GS & (UINT)stage)
			mContext->GSSetShaderResources(startSlot, 1, ppSRV);
		if ((UINT)eShaderStage::PS & (UINT)stage)
			mContext->PSSetShaderResources(startSlot, 1, ppSRV);
		if ((UINT)eShaderStage::CS & (UINT)stage)
			mContext->CSSetShaderResources(startSlot, 1, ppSRV);
	}

	void GraphicDevice_DX11::SetDataGpuBuffer(ID3D11Buffer* buffer, void* data, UINT size)
	{
		D3D11_MAPPED_SUBRESOURCE sub = {};
		mContext->Map(buffer, 0, D3D11_MAP::D3D11_MAP_WRITE_DISCARD, 0, &sub);

		memcpy(sub.pData, data, size);
		mContext->Unmap(buffer, 0);

	
	}

	void GraphicDevice_DX11::BindInputLayout(ID3D11InputLayout* pInputLayout)
	{
		mContext->IASetInputLayout(pInputLayout);
	}

	void GraphicDevice_DX11::BindPrimitiveTopology(const D3D11_PRIMITIVE_TOPOLOGY topology)
	{
		mContext->IASetPrimitiveTopology(topology);
	}

	void GraphicDevice_DX11::BindVS(ID3D11VertexShader * pVertexShader)
	{
		mContext->VSSetShader(pVertexShader, 0, 0);
	}

	void GraphicDevice_DX11::BindPS(ID3D11PixelShader* pPixelShader)
	{
		mContext->PSSetShader(pPixelShader, 0, 0);
	}

	void GraphicDevice_DX11::BindVertexBuffer(UINT StartSlot, UINT NumBuffers, ID3D11Buffer* const* ppVertexBuffers
		, const UINT* pStrides, const UINT* pOffsets)
	{
		mContext->IASetVertexBuffers(StartSlot, NumBuffers, ppVertexBuffers, pStrides, pOffsets);
	}

	void GraphicDevice_DX11::BindIndexBuffer(ID3D11Buffer* pIndexBuffer, DXGI_FORMAT Format, UINT Offset)
	{
		mContext->IASetIndexBuffer(pIndexBuffer, Format, Offset);
	}

	void GraphicDevice_DX11::BindConstantBuffer(eShaderStage stage, eCBType type, ID3D11Buffer* buffer)
	{
		UINT slot = (UINT)type;
	
		switch (stage)
		{
		case ME::graphics::eShaderStage::VS:
			mContext->VSSetConstantBuffers(slot, 1, &buffer);
			break;
		case ME::graphics::eShaderStage::HS:
			mContext->HSSetConstantBuffers(slot, 1, &buffer);
			break;
		case ME::graphics::eShaderStage::DS:
			mContext->DSSetConstantBuffers(slot, 1, &buffer);
			break;
		case ME::graphics::eShaderStage::GS:
			mContext->GSSetConstantBuffers(slot, 1, &buffer);
			break;
		case ME::graphics::eShaderStage::PS:
			mContext->PSSetConstantBuffers(slot, 1, &buffer);
			break;
		case ME::graphics::eShaderStage::CS:
			mContext->CSSetConstantBuffers(slot, 1, &buffer);
			break;
		case ME::graphics::eShaderStage::All:
			mContext->VSSetConstantBuffers(slot, 1, &buffer);
			mContext->HSSetConstantBuffers(slot, 1, &buffer);
			mContext->DSSetConstantBuffers(slot, 1, &buffer);
			mContext->GSSetConstantBuffers(slot, 1, &buffer);
			mContext->PSSetConstantBuffers(slot, 1, &buffer);
			mContext->CSSetConstantBuffers(slot, 1, &buffer);
			break;
		default:
			break;
		}
	}

	void GraphicDevice_DX11::BindSampler(eShaderStage stage, UINT StartSlot, UINT NumSamplers, ID3D11SamplerState* const* ppSamplers)
	{
		if (mContext == nullptr)
			assert(NULL && "mContext is nullptr! BindSampler");

		if (eShaderStage::VS == stage)
			mContext->VSSetSamplers(StartSlot, NumSamplers, ppSamplers);
		if (eShaderStage::HS == stage)
			mContext->HSSetSamplers(StartSlot, NumSamplers, ppSamplers);
		if (eShaderStage::DS == stage)
			mContext->DSSetSamplers(StartSlot, NumSamplers, ppSamplers);
		if (eShaderStage::GS == stage)
			mContext->GSSetSamplers(StartSlot, NumSamplers, ppSamplers);
		if (eShaderStage::PS == stage)
			mContext->PSSetSamplers(StartSlot, NumSamplers, ppSamplers);
	}
	void GraphicDevice_DX11::BindSamplers(UINT StartSlot, UINT NumSamplers, ID3D11SamplerState* const* ppSamplers)
	{
		BindSampler(eShaderStage::VS, StartSlot, NumSamplers, ppSamplers);
		BindSampler(eShaderStage::HS, StartSlot, NumSamplers, ppSamplers);
		BindSampler(eShaderStage::DS, StartSlot, NumSamplers, ppSamplers);
		BindSampler(eShaderStage::GS, StartSlot, NumSamplers, ppSamplers);
		BindSampler(eShaderStage::PS, StartSlot, NumSamplers, ppSamplers);
	}

	void GraphicDevice_DX11::BindRasterizerState(ID3D11RasterizerState* pRasterizerState)
	{
		mContext->RSSetState(pRasterizerState);
	}

	void GraphicDevice_DX11::BindBlendState(ID3D11BlendState* pBlendState, const FLOAT BlendFactor[4], UINT SampleMask)
	{
		mContext->OMSetBlendState(pBlendState, BlendFactor, SampleMask);
	}

	void GraphicDevice_DX11::BindDepthStencilState(ID3D11DepthStencilState* pDepthStencilState, UINT StencilRef)
	{
		mContext->OMSetDepthStencilState(pDepthStencilState, StencilRef);
	}

	void GraphicDevice_DX11::BindViewPort()
	{

		D3D11_VIEWPORT viewPort =
		{
			0, 0, (float)application.GetWidth(), (float)application.GetHeight(),
			0.0f, 1.0f
		};
		mContext->RSSetViewports(1, &viewPort);
	}

	void GraphicDevice_DX11::BindRenderTargets(UINT NumViews, ID3D11RenderTargetView* const* ppRenderTargetViews,
		ID3D11DepthStencilView* pDepthStencilViews)
	{
		mContext->OMSetRenderTargets(NumViews, ppRenderTargetViews, pDepthStencilViews);
	}	


	void GraphicDevice_DX11::BindDefaultRenderTarget()
	{
		mContext->OMSetRenderTargets(1, mRenderTargetView.GetAddressOf(), mDepthStencilView.Get());
	}

	void GraphicDevice_DX11::ClearRenderTargetView()
	{
		FLOAT backgroundColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
		mContext->ClearRenderTargetView(mRenderTargetView.Get(), backgroundColor);
	}

	void GraphicDevice_DX11::ClearDepthStencilView()
	{
		mContext->ClearDepthStencilView(mDepthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);
	}

	void GraphicDevice_DX11::Initialize()
	{

#pragma region swapchain desc

		DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
		swapChainDesc.OutputWindow = application.GetHwnd();
		swapChainDesc.Windowed = true;
		swapChainDesc.BufferCount = 2;
		swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD; // 이전 프레임 장면을 유지하지 않는다.
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.BufferDesc.Width = application.GetWidth();
		swapChainDesc.BufferDesc.Height = application.GetHeight();
		swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		swapChainDesc.BufferDesc.RefreshRate.Numerator = 144;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
		swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
		swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;

#pragma region 4X MSAA surported check

		UINT quility = 0;
		mDevice->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UNORM, 4, &quility);
		//if (quility > 0)
		//{
		//	swapChainDesc.SampleDesc.Count = 4; // how many multisamples
		//	swapChainDesc.SampleDesc.Quality = quility - 1;
		//}
#pragma endregion

		swapChainDesc.SampleDesc.Count = 1; // how many multisamples
		swapChainDesc.SampleDesc.Quality = 0;
		
#pragma endregion

		if(!(CreateSwapChain(swapChainDesc)))
			assert(NULL && "Create SwapChain Failed!");

		if(!(GetBuffer(0,__uuidof(ID3D11Texture2D), (void**)mRenderTarget.GetAddressOf())))
			assert(NULL && "Couldn't Bring RenderTarget!");

		if (!(CreateRenderTargetView(mRenderTarget.Get(), nullptr, mRenderTargetView.GetAddressOf())))
			assert(NULL && "Create RenderTargetView Failed!");

#pragma region depthstencil desc

			
		D3D11_TEXTURE2D_DESC depthStencilDesc = {};
		depthStencilDesc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_DEPTH_STENCIL;
		depthStencilDesc.Format = DXGI_FORMAT::DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthStencilDesc.Usage = D3D11_USAGE::D3D11_USAGE_DEFAULT;
		depthStencilDesc.Width = application.GetWidth();
		depthStencilDesc.Height = application.GetHeight();
		depthStencilDesc.ArraySize = 1;
		depthStencilDesc.SampleDesc.Count = 1;
		depthStencilDesc.SampleDesc.Quality = 0;
		
#pragma endregion

		if (!(CreateTexture2D(&depthStencilDesc, nullptr, mDepthStencil.GetAddressOf())))
			assert(NULL && "Create depthstencil texture failed!");

		if (!(CreateDepthStencilView(mDepthStencil.Get(), nullptr, mDepthStencilView.GetAddressOf())))
			assert(NULL && "Create depthstencilview failed!");
	

	}

	void GraphicDevice_DX11::Draw(UINT vertexCount, UINT StartVertexLocation)
	{

		mContext->Draw(vertexCount, StartVertexLocation);

	}


	void GraphicDevice_DX11::DrawIndexed(UINT indexCount, UINT StartIndexLocation, INT BaseVertexLocation)
	{
		mContext->DrawIndexed(indexCount, StartIndexLocation, BaseVertexLocation);
	}

	void GraphicDevice_DX11::Present()
	{
		mSwapChain->Present(1, 0);
	}

}