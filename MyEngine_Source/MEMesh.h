#pragma once

#include "MEResource.h"
#include "MEVertexBuffer.h"
#include "MEIndexBuffer.h"
#include "MEInputLayout.h"
#include "MEBone.h"


namespace ME
{
	class Material;
	class Mesh: public Resource
	{
	public:


		struct MeshData
		{
			MeshData();
			~MeshData();

			D3D11_PRIMITIVE_TOPOLOGY mTopology;
			std::vector<graphics::Vertex> vertices;
			std::vector<UINT> indices;
			std::vector<Bone> bones;

		};

		Mesh();
		Mesh(std::vector<graphics::Vertex> &vertices, std::vector<unsigned int>& indicies, std::vector<Bone>& bones);
		~Mesh();

		virtual HRESULT Save(const std::wstring& path)override;
		virtual HRESULT Load(const std::wstring& path) override;

		bool CreateVB(const std::vector<graphics::Vertex>& vertices);
		bool CreateIB(const std::vector<UINT>& indices);
		void SetVertexBufferParams(UINT vertexCount, D3D11_INPUT_ELEMENT_DESC* layout, const void* pShaderByteCodeWithInputSignature
			, SIZE_T BytecodeLength);
		void Bind();

		UINT GetIndexCount() const { return mIB.GetIndexCount(); }


		void SetMaterial(std::shared_ptr<Material> material) { mMaterial = material; }
		std::shared_ptr<Material> GetMaterial() { return mMaterial; }

		void SetSkinned(bool skinned) { mbIsSkinned = skinned; }
		bool IsSkinned() { return mbIsSkinned; }

		void SetBoundingBox(math::Vector3 min, math::Vector3 max)
		{
			mMinBounds = min;
			mMaxBounds = max;
		}

		math::Vector3 GetMinBounds() { return mMinBounds; }
		math::Vector3 GetMaxBounds() { return mMaxBounds; }

	private:

		graphics::InputLayout mInputLayout;
		graphics::VertexBuffer mVB;
		graphics::IndexBuffer mIB;

		std::shared_ptr<Material> mMaterial;

		bool mbIsSkinned;

		MeshData mData;

		math::Vector3 mMaxBounds;
		math::Vector3 mMinBounds;

	};

}
