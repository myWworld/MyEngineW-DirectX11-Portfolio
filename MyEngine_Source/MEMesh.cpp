#include "MEMesh.h"


namespace ME
{

	Mesh::MeshData::MeshData()
		:mTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST)
		,vertices {}
		,indices {}
	{

	}

	Mesh::MeshData::~MeshData()
	{

	}
		
	Mesh::Mesh()
		:Resource(enums::eResourceType::Mesh)
	{
	}

	Mesh::Mesh(std::vector<graphics::Vertex>& vertices, std::vector<unsigned int>& indicies, std::vector<Bone>& bones)
		:Resource(enums::eResourceType::Mesh)
	{
		CreateVB(vertices);
		CreateIB(indicies);
		mData.bones = bones;
	}
	Mesh::~Mesh()
	{
	}
	HRESULT Mesh::Save(const std::wstring& path)
	{



		return S_OK;
	}
	HRESULT Mesh::Load(const std::wstring& path)
	{
		return S_OK;
	}
	bool Mesh::CreateVB(const std::vector<graphics::Vertex>& vertices)
	{
		mData.vertices = vertices;
		return mVB.Create(vertices);

	}
	bool Mesh::CreateIB(const std::vector<UINT>& indices)
	{
		mData.indices = indices;
		return mIB.Create(indices);
	}

	void Mesh::SetVertexBufferParams(UINT vertexCount, D3D11_INPUT_ELEMENT_DESC* layout, const void* pShaderByteCodeWithInputSignature
		, SIZE_T BytecodeLength)
	{
		mInputLayout.CreateInputLayout(vertexCount, layout, pShaderByteCodeWithInputSignature, BytecodeLength);
	}

	void Mesh::Bind()
	{

		mInputLayout.Bind();
		mVB.Bind();
		mIB.Bind();

		graphics::GetDevice()->BindPrimitiveTopology(mData.mTopology);
	}
}