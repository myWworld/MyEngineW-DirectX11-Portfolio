#pragma once
#include <unordered_map>
#include <string>
#include <any>

namespace ME
{
	class BlackBoard
	{

	public:

		std::unordered_map<uint32_t, std::any> mData;

	public:
		
		~BlackBoard() = default;

		template<typename T>
		void SetValue(uint32_t key, T data)
		{
			mData[key] = data;
		}


		template<typename T>
		T GetValue(uint32_t key)
		{
			auto itr = mData.find(key);

			if (itr != mData.end())
			{
				return std::any_cast<T>(itr->second);
			}

			return T();
		}

		bool HasKey(uint32_t key)
		{
			return mData.find(key) != mData.end();
		}
		
	};
}
