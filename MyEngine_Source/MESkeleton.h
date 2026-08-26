#pragma once

#include "CommonInclude.h"
#include "MEBone.h"
#include "MEResource.h"
#include "BoneMapManager.h"


#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>

namespace ME
{
	class Skeleton : public Resource
	{
	public:
		Skeleton();
		~Skeleton();

		std::vector<Bone> mBones;
		std::unordered_map<size_t, int> mBoneHashToIndexMap;
		std::vector<Matrix> mBonesTransform;
	
		size_t mRootBoneHash;

		enums::eModelType mModelType;

		std::unordered_set<int> visited;

		enums::eBoneProfile mBoneProfile;
		const std::unordered_map<std::string, std::string>* mBoneNameMap;

	public:


		HRESULT Save(const std::wstring& path) override;
		HRESULT Load(const std::wstring& path)override;

		void BuildSkeleton(aiNode* node);

		int GetBoneIndex(const std::string& name) const;
		int GetBoneIndex(const size_t& hash) const;
		int GetBoneIndexToMatchWithAnim(const std::string& name) const;

		Bone* GetBone(const std::wstring& boneName);

		void CalculateFinalTransform();
		void RegisterBone(aiNode* node, std::unordered_map<std::string, math::Matrix>& preRot);
		void RegisterSkinData(const aiScene* scene);

		void SetModelType( enums::eModelType type) { mModelType = type; }
		enums::eModelType GetModelType() { return mModelType; }

		std::string ExtractBoneName(const std::string& name) const;

		void SetBoneProfile(enums::eBoneProfile profile)
		{
			mBoneProfile = profile;
			mBoneNameMap = BoneMapManager::GetBoneMap(profile);
		}

		Bone* GetLeftHandTransform();
		Bone* GetRightHandTransform();

	public :
		Skeleton& operator=(const Skeleton& other)
		{
			if (this != &other)
			{
				mBones = other.mBones;
				mBoneHashToIndexMap = other.mBoneHashToIndexMap;
				mBonesTransform = other.mBonesTransform;
				mRootBoneHash = other.mRootBoneHash;
				mModelType = other.mModelType;
				mBoneProfile = other.mBoneProfile;
				mBoneNameMap = other.mBoneNameMap;

				for (int i = 0; i < mBones.size(); ++i)
				{
		
					if (other.mBones[i].mParent != nullptr)
					{
						int parentIdx = other.mBones[i].mParent->mIndex;
						mBones[i].mParent = &mBones[parentIdx];
					}
					else
					{
						mBones[i].mParent = nullptr;
					}

					mBones[i].mChildren.clear(); 
					for (Bone* child : other.mBones[i].mChildren)
					{
						int childIdx = child->mIndex;
						mBones[i].mChildren.push_back(&mBones[childIdx]);
					}
				}
			}
			return *this;
		}

	private:
		void updateBoneRecursive(int boneIndex, const math::Matrix& parentTransform);
		
		
		math::Matrix ConvertAIMatrixToMatrix(aiMatrix4x4& aiMat);
	};
}

