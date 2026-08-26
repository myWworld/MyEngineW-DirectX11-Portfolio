#include "MEVertexBuffer.h"


namespace ME::graphics
{


	ME::graphics::VertexBuffer::VertexBuffer()
	{	
	}

	ME::graphics::VertexBuffer::~VertexBuffer()
	{
	}

	bool ME::graphics::VertexBuffer::Create(const std::vector<Vertex>& vertexes)
	{
		desc.ByteWidth = sizeof(Vertex) * (UINT)vertexes.size(); //96 * n 16btye단위
		desc.Usage = D3D11_USAGE_DEFAULT;//정점은 한번 등록후 내용 덮어쓸 일 없으므로 Dynamic X
		desc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_VERTEX_BUFFER;
		desc.CPUAccessFlags = 0;

		D3D11_SUBRESOURCE_DATA sub = {};
		sub.pSysMem = vertexes.data();

		if (!(GetDevice()->CreateBuffer(&desc, &sub, buffer.GetAddressOf())))
			assert(NULL /*"Create Vertex buffer failed!"*/);
		return true;
	
	}

	void ME::graphics::VertexBuffer::Bind() 
	{
		UINT offset = 0;
		UINT vertexSize = sizeof(Vertex);
		GetDevice()->BindVertexBuffer(0, 1, buffer.GetAddressOf(), &vertexSize, &offset);
	}

}

