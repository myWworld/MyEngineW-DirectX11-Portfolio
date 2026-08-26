#pragma once

#include "MEGraphicDevice_DX11.h"

namespace ME::graphics
{

	class IndexBuffer :public GpuBuffer
	{

	public:

		IndexBuffer();
		~IndexBuffer();

		bool Create(const std::vector<UINT>& indices);
		void Bind() const;

		UINT GetIndexCount() const { return mIndexCount;}

	private:

		UINT mIndexCount;


	};



}
