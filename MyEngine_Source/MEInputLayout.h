#pragma once
#include "MEGraphicDevice_DX11.h"

namespace ME::graphics
{
	class InputLayout
	{
		
	public:
		
		InputLayout();
		~InputLayout();

		void CreateInputLayout(UINT vertexCount, D3D11_INPUT_ELEMENT_DESC* layout, const void* pShaderByteCodeWithInputSignature
			, SIZE_T BtyecodeLength);


		void Bind();

	private:

		Microsoft::WRL::ComPtr<ID3D11InputLayout> mInputLayout;

		

	};
}



