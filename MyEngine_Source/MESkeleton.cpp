#include "MESkeleton.h"
#include "MEAnimator3D.h"
#include "StringUtility.h"

namespace ME
{

	Skeleton::Skeleton()
		:Resource(enums::eResourceType::Skeleton)
		, mRootBoneHash(0)
		, mModelType(enums::eModelType::Static)
	{

	}
	Skeleton::~Skeleton()
	{

	}

	HRESULT Skeleton::Save(const std::wstring& path)
	{
		return E_NOTIMPL;
	}
	HRESULT Skeleton::Load(const std::wstring& path)
	{
		return E_NOTIMPL;
	}

	void Skeleton::BuildSkeleton(aiNode* node)//미리 등록해둔 뼈 데이터로 부모-자식 관계 만들기
	{
		std::string nodeName = node->mName.C_Str();
		size_t nodeHash = 0;


			nodeName = ExtractBoneName(nodeName);
			nodeHash = std::hash<std::string>{}(nodeName);
		

		if (mBoneHashToIndexMap.contains(nodeHash))
		{
			int boneIndex = mBoneHashToIndexMap[nodeHash];

			if (node->mParent)
			{
				std::string parentName = node->mParent->mName.C_Str();
				size_t parentHash = 0;

					parentName = ExtractBoneName(parentName);
					parentHash = std::hash<std::string>{}(parentName);
				

				if (mBoneHashToIndexMap.contains(parentHash))
				{
					int parentIndex = mBoneHashToIndexMap[parentHash];

					auto& children = mBones[parentIndex].mChildren;//자식 배열
					if (std::find(children.begin(), children.end(), &mBones[boneIndex]) == children.end())
					{
						mBones[boneIndex].mParent = &mBones[parentIndex];
						children.push_back(&mBones[boneIndex]);
					}
				}
			}
		}


		for (unsigned int i = 0; i < node->mNumChildren; i++)
		{
			BuildSkeleton(node->mChildren[i]);
		}
	}


	

	void Skeleton::RegisterBone(aiNode* node, std::unordered_map<std::string, math::Matrix>& preRot)//스켈레톤의 뼈배열 등록 및 해시맵에 등록
	{
		std::string nodeName = node->mName.C_Str();
		size_t nodeHash = 0;

		bool bSkiped = false;
		std::string pureName = ExtractBoneName(nodeName);

		if (pureName == "RootNode")
		{
			nodeHash = std::hash<std::string>{}(pureName);
			mRootBoneHash = nodeHash;
		}
		else
		{
													// 네임스페이스 제거
			nodeHash = std::hash<std::string>{}(pureName);    // 순수 이름 해싱
		}

		if (mBoneHashToIndexMap.find(nodeHash) == mBoneHashToIndexMap.end() && bSkiped == false)
		{
			int idx = static_cast<int>(mBones.size());
			Bone newBone;
			newBone.mName = pureName;
		
			auto it = preRot.find(nodeName);
			Matrix pureRotation = Matrix::Identity;
			if (it != preRot.end())
				pureRotation = it->second;

			mBonesTransform.push_back(pureRotation);

			Matrix local = ConvertAIMatrixToMatrix(node->mTransformation);


			newBone.mLocalTransform = local;
			newBone.mDefaultLocalTransform = local;

			mBoneHashToIndexMap[nodeHash] = idx;
			newBone.mIndex = idx;
			mBones.push_back(newBone);
		}

		for (unsigned int i = 0; i < node->mNumChildren; i++)
		{
			RegisterBone(node->mChildren[i],preRot);
		}



	}

	void Skeleton::RegisterSkinData(const aiScene* scene)//역행렬 등록
	{
		for (unsigned int i = 0; i < scene->mNumMeshes; i++)
		{
			aiMesh* mesh = scene->mMeshes[i];

			for (unsigned int j = 0; j < mesh->mNumBones; j++)
			{
				aiBone* bone = mesh->mBones[j];
				std::string boneName = bone->mName.C_Str();
				
				boneName = ExtractBoneName(boneName);
				size_t boneHash = std::hash<std::string>{}(boneName);

				auto iter = mBoneHashToIndexMap.find(boneHash);

	
				if (iter != mBoneHashToIndexMap.end())
				{
					int boneIndex = iter->second;				
					mBones[boneIndex].mOffsetMatrix = ConvertAIMatrixToMatrix(bone->mOffsetMatrix);
				}
				else
				{
					std::cerr << "Bone not found in skeleton(SkinData): " << boneName << std::endl;
				}
			}
		}
	}

	int Skeleton::GetBoneIndexToMatchWithAnim(const std::string& name) const //애니메이션 bone 을 모델 내의 bone과 매칭 시키기 위해 사용
	{
		std::string pureName = StringUtility::extractBoneName(name);
		size_t nameHash = std::hash<std::string>{}(pureName);

		auto it = mBoneHashToIndexMap.find(nameHash);


		if (it != mBoneHashToIndexMap.end())
		{
			return it->second;
		}
		else
		{
			auto iter = mBoneNameMap->find(pureName);

			if (iter != mBoneNameMap->end())
			{
				std::string newName = iter->second;

				size_t newNameHash = std::hash<std::string>{}(newName);

				auto it2 = mBoneHashToIndexMap.find(newNameHash);

				if (it2 != mBoneHashToIndexMap.end())
				{
					return it2->second;
				}
			}
		}
		return -1;
	}

	int Skeleton::GetBoneIndex(const size_t& hash) const
	{
		auto it = mBoneHashToIndexMap.find(hash);


		if (it != mBoneHashToIndexMap.end())
		{
			return it->second;
		}

			return -1;

	
	}

	int Skeleton::GetBoneIndex(const std::string& name) const
	{
		size_t nameHash = std::hash<std::string>{}(name);
		int idx = GetBoneIndex(nameHash);

		if (idx != -1)
		{
			return idx;
		}
		else
		{
			auto iter = BoneNameManualMapping.find(name);

			if (iter != BoneNameManualMapping.end())
			{
				std::string newName = iter->second;
				size_t newNameHash = std::hash<std::string>{}(newName);

				auto it2 = mBoneHashToIndexMap.find(newNameHash);

				if (it2 != mBoneHashToIndexMap.end())
				{
					return it2->second;
				}
			}
		}

		return -1;
	}

	Bone* Skeleton::GetBone(const std::wstring& boneName)
	{

		std::string strName = StringUtility::ws2s(boneName);

		int index = GetBoneIndex(strName);

		if (index != -1 && index < mBones.size())
		{
			return &mBones[index]; 
		}

		return nullptr; 
	}

	Bone* Skeleton::GetLeftHandTransform()
	{
		static const size_t leftHandHash = std::hash<std::string>{}("LeftHand");
		auto it = mBoneHashToIndexMap.find(leftHandHash);

		if (it == mBoneHashToIndexMap.end())
			return nullptr;

		return &mBones[it->second];
	
	}

	Bone* Skeleton::GetRightHandTransform()
	{
		static const size_t rightHandHash = std::hash<std::string>{}("RightHand");
		auto it = mBoneHashToIndexMap.find(rightHandHash);

		if (it == mBoneHashToIndexMap.end())
			return nullptr;

		return &mBones[it->second];
	}


	void Skeleton::CalculateFinalTransform()
	{
		// 배열 순서가 이미 '부모 -> 자식' 순서이므로, 
		// i번째 뼈를 계산할 때 부모(mParent)의 mWolrdTransform은 무조건 계산이 끝난 상태가 보장
		for (int i = 0; i < mBones.size(); ++i)
		{
			Bone& bone = mBones[i];

			if (bone.mParent != nullptr)
			{
				// 부모가 있다면, 나의 로컬 * 부모의 월드(이미 계산됨)
				bone.mWolrdTransform = bone.mLocalTransform * bone.mParent->mWolrdTransform;
			}
			else
			{
				bone.mWolrdTransform = bone.mLocalTransform;
			}

			// 셰이더로 보낼 최종 행렬 조립 (역 바인드 포즈 * 현재 월드)
			bone.FinalTransform = bone.mOffsetMatrix * bone.mWolrdTransform;
		}
		//int rootIdx = GetBoneIndex(mRootBoneHash);

		//if (rootIdx == -1)
		//{
		//	return;
		//	//assert(false && "Root bone not found in skeleton.");
		//}


		//updateBoneRecursive(rootIdx, math::Matrix::Identity);
	}

	void Skeleton::updateBoneRecursive(int boneIndex, const math::Matrix& parentTransform)
	{
		
		Bone& bone = mBones[boneIndex];

		math::Matrix globalMatrix;

		globalMatrix = bone.mLocalTransform * parentTransform;

		bone.mWolrdTransform = globalMatrix;

		bone.FinalTransform = bone.mOffsetMatrix * globalMatrix;

		for (auto child : bone.mChildren)
		{
			updateBoneRecursive(child->mIndex, globalMatrix); 
		}
	}

	std::string Skeleton::ExtractBoneName(const std::string& name) const
	{
		return ME::StringUtility::extractBoneName(name);
	}


	math::Matrix Skeleton::ConvertAIMatrixToMatrix(aiMatrix4x4& aiMat)
	{
		math::Matrix mat;

		mat._11 = aiMat.a1; mat._12 = aiMat.b1; mat._13 = aiMat.c1; mat._14 = aiMat.d1;
		mat._21 = aiMat.a2; mat._22 = aiMat.b2; mat._23 = aiMat.c2; mat._24 = aiMat.d2;
		mat._31 = aiMat.a3; mat._32 = aiMat.b3; mat._33 = aiMat.c3; mat._34 = aiMat.d3;
		mat._41 = aiMat.a4; mat._42 = aiMat.b4; mat._43 = aiMat.c4; mat._44 = aiMat.d4;

		return mat;
	}
}