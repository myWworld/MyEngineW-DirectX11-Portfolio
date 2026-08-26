#pragma once

#include "MEGraphicDevice_DX11.h"

namespace ME::graphics
{

	class VertexBuffer :public GpuBuffer
	{

	public:

		VertexBuffer();
		~VertexBuffer();

		bool Create(const std::vector<Vertex>& vertexes);
		void Bind();

	private:
		

	};



}
