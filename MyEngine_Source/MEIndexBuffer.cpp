#include "MEIndexBuffer.h"


namespace ME::graphics
{
	IndexBuffer::IndexBuffer()
		:mIndexCount(0)
	{
	}
	IndexBuffer::~IndexBuffer()
	{
	}
	bool IndexBuffer::Create(const std::vector<UINT>& indices)
	{
		mIndexCount = (UINT)indices.size();

		desc.ByteWidth = sizeof(UINT) * (UINT)indices.size();
		desc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_INDEX_BUFFER;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.CPUAccessFlags = 0;

		D3D11_SUBRESOURCE_DATA sub = {};
		sub.pSysMem = indices.data();

		if (!(GetDevice()->CreateBuffer(&desc, &sub, buffer.GetAddressOf())))
			assert(NULL, "Create Index Buffer failed!");

		return true;
	}
	void IndexBuffer::Bind() const
	{
		GetDevice()->BindIndexBuffer(buffer.Get(), DXGI_FORMAT_R32_UINT, 0);
	}
}