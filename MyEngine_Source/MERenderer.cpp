#include "MERenderer.h"
#include "MEGraphicDevice_DX11.h"

#include "MEResources.h"
#include "MEShader.h"

#include "MEMaterial.h"


namespace ME::renderer
{
	Camera* mainCamera = nullptr;
	Camera* playerCamera = nullptr;

	

	ConstantBuffer* constantBuffers[(UINT)graphics::eCBType::End] = {};

	Microsoft::WRL::ComPtr<ID3D11SamplerState> samplerStates[(UINT)eSamplerType::End] = {};
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizerStates[(UINT)eRasterizerState::End] = {};
	Microsoft::WRL::ComPtr<ID3D11BlendState> blendStates[(UINT)eBlendState::End] = {};
	Microsoft::WRL::ComPtr<ID3D11DepthStencilState> depthStencilStates[(UINT)eDepthStencilState::End] = {};

	void LoadStates()
	{
#pragma region SamplerState

		D3D11_SAMPLER_DESC samplerDesc = {};
		samplerDesc.Filter = D3D11_FILTER::D3D11_FILTER_ANISOTROPIC;
		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.ComparisonFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_NEVER;
		samplerDesc.MinLOD = 0;
		samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
		GetDevice()->CreateSamplerState(&samplerDesc, samplerStates[(UINT)eSamplerType::Anisotropic].GetAddressOf());

		ZeroMemory(&samplerDesc, sizeof(samplerDesc));

		samplerDesc.Filter = D3D11_FILTER::D3D11_FILTER_COMPARISON_MIN_MAG_MIP_POINT;
		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.ComparisonFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_NEVER;
		samplerDesc.MinLOD = 0;
		samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
		GetDevice()->CreateSamplerState(&samplerDesc, samplerStates[(UINT)eSamplerType::Point].GetAddressOf());

		ZeroMemory(&samplerDesc, sizeof(samplerDesc));


		samplerDesc.Filter = D3D11_FILTER::D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_WRAP;
		samplerDesc.ComparisonFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_NEVER;
		samplerDesc.MinLOD = 0;
		samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
		GetDevice()->CreateSamplerState(&samplerDesc, samplerStates[(UINT)eSamplerType::Linear].GetAddressOf());

		ZeroMemory(&samplerDesc, sizeof(samplerDesc));

		samplerDesc.Filter = D3D11_FILTER::D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_CLAMP;
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_CLAMP;
		samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_MODE::D3D11_TEXTURE_ADDRESS_CLAMP;
		samplerDesc.ComparisonFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_NEVER;
		samplerDesc.MinLOD = 0;
		samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
		GetDevice()->CreateSamplerState(&samplerDesc, samplerStates[(UINT)eSamplerType::PostProcess].GetAddressOf());

		GetDevice()->BindSamplers((UINT)eSamplerType::Point, 1, samplerStates[(UINT)eSamplerType::Point].GetAddressOf());
		GetDevice()->BindSamplers((UINT)eSamplerType::Linear, 1, samplerStates[(UINT)eSamplerType::Linear].GetAddressOf());
		GetDevice()->BindSamplers((UINT)eSamplerType::Anisotropic, 1, samplerStates[(UINT)eSamplerType::Anisotropic].GetAddressOf());
		GetDevice()->BindSamplers((UINT)eSamplerType::PostProcess, 1, samplerStates[(UINT)eSamplerType::PostProcess].GetAddressOf());

#pragma endregion

#pragma region rasterizer state

		D3D11_RASTERIZER_DESC rsDesc = {};

		rsDesc.AntialiasedLineEnable = false;
		rsDesc.CullMode = D3D11_CULL_MODE::D3D11_CULL_BACK;
		rsDesc.DepthBias = 0;
		rsDesc.DepthBiasClamp = 0.0f;
		rsDesc.DepthClipEnable = true;
		rsDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;
		rsDesc.FrontCounterClockwise = false;
		rsDesc.MultisampleEnable = false;
		rsDesc.ScissorEnable = false;
		rsDesc.SlopeScaledDepthBias = 0.0f;

		GetDevice()->CreateRasterizerState(&rsDesc, rasterizerStates[(UINT)eRasterizerState::SolidBack].GetAddressOf());

		rsDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;
		rsDesc.CullMode = D3D11_CULL_MODE::D3D11_CULL_FRONT;

		GetDevice()->CreateRasterizerState(&rsDesc, rasterizerStates[(UINT)eRasterizerState::SolidFront].GetAddressOf());

		rsDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;
		rsDesc.CullMode = D3D11_CULL_MODE::D3D11_CULL_NONE;

		GetDevice()->CreateRasterizerState(&rsDesc, rasterizerStates[(UINT)eRasterizerState::SolidNone].GetAddressOf());

		rsDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_WIREFRAME;
		rsDesc.CullMode = D3D11_CULL_MODE::D3D11_CULL_NONE;

		GetDevice()->CreateRasterizerState(&rsDesc, rasterizerStates[(UINT)eRasterizerState::WireFrame].GetAddressOf());




#pragma endregion


#pragma region blend state

		D3D11_BLEND_DESC bsDesc = {};

		bsDesc.AlphaToCoverageEnable = false;
		bsDesc.IndependentBlendEnable = false;

		bsDesc.RenderTarget[0].BlendEnable = false;
		bsDesc.RenderTarget[0].SrcBlend = D3D11_BLEND::D3D11_BLEND_SRC_ALPHA;
		bsDesc.RenderTarget[0].DestBlend = D3D11_BLEND::D3D11_BLEND_INV_SRC_ALPHA;
		bsDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP::D3D11_BLEND_OP_ADD;
		bsDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND::D3D11_BLEND_ONE;
		bsDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND::D3D11_BLEND_ZERO;
		bsDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP::D3D11_BLEND_OP_ADD;
		bsDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE::D3D11_COLOR_WRITE_ENABLE_ALL;

		GetDevice()->CreateBlendState(&bsDesc, blendStates[(UINT)eBlendState::Opaque].GetAddressOf());

		bsDesc.RenderTarget[0].BlendEnable = true;
		bsDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		bsDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;

		GetDevice()->CreateBlendState(&bsDesc, blendStates[(UINT)eBlendState::AlphaBlend].GetAddressOf());

		bsDesc.RenderTarget[0].SrcBlend = D3D11_BLEND::D3D11_BLEND_ONE;
		bsDesc.RenderTarget[0].DestBlend = D3D11_BLEND::D3D11_BLEND_ONE;
		GetDevice()->CreateBlendState(&bsDesc, blendStates[(UINT)eBlendState::OneOne].GetAddressOf());

#pragma endregion


#pragma region depthstencil state

		D3D11_DEPTH_STENCIL_DESC dsDesc = {};

		dsDesc.DepthEnable = true;
		dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK::D3D11_DEPTH_WRITE_MASK_ALL;
		dsDesc.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_LESS_EQUAL;
		dsDesc.StencilEnable = false;

		GetDevice()->CreateDepthStencilState(&dsDesc, depthStencilStates[(UINT)eDepthStencilState::LessEqual].GetAddressOf());

		dsDesc.DepthEnable = false;
		dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK::D3D11_DEPTH_WRITE_MASK_ZERO;
		dsDesc.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_LESS_EQUAL;
		dsDesc.StencilEnable = false;
		GetDevice()->CreateDepthStencilState(&dsDesc, depthStencilStates[(UINT)eDepthStencilState::DepthNone].GetAddressOf());

#pragma endregion
	}
	
	void LoadTriangleMesh()
	{
		std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>();

		std::vector<graphics::Vertex> vertexes = {};
		std::vector<UINT> indices;

		vertexes.resize(3);

		vertexes[0].pos = Vector3(0.f, 0.5f, 0.0f);
		vertexes[0].color = Vector4(0.0f, 1.0f, 0.0f, 1.0f);

		vertexes[1].pos = Vector3(0.5f, -0.5f, 0.0f);
		vertexes[1].color = Vector4(1.0f, 0.0f, 0.0f, 1.0f);

		vertexes[2].pos = Vector3(-0.5f, -0.5f, 0.0f);
		vertexes[2].color = Vector4(0.0f, 0.0f, 1.0f, 1.0f);

		indices.push_back(0);
		indices.push_back(1);
		indices.push_back(2);


		D3D11_INPUT_ELEMENT_DESC inputLayoutDesces[2] = {};
		inputLayoutDesces[0].AlignedByteOffset = 0;
		inputLayoutDesces[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
		inputLayoutDesces[0].InputSlot = 0;
		inputLayoutDesces[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		inputLayoutDesces[0].SemanticName = "POSITION";
		inputLayoutDesces[0].SemanticIndex = 0;

		inputLayoutDesces[1].AlignedByteOffset = 12;
		inputLayoutDesces[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		inputLayoutDesces[1].InputSlot = 0;
		inputLayoutDesces[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		inputLayoutDesces[1].SemanticName = "COLOR";
		inputLayoutDesces[1].SemanticIndex = 0;

		graphics::Shader* triangleShader = Resources::Find<graphics::Shader>(L"TriangleShader").get();
		mesh->SetVertexBufferParams(2, inputLayoutDesces, triangleShader->GetVSBlob()->GetBufferPointer(), triangleShader->GetVSBlob()->GetBufferSize());

		mesh->CreateIB(indices);
		mesh->CreateVB(vertexes);

		ME::Resources::Insert(L"TriangleMesh", mesh);
	}

	void LoadRectMesh()
	{
		std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>();

		std::vector<graphics::Vertex> vertexes = {};
		std::vector<UINT> indices;

		vertexes.resize(4);

		vertexes[0].pos = Vector3(-0.5f, 0.5f, 0.0f);
		vertexes[0].color = Vector4(0.0f, 1.0f, 0.0f, 1.0f);
		vertexes[0].uv = Vector2(0.0f, 0.0f);

		vertexes[1].pos = Vector3(0.5f, 0.5f, 0.0f);
		vertexes[1].color = Vector4(1.0f, 0.0f, 0.0f, 1.0f);
		vertexes[1].uv = Vector2(1.0f, 0.0f);

		vertexes[2].pos = Vector3(0.5f, -0.5f, 0.0f);
		vertexes[2].color = Vector4(0.0f, 0.0f, 1.0f, 1.0f);
		vertexes[2].uv = Vector2(1.0f, 1.0f);

		vertexes[3].pos = Vector3(-0.5f, -0.5f, 0.0f);
		vertexes[3].color = Vector4(0.0f, 0.0f, 1.0f, 1.0f);
		vertexes[3].uv = Vector2(0.0f, 1.0f);

		indices.push_back(0);
		indices.push_back(2);
		indices.push_back(3);

		indices.push_back(0);
		indices.push_back(1);
		indices.push_back(2);

		D3D11_INPUT_ELEMENT_DESC inputLayoutDesces[4] = {};

		
		inputLayoutDesces[0].AlignedByteOffset = 0;
		inputLayoutDesces[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
		inputLayoutDesces[0].InputSlot = 0;
		inputLayoutDesces[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		inputLayoutDesces[0].SemanticName = "POSITION";
		inputLayoutDesces[0].SemanticIndex = 0;

		inputLayoutDesces[1].AlignedByteOffset = 12;
		inputLayoutDesces[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		inputLayoutDesces[1].InputSlot = 0;
		inputLayoutDesces[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		inputLayoutDesces[1].SemanticName = "COLOR";
		inputLayoutDesces[1].SemanticIndex = 0;

		inputLayoutDesces[2].AlignedByteOffset = 28; // pos(12) + color(16)
		inputLayoutDesces[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
		inputLayoutDesces[2].InputSlot = 0;
		inputLayoutDesces[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		inputLayoutDesces[2].SemanticName = "NORMAL";
		inputLayoutDesces[2].SemanticIndex = 0;

		inputLayoutDesces[3].AlignedByteOffset = 52; // pos(12) + color(16) + normal(12) + tangent(12)
		inputLayoutDesces[3].Format = DXGI_FORMAT_R32G32_FLOAT;
		inputLayoutDesces[3].InputSlot = 0;
		inputLayoutDesces[3].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		inputLayoutDesces[3].SemanticName = "TEXCOORD";
		inputLayoutDesces[3].SemanticIndex = 0;

		graphics::Shader* spriteShader = Resources::Find<graphics::Shader>(L"SpriteDefaultShader").get();

		if (spriteShader != nullptr)
		{
			mesh->SetVertexBufferParams(4, inputLayoutDesces, spriteShader->GetVSBlob()->GetBufferPointer(), spriteShader->GetVSBlob()->GetBufferSize());
		}

		mesh->CreateIB(indices);
		mesh->CreateVB(vertexes);

		ME::Resources::Insert(L"RectMesh", mesh);
	}

	void LoadModels(Mesh* mesh)
	{
		D3D11_INPUT_ELEMENT_DESC inputLayoutDesces[7] = {};
		inputLayoutDesces[0].AlignedByteOffset = 0;
		inputLayoutDesces[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
		inputLayoutDesces[0].InputSlot = 0;
		inputLayoutDesces[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		inputLayoutDesces[0].SemanticName = "POSITION";
		inputLayoutDesces[0].SemanticIndex = 0;

		inputLayoutDesces[1].AlignedByteOffset = 12;
		inputLayoutDesces[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		inputLayoutDesces[1].InputSlot = 0;
		inputLayoutDesces[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		inputLayoutDesces[1].SemanticName = "COLOR";
		inputLayoutDesces[1].SemanticIndex = 0;

		inputLayoutDesces[2].AlignedByteOffset = 28; 
		inputLayoutDesces[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
		inputLayoutDesces[2].InputSlot = 0;
		inputLayoutDesces[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		inputLayoutDesces[2].SemanticName = "NORMAL";
		inputLayoutDesces[2].SemanticIndex = 0;

		
		inputLayoutDesces[3].AlignedByteOffset = 40;
		inputLayoutDesces[3].Format = DXGI_FORMAT_R32G32B32_FLOAT;
		inputLayoutDesces[3].InputSlot = 0;
		inputLayoutDesces[3].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		inputLayoutDesces[3].SemanticName = "TANGENT";
		inputLayoutDesces[3].SemanticIndex = 0;


		inputLayoutDesces[4].AlignedByteOffset = 52;
		inputLayoutDesces[4].Format = DXGI_FORMAT_R32G32_FLOAT;
		inputLayoutDesces[4].InputSlot = 0;
		inputLayoutDesces[4].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		inputLayoutDesces[4].SemanticName = "TEXCOORD";
		inputLayoutDesces[4].SemanticIndex = 0;


		inputLayoutDesces[5].AlignedByteOffset = 60; 
		inputLayoutDesces[5].Format = DXGI_FORMAT_R32G32B32A32_UINT;
		inputLayoutDesces[5].InputSlot = 0;
		inputLayoutDesces[5].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		inputLayoutDesces[5].SemanticName = "BONEINDICES";
		inputLayoutDesces[5].SemanticIndex = 0;


		inputLayoutDesces[6].AlignedByteOffset = 76; 
		inputLayoutDesces[6].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		inputLayoutDesces[6].InputSlot = 0;
		inputLayoutDesces[6].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		inputLayoutDesces[6].SemanticName = "BONEWEIGHTS";
		inputLayoutDesces[6].SemanticIndex = 0;



		graphics::Shader* modelShader = Resources::Find<graphics::Shader>(L"ModelShader").get();

		if (modelShader != nullptr)
		{
			modelShader->SetBlendState(graphics::eBlendState::Opaque);

			mesh->SetVertexBufferParams(7, inputLayoutDesces, modelShader->GetVSBlob()->GetBufferPointer(), modelShader->GetVSBlob()->GetBufferSize());
		}
	}

	void LoadStaticModels(Mesh* mesh)
	{
		D3D11_INPUT_ELEMENT_DESC inputLayoutDesces[5] = {};
		inputLayoutDesces[0].AlignedByteOffset = 0;
		inputLayoutDesces[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
		inputLayoutDesces[0].InputSlot = 0;
		inputLayoutDesces[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		inputLayoutDesces[0].SemanticName = "POSITION";
		inputLayoutDesces[0].SemanticIndex = 0;

		inputLayoutDesces[1].AlignedByteOffset = 12;
		inputLayoutDesces[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		inputLayoutDesces[1].InputSlot = 0;
		inputLayoutDesces[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		inputLayoutDesces[1].SemanticName = "COLOR";
		inputLayoutDesces[1].SemanticIndex = 0;

		inputLayoutDesces[2].AlignedByteOffset = 28; //12 + 16
		inputLayoutDesces[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
		inputLayoutDesces[2].InputSlot = 0;
		inputLayoutDesces[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		inputLayoutDesces[2].SemanticName = "NORMAL";
		inputLayoutDesces[2].SemanticIndex = 0;

		inputLayoutDesces[3].AlignedByteOffset = 40; //28 + 12
		inputLayoutDesces[3].Format = DXGI_FORMAT_R32G32B32_FLOAT;
		inputLayoutDesces[3].InputSlot = 0;
		inputLayoutDesces[3].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		inputLayoutDesces[3].SemanticName = "TANGENT";
		inputLayoutDesces[3].SemanticIndex = 0;


		inputLayoutDesces[4].AlignedByteOffset = 52; //40 + 12
		inputLayoutDesces[4].Format = DXGI_FORMAT_R32G32_FLOAT;
		inputLayoutDesces[4].InputSlot = 0;
		inputLayoutDesces[4].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		inputLayoutDesces[4].SemanticName = "TEXCOORD";
		inputLayoutDesces[4].SemanticIndex = 0;

		graphics::Shader* modelShader = Resources::Find<graphics::Shader>(L"StaticModelShader").get();

		if (modelShader != nullptr)
		{
			modelShader->SetBlendState(graphics::eBlendState::Opaque);
			mesh->SetVertexBufferParams(5, inputLayoutDesces, modelShader->GetVSBlob()->GetBufferPointer(), modelShader->GetVSBlob()->GetBufferSize());
		}
	}

	void LoadCubeMesh()
	{
		std::shared_ptr<Mesh> mesh = std::make_shared<Mesh>();

		std::vector<graphics::Vertex> vertexes = {};
		std::vector<UINT> indices;

		vertexes.resize(8);

		// 앞면 (Front)
		vertexes[0].pos = Vector3(-0.5f, 0.5f, -0.5f);
		vertexes[1].pos = Vector3(0.5f, 0.5f, -0.5f);
		vertexes[2].pos = Vector3(0.5f, -0.5f, -0.5f);
		vertexes[3].pos = Vector3(-0.5f, -0.5f, -0.5f);

		// 뒷면 (Back)
		vertexes[4].pos = Vector3(-0.5f, 0.5f, 0.5f);
		vertexes[5].pos = Vector3(0.5f, 0.5f, 0.5f);
		vertexes[6].pos = Vector3(0.5f, -0.5f, 0.5f);
		vertexes[7].pos = Vector3(-0.5f, -0.5f, 0.5f);

		for (int i = 0; i < 8; ++i)
		{
			vertexes[i].color = Vector4(0.0f, 1.0f, 0.0f, 1.0f); // 초록색
			vertexes[i].uv = Vector2(0.0f, 0.0f);
		}

		UINT lineIndices[] = {
		0, 1, 1, 2, 2, 3, 3, 0, // 앞면 사각형 테두리 4줄
		4, 5, 5, 6, 6, 7, 7, 4, // 뒷면 사각형 테두리 4줄
		0, 4, 1, 5, 2, 6, 3, 7  // 앞면과 뒷면을 이어주는 기둥 4줄
		};
		indices.assign(lineIndices, lineIndices + 24);

		D3D11_INPUT_ELEMENT_DESC inputLayoutDesces[3] = {};

		inputLayoutDesces[0].AlignedByteOffset = 0;
		inputLayoutDesces[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
		inputLayoutDesces[0].InputSlot = 0;
		inputLayoutDesces[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		inputLayoutDesces[0].SemanticName = "POSITION";
		inputLayoutDesces[0].SemanticIndex = 0;

		inputLayoutDesces[1].AlignedByteOffset = 12;
		inputLayoutDesces[1].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		inputLayoutDesces[1].InputSlot = 0;
		inputLayoutDesces[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		inputLayoutDesces[1].SemanticName = "COLOR";
		inputLayoutDesces[1].SemanticIndex = 0;

		inputLayoutDesces[2].AlignedByteOffset = 28;
		inputLayoutDesces[2].Format = DXGI_FORMAT_R32G32_FLOAT; 
		inputLayoutDesces[2].InputSlot = 0;
		inputLayoutDesces[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		inputLayoutDesces[2].SemanticName = "TEXCOORD";
		inputLayoutDesces[2].SemanticIndex = 0;


		graphics::Shader* wireShader = Resources::Find<graphics::Shader>(L"WireFrameShader").get();

		if (wireShader != nullptr)
		{
			mesh->SetVertexBufferParams(3, inputLayoutDesces, wireShader->GetVSBlob()->GetBufferPointer(), wireShader->GetVSBlob()->GetBufferSize());
		}

		mesh->CreateIB(indices);
		mesh->CreateVB(vertexes);

		ME::Resources::Insert(L"CubeMesh", mesh);
	}

	void LoadMeshes()
	{
		LoadTriangleMesh();
		LoadRectMesh();
		LoadCubeMesh();
	}

	void LoadShaders()
	{
		ME::Resources::Load<graphics::Shader>(L"TriangleShader", L"..\\Shaders_SOURCE\\Triangle");

		graphics::Shader* spriteShader = ME::Resources::Load<graphics::Shader>(L"SpriteDefaultShader", L"..\\Shaders_SOURCE\\SpriteDefault").get();
		spriteShader->SetBlendState(graphics::eBlendState::AlphaBlend);
		spriteShader->SetDepthStencilState(graphics::eDepthStencilState::DepthNone);
		spriteShader->SetRasterizerState(graphics::eRasterizerState::SolidNone);

		ME::Resources::Load<graphics::Shader>(L"WireFrameShader", L"..\\Shaders_SOURCE\\WireFrame");
		ME::Resources::Load<graphics::Shader>(L"ModelShader", L"..\\Shaders_SOURCE\\Model");
		ME::Resources::Load<graphics::Shader>(L"StaticModelShader", L"..\\Shaders_SOURCE\\StaticModel");

	}

	void LoadMaterials()
	{
		std::shared_ptr<Material> triangleMaterial = std::make_shared<Material>();
		ME::Resources::Insert(L"TriangleMaterial", triangleMaterial);

		std::shared_ptr<Material> spriteMaterial = std::make_shared<Material>();
		ME::Resources::Insert(L"SpriteMaterial", spriteMaterial);

		std::shared_ptr<Material> modelMaterial = std::make_shared<Material>();
		ME::Resources::Insert(L"ModelMaterial", modelMaterial);

		std::shared_ptr<Material> staticModelMaterial = std::make_shared<Material>();	
		ME::Resources::Insert(L"StaticModelMaterial", staticModelMaterial);

		spriteMaterial->SetShader(ME::Resources::Find <graphics::Shader>(L"SpriteDefaultShader"));
		triangleMaterial->SetShader(ME::Resources::Find <graphics::Shader>(L"TriangleShader"));
		modelMaterial->SetShader(ME::Resources::Find <graphics::Shader>(L"ModelShader"));
		staticModelMaterial->SetShader(ME::Resources::Find <graphics::Shader>(L"StaticModelShader"));
	}

	void LoadConstantBuffers()
	{
		constantBuffers[CBSLOT_TRANSFORM] = new ConstantBuffer(eCBType::Transform);
		constantBuffers[CBSLOT_TRANSFORM]->Create(sizeof(TransformCB));

		constantBuffers[CBSLOT_ANIMATION] = new ConstantBuffer(eCBType::Animation);
		constantBuffers[CBSLOT_ANIMATION]->Create(sizeof(AnimationCB));

		constantBuffers[CBSLOT_UI] = new ConstantBuffer(eCBType::UI);
		constantBuffers[CBSLOT_UI]->Create(sizeof(UICB));

	}

	void Initialize()
	{
		LoadStates();
		LoadShaders();
		LoadMeshes();
		LoadMaterials();
		LoadConstantBuffers();
	}

	void Release()
	{
		for (int i = 0; i < (UINT)eCBType::End; i++)
		{
			delete constantBuffers[i];
			constantBuffers[i] = nullptr;
		}
		
	}
}