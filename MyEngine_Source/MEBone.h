#pragma once

#include "CommonInclude.h"

namespace ME
{
	class Bone
	{
	public:

		Bone() {};
		~Bone() = default;
		std::string mName;
		int mIndex;
		math::Matrix mOffsetMatrix;
		math::Matrix mLocalTransform;
		math::Matrix FinalTransform;
		math::Matrix mDefaultLocalTransform;
		math::Matrix mWolrdTransform;

		Bone* mParent;
		std::vector<Bone*> mChildren;

	public:

		Bone(std::string& name, int index, const math::Matrix& offset):
			mName(name),mIndex(index), mOffsetMatrix(offset), mLocalTransform(math::Matrix::Identity), mParent(nullptr)
		{}


	};

}