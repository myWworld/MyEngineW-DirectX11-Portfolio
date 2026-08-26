#pragma once
#include "CommonInclude.h"
#include "MEGameObject.h"

namespace ME
{
	template<typename T>
	class ObjectPool
	{
	public:
		std::queue <T*> mPool;
		std::function<T* ()> mInstantiateFunc;

		ObjectPool(size_t poolSize, std::function<T* ()> instantiateFunc)
			: mInstantiateFunc(instantiateFunc)
		{
			for (size_t i = 0; i < poolSize; i++)
			{
				T* obj = mInstantiateFunc();

				if (obj == nullptr) continue;

				obj->SetActive(false);
				mPool.push(obj);
			}
		}

		~ObjectPool()
		{
			while (!mPool.empty())
			{
				mPool.pop();
			}

			mInstantiateFunc = nullptr;
		}

		T* Get()
		{
			T* obj = nullptr;

			if (!mPool.empty())
			{
				obj = mPool.front();
				mPool.pop();
				
			}
			else
			{
				obj = mInstantiateFunc();
			}

			if(obj)
				obj->SetActive(true);

			return obj;
		}

		void Return(T* obj)
		{
			if (obj)
			{
				obj->SetActive(false);
				mPool.push(obj);
			}
		}
	};

}
