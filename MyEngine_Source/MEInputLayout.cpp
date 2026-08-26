#include "MEInputLayout.h"


namespace ME::graphics
{
	InputLayout::InputLayout()
		:mInputLayout(nullptr)
	{
	}
	InputLayout::~InputLayout()
	{
	}
	void InputLayout::CreateInputLayout(UINT vertexCount, D3D11_INPUT_ELEMENT_DESC* layout, const void* pShaderByteCodeWithInputSignature, SIZE_T BtyecodeLength)
	{
		if (!(GetDevice()->CreateInputLayout(layout, vertexCount
			, pShaderByteCodeWithInputSignature
			, BtyecodeLength
			, mInputLayout.GetAddressOf())))
				assert(NULL && "Create input layout failed!");
	}
	void InputLayout::Bind()
	{
		if (mInputLayout)
			GetDevice()->BindInputLayout(mInputLayout.Get());
	}
}