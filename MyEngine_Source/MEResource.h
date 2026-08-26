#pragma once
#include "MEEntity.h"


namespace ME
{


	class Resource abstract: public Entity // 메모리 할당 X 오직 상속받아서 사용
	{
	public:
	
		Resource(enums::eResourceType type);
		virtual~Resource();

		virtual HRESULT Save(const std::wstring& path) = 0;
		virtual HRESULT Load(const std::wstring& path) = 0;
		
		const std::wstring& GetPath(){ return mPath; }
		void SetPath(const std::wstring& path) { mPath = path; }

	private:

		const enums::eResourceType mType;
		std::wstring mPath;
	};

}
