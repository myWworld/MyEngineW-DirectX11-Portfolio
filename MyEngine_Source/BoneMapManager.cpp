#include "BoneMapManager.h"

namespace ME
{
	std::array<std::unordered_map<std::string, std::string>, static_cast<size_t>(enums::eBoneProfile::End)> BoneMapManager::mProfileBoneMaps;
}