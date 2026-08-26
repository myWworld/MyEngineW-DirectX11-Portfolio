#pragma once
#include "MEGraphics.h"


//https://github.com/kevinmoran/BeginnerDirect3D11

namespace ME::graphics
{
	class GraphicDevice_DX11
	{
	public:
		GraphicDevice_DX11();
		~GraphicDevice_DX11();

		bool CreateDevice();
		bool CreateSwapChain(DXGI_SWAP_CHAIN_DESC desc);
		bool GetBuffer(UINT buffer, REFIID riid, void** ppSurface);
		bool CreateRenderTargetView(ID3D11Resource* pResource, const D3D11_RENDER_TARGET_VIEW_DESC* pDesc, ID3D11RenderTargetView** ppRTView);
		bool CreateDepthStencilView(ID3D11Resource* pResource, const D3D11_DEPTH_STENCIL_VIEW_DESC* pDesc, ID3D11DepthStencilView** ppDepthStencilView);
		bool CreateTexture2D(const D3D11_TEXTURE2D_DESC* pDesc, const D3D11_SUBRESOURCE_DATA* pInitialData, ID3D11Texture2D** ppTexture2D);

		bool CreateSamplerState(const D3D11_SAMPLER_DESC* pSamplerDesc, ID3D11SamplerState** ppSamplerState);

		bool CreateVertexShader(const std::wstring& fileName, ID3DBlob** ppCode, ID3D11VertexShader** ppVertexShader);
		bool CreatePixelShader(const std::wstring& fileName, ID3DBlob** ppCode, ID3D11PixelShader** ppPixelShader);
		bool CreateInputLayout(const D3D11_INPUT_ELEMENT_DESC* pInputElementDescs, UINT NumElements
			, const void* pShaderBytecodeWithInputSignature, SIZE_T BytecodeLength, ID3D11InputLayout** ppInputLayout);
		bool CreateBuffer(const D3D11_BUFFER_DESC* pDesc, const D3D11_SUBRESOURCE_DATA* pInitialData, ID3D11Buffer** ppBuffer);

		bool CreateShaderResourceView(ID3D11Resource* pResource, const D3D11_SHADER_RESOURCE_VIEW_DESC* pDesc, ID3D11ShaderResourceView** ppSRView);

		bool CreateRasterizerState(const D3D11_RASTERIZER_DESC* pRasterizerDesc, ID3D11RasterizerState** pRasterizerState);
		bool CreateBlendState(const D3D11_BLEND_DESC* pBlendDesc, ID3D11BlendState** pBlendState);
		bool CreateDepthStencilState(const D3D11_DEPTH_STENCIL_DESC* pDepthStencilDesc, ID3D11DepthStencilState** pDepthStencilState);

		void SetDataGpuBuffer(ID3D11Buffer* buffer, void* data, UINT size);
		void SetShaderResource(eShaderStage stage, UINT startSlot, ID3D11ShaderResourceView** ppSRV);

		void BindInputLayout(ID3D11InputLayout* pInputLayout);
		void BindPrimitiveTopology(const D3D11_PRIMITIVE_TOPOLOGY topology);
		void BindVS(ID3D11VertexShader* pVertexShader);
		void BindPS(ID3D11PixelShader* pPixelShader);

		void BindVertexBuffer(UINT StartSlot, UINT NumBuffers, ID3D11Buffer* const* ppVertexBuffers
			, const UINT* pStrides, const UINT* pOffsets);
		void BindIndexBuffer(ID3D11Buffer* pIndexBuffer, DXGI_FORMAT Format, UINT Offset);

		void BindConstantBuffer(eShaderStage stage, eCBType type, ID3D11Buffer* buffer);

		void BindSampler(eShaderStage stage, UINT StartSlot, UINT NumSamplers, ID3D11SamplerState* const* ppSamplers);
		void BindSamplers(UINT StartSlot, UINT NumSamplers, ID3D11SamplerState* const* ppSamplers);

		void BindRasterizerState(ID3D11RasterizerState* pRasterizerState);
		void BindBlendState(ID3D11BlendState* pBlendState, const FLOAT BlendFactor[4], UINT SampleMask);
		void BindDepthStencilState(ID3D11DepthStencilState* pDepthStencilState, UINT StencilRef);


		void BindViewPort();
		void BindRenderTargets(UINT NumViews = 1, ID3D11RenderTargetView* const* ppRenderTargetViews = nullptr,
			ID3D11DepthStencilView* pDepthStencilViews = nullptr);
		void BindDefaultRenderTarget();

		void ClearRenderTargetView();
		void ClearDepthStencilView();



		void Initialize();
		void Draw(UINT vertexCount , UINT StartVertexLocation);
		
		void DrawIndexed(UINT indexCount, UINT StartIndexLocation, INT BaseVertexLocation);
		void Present();

		[[nodiscard("ID3D11Device 값을 받고 있지 않습니다!")]]
		Microsoft::WRL::ComPtr<ID3D11Device> GetID3D11Device() { return mDevice; }

	private:
		Microsoft::WRL::ComPtr<ID3D11Device> mDevice;
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> mContext;
		Microsoft::WRL::ComPtr<ID3D11Texture2D> mRenderTarget;
		Microsoft::WRL::ComPtr<ID3D11RenderTargetView>	mRenderTargetView;
		Microsoft::WRL::ComPtr<ID3D11Texture2D>			mDepthStencil;
		Microsoft::WRL::ComPtr<ID3D11DepthStencilView>	mDepthStencilView;

		Microsoft::WRL::ComPtr<IDXGISwapChain>	mSwapChain;
		Microsoft::WRL::ComPtr<ID3D11SamplerState> mSamplers;
	};

	

	inline GraphicDevice_DX11*& GetDevice()
	{
		static GraphicDevice_DX11* device = nullptr;
		return device;
	}
}