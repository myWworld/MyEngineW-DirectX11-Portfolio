#pragma once
#include "MECamera.h"
#include "MEGraphicDevice_DX11.h"
#include "MEMesh.h"
#include "MEConstantBuffer.h"

using namespace ME::math;
using namespace ME::graphics;

namespace ME::renderer 
{
	extern Camera* mainCamera;
	extern Camera* playerCamera;

	extern graphics::ConstantBuffer* constantBuffers[(UINT)graphics::eCBType::End];
	
	extern Microsoft::WRL::ComPtr<ID3D11SamplerState> samplterState[(UINT)eSamplerType::End];
	 extern Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizerStates[(UINT)eRasterizerState::End];
	 extern Microsoft::WRL::ComPtr<ID3D11BlendState> blendStates[(UINT)eBlendState::End];
	 extern Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depthStencilStates[(UINT)eDepthStencilState::End];

	 void LoadModels(Mesh* mesh);
	 void LoadStaticModels(Mesh* mesh);

	void Initialize();
	void Release();

}

