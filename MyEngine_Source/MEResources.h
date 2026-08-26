#pragma once
#include "MEResource.h"
#include "json.hpp"
#include "StringUtility.h"
#include <memory>

using json = nlohmann::json;

namespace ME
{
	class Model;
	class Resources
	{
	public:


		
		template <typename T>
		static std::shared_ptr<T> Find(const std::wstring& key)
		{
			auto iter = mResources.find(key);
		
			if (iter == mResources.end())
				return nullptr;

			return std::dynamic_pointer_cast<T>(iter->second);
		}
	


		template <typename T, typename ...Args> //기존 템플릿 함수에서 확장성을 위해 여러 인자도 받을 수 있게 함
		static std::shared_ptr<T> Load(const std::wstring& key, const std::wstring &path, Args&& ...args)
		{
			std::shared_ptr<T> resource = Resources::Find<T>(key);
			if (resource != nullptr)
				return resource;

			resource = std::make_shared<T>();

			if (FAILED(resource->Load(path, std::forward<Args>(args)...))) //forward를 통해 인자를 그대로 전달, Load함수에서 여러 인자를 받을 수 있도록 확장
			{
				std::wstring warnMsg = L"[리소스 로드 실패] 파일을 찾을 수 없습니다: " + path + L"\n";
				OutputDebugStringW(warnMsg.c_str()); // 출력 창에 빨간불

				return nullptr;
			}

			resource->SetName(key);
			resource->SetPath(path);
			mResources.emplace(key,resource );
			
			return resource;
		}

		static void Insert(const std::wstring& key, std::shared_ptr<Resource> resource)
		{
			if (key == L"")
				return;
			if (resource == nullptr)
				return;

			mResources.insert({ key, resource });
		}

		static void Release()
		{
			mResources.clear();
		}

		template<typename T>
		static void LoadFromJSON(const json& data, const std::string& category)
		{
			if (!data.contains(category)) return;

			for (const auto& item : data[category])
			{

				if (!item.contains("key") || !item.contains("path"))
				{
					assert(false && "Invalid JSON format for resource loading.");
					continue;
				}

				std::wstring key = StringUtility::s2ws(item["key"].get<std::string>());
				std::wstring path = StringUtility::s2ws(item["path"].get<std::string>());

				if constexpr (std::is_same_v<T, Model>) //모델 뼈 매핑을 json으로 하는 Data_Driven방식을 위해서 컴파일 시간에 미리 Model인지 확인하고, 뼈 매핑을 위한 프로필을 포함하여 로드
				{

					enums::eBoneProfile profile = enums::eBoneProfile::None;

					if (item.contains("profile"))
					{
						std::string profStr = item["profile"].get<std::string>();
						if (profStr == "Humanoid") profile = enums::eBoneProfile::Humanoid;
						else if (profStr == "Quadruped") profile = enums::eBoneProfile::Quadruped;
						else if (profStr == "None") profile = enums::eBoneProfile::None;
						else
						{
							assert(false && "Invalid bone profile in JSON.");
						}
					}

					// 프로필을 포함하여 로드
					Load<T>(key, path, profile);
				}
				else
				{
					// 텍스처, 애니메이션 등은 경로만으로 로드
					Load<T>(key, path);
				}
			}
		}
		
	private:

		static std::unordered_map<std::wstring, std::shared_ptr<Resource>> mResources;
	};
}
