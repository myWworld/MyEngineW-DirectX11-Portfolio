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
				//  값(Value) 대신 포인터(Pointer) 주소를 넣어 캐스팅
				T* valuePtr = std::any_cast<T>(&itr->second);

				if (valuePtr != nullptr)
				{
					return *valuePtr; // 안전하게 역참조하여 반환
				}
			}

			// 키가 없거나 타입이 일치하지 않으면 기본값 반환
			return T();
		}

		bool HasKey(uint32_t key)
		{
			return mData.find(key) != mData.end();
		}
		
	};
}
