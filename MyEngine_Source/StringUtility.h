#pragma once
#include <string>

namespace ME
{
	class StringUtility
	{
	public:
		StringUtility() = delete;

		static std::wstring s2ws(const std::string& str)
		{
			if (str.empty())
				return std::wstring();

			// 변환 후 완성될 문자열의 길이를 먼저 알아냄
			int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), NULL, 0);

			//  알아낸 길이만큼 wstring에 미리 공간을 할당
			std::wstring wstrTo(size_needed, 0);

			//실제 문자열 변환을 수행하여 wstrTo에 채워 넣기
			MultiByteToWideChar(CP_UTF8, 0, &str[0], (int)str.size(), &wstrTo[0], size_needed);

			return wstrTo;
		}

		static std::string ws2s(const std::wstring& wstr)
		{
			if (wstr.empty())
				return std::string();

			// 변환 후 완성될 문자열의 길이를 알아냄
			int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);

			//공간 할당
			std::string strTo(size_needed, 0);

			// 실제 변환 수행
			WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);

			return strTo;
		}

		static std::string extractBoneName(const std::string& name)
		{
			std::string newName = name;
			size_t pos = newName.find_last_of("/|:");
			if (pos != std::string::npos)
			{
				newName = newName.substr(pos + 1);
			}
			size_t assimpFbxPos = newName.find("$AssimpFbx$");
			if (assimpFbxPos != std::string::npos)
			{
				newName = newName.substr(0, assimpFbxPos - 1);
			}
			return newName;
		}

		static constexpr uint32_t HashStr(const wchar_t* str)
		{
			uint32_t hash = 2166136261u;

			for (size_t i = 0; str[i] != L'\0'; ++i)
			{
				hash ^= static_cast<uint32_t>(str[i]);
				hash *= 16777619u;
			}

			return hash;
		}
	};
}