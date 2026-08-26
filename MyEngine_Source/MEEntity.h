 #pragma once
#include "CommonInclude.h"

using namespace ME::math;


namespace ME
{
	class Entity
	{
	public:
		Entity();
		virtual ~Entity();

		void SetName(const std::wstring& name) { mName = name; }
		const std::wstring& GetName() const { return mName; }
		

	private:
		std::wstring mName;
	};
}

