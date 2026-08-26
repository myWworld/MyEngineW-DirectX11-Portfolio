#include "MEAnimation3D.h"
#include "METime.h"

#include "METransform.h"
#include "MEAnimator3D.h"
#include "MEGameObject.h"
#include "MERenderer.h"
#include "MECamera.h"
#include "MESceneManager.h"
#include "MESkeleton.h"
#include "StringUtility.h"

namespace ME
{

	Animation3D::Animation3D()
		:Resource(enums::eResourceType::Animation3D)
		,mSkeleton(nullptr)
		,mPrevRootPos(Vector3::Zero)
	{
	}

	Animation3D::~Animation3D()
	{
	}
	HRESULT Animation3D::Save(const std::wstring& path)
	{
		return E_NOTIMPL;
	}
	HRESULT Animation3D::Load(const std::wstring& path)
	{
	
		Assimp::Importer importer;
		std::string path_str = std::string(path.begin(), path.end());

	
		importer.SetPropertyBool(AI_CONFIG_IMPORT_FBX_PRESERVE_PIVOTS, false);

		const aiScene* animScene = importer.ReadFile(path_str,
			aiProcess_Triangulate |
			aiProcess_ConvertToLeftHanded |    
			aiProcess_CalcTangentSpace |
			aiProcess_FlipUVs |
			aiProcess_RemoveRedundantMaterials |
			aiProcess_ImproveCacheLocality |
			aiProcess_JoinIdenticalVertices |
			aiProcessPreset_TargetRealtime_Quality
		);

		if (!animScene || animScene->mNumAnimations == 0)
		{
			return E_FAIL;
		}

		// Assimp에서 애니메이션 파싱
		aiAnimation* anim = animScene->mAnimations[0];
		CreateFromAssimp(anim);

		// 경로 저장
		this->SetPath(path);

		return S_OK;
	}
	void Animation3D::Update()
	{



	}

	void Animation3D::Render()
	{
		
	}


	void Animation3D::UpdateAnimation(float currentTime, Skeleton* skeleton, Animator3D* animator)
	{


		if (skeleton == nullptr || animator == nullptr)
			return;

		float timeInTicks = currentTime * mTickersPerSecond;
		std::unordered_set<int> updatedBoneIndices;
		Vector3 currentRootPos = Vector3::Zero;


		for (auto& bones : boneAnimations) 
		{
		
			int boneindex = skeleton->GetBoneIndexToMatchWithAnim(bones.boneName);

			if (boneindex == -1)
				continue;

			math::Matrix localTransform = InterpolateLocalTransform(bones, timeInTicks, boneindex, skeleton, animator);
			skeleton->mBones[boneindex].mLocalTransform = localTransform;

			updatedBoneIndices.insert(boneindex);

			if (bones.boneName == "mixamorig:Hips" || bones.boneName == "Hips")
				currentRootPos = localTransform.Translation();
		}

		for (auto& bone : skeleton->mBones)
		{
			if (updatedBoneIndices.find(skeleton->GetBoneIndex(bone.mName)) == updatedBoneIndices.end())
			{
				bone.mLocalTransform = bone.mDefaultLocalTransform;
			}
		}

		Transform* tr = animator->GetOwner()->GetComponent<Transform>();

	/*	if (animator->GetApplyRootMotion())
		{

			Vector3 prevRootPos = animator->GetPrevRootPos();
			Vector3 delta = currentRootPos - prevRootPos;


			delta.y = 0;

			Vector3 currentPos = tr->GetPosition();
			tr->SetPosition(currentPos + delta);

	
			animator->SetPrevRootPos(currentRootPos);
		}*/


		skeleton->CalculateFinalTransform();
	}


	void Animation3D::CreateFromAssimp(aiAnimation* anim)
	{
	
		std::string name = anim->mName.C_Str();

		this->mName = std::wstring(name.begin(), name.end());
		this->mDuration = static_cast<float>(anim->mDuration);
		this->mTickersPerSecond = static_cast<float>(anim->mTicksPerSecond != 0 ? anim->mTicksPerSecond : 25.0f);


		for (unsigned int i = 0; i < anim->mNumChannels; i++)
		{
			aiNodeAnim* channel = anim->mChannels[i];
	
			BoneAnimation boneAnim;
			std::string boneName = channel->mNodeName.C_Str();
		
			boneName = extractBoneName(boneName);
			boneAnim.boneName = boneName;

			for (unsigned int pos = 0; pos < channel->mNumPositionKeys; pos++)
			{
				aiVectorKey aiPosKey = channel->mPositionKeys[pos];

				VectorKey key;
				key.time = static_cast<float>(aiPosKey.mTime);
				key.value = Vector3(aiPosKey.mValue.x, aiPosKey.mValue.y, aiPosKey.mValue.z);
				boneAnim.positions.push_back(key);
			}

			for (unsigned int pos = 0; pos < channel->mNumRotationKeys; pos++)
			{
				aiQuatKey aiQuatKey = channel->mRotationKeys[pos];

				QuatKey key;
				key.time = static_cast<float>(aiQuatKey.mTime);
				key.value = Quaternion(aiQuatKey.mValue.x, aiQuatKey.mValue.y, aiQuatKey.mValue.z, aiQuatKey.mValue.w);
				boneAnim.rotations.push_back(key);
			}

			for (unsigned int pos = 0; pos < channel->mNumScalingKeys; pos++)
			{
				aiVectorKey aiPosKey = channel->mScalingKeys[pos];

				VectorKey key;
				key.time = static_cast<float>(aiPosKey.mTime);
				key.value = Vector3(aiPosKey.mValue.x, aiPosKey.mValue.y, aiPosKey.mValue.z);
				boneAnim.scales.push_back(key);
			}

			this->boneAnimations.push_back(boneAnim);
		}

		//std::ofstream modelFile("AnimationSkeletonData2.txt", std::ios::out);

		//if (modelFile.is_open())
		//{
		//	modelFile << "--- 애니메이션 뼈 리스트 ---" << std::endl;
		//	for (const auto& bone : boneAnimations)
		//	{

		//		modelFile << bone.boneName << std::endl;

		//	}
		//}


	}

	std::string Animation3D::extractBoneName(std::string& name)
	{
		return ME::StringUtility::extractBoneName(name);
	}

	math::Matrix Animation3D::InterpolateLocalTransform(const BoneAnimation& boneAnim, float currentTime,int boneIdx, Skeleton* skeleton, Animator3D* animator)
	{
		// 위치(Position) 방어: 남의 뼈 길이 무시하고 내 뼈 길이 유지
		Vector3 interpolatedPos = InterpolatePosition(boneAnim.positions, currentTime);

		if (boneAnim.boneName != "Hips" && boneAnim.boneName != "mixamorig:Hips")
		{
			if (boneIdx != -1)
			{
				interpolatedPos = skeleton->mBones[boneIdx].mDefaultLocalTransform.Translation();
			}
		}
		else
		{
			if (!animator->GetApplyRootMotion())
				interpolatedPos = Vector3::Zero;
		}

		//크기(Scale) 방어: 무조건 1.0으로 고정해서 늘어나는 현상 차단
		Vector3 interpolatedScale = Vector3(1.0f, 1.0f, 1.0f);

		//  회전(Rotation) 계산
		Quaternion interpolatedRot = InterpolateRotation(boneAnim.rotations, currentTime);

		Matrix baseTransform = Matrix::Identity;
		if (boneIdx != -1)
		{
			baseTransform = skeleton->mBonesTransform[boneIdx];
		}

	
		Matrix transform = Matrix::CreateScale(interpolatedScale) * Matrix::CreateFromQuaternion(interpolatedRot);
		transform = transform * baseTransform;

		//회전이 다 끝난 행렬에, 마지막으로 위치값을 직접 꽂아 넣습니다
		transform._41 = interpolatedPos.x;
		transform._42 = interpolatedPos.y;
		transform._43 = interpolatedPos.z;

		return transform;
		//Vector3 interpolatedPos = InterpolatePosition(boneAnim.positions, currentTime);

		//if (boneAnim.boneName != "Hips" && boneAnim.boneName != "mixamorig:Hips")
		//{
		//	if (boneIdx != -1)
		//	{
		//		// 애니메이션 위치값 덮어쓰기 -> 내 뼈의 원래 위치(길이)
		//		interpolatedPos = skeleton->mBones[boneIdx].mDefaultLocalTransform.Translation();
		//	}
		//}
		//else
		//{
		//	// 골반(Hips) 루트모션 처리 
		//	if (!animator->GetApplyRootMotion())
		//		interpolatedPos = Vector3::Zero;
		//}

		//Quaternion interpolatedRot = InterpolateRotation(boneAnim.rotations, currentTime);

		////Quaternion finalRot = interpolatedRot;
		////
		////
		//Matrix baseTransform = Matrix::Identity;
		//
		//if (boneIdx != -1)
		//{
		//	 baseTransform = skeleton->mBonesTransform[boneIdx];
		//}
		////
		//Vector3 interpolatedScale = InterpolateScale(boneAnim.scales, currentTime);

		//Matrix transform =
		//	Matrix::CreateScale(interpolatedScale) *
		//	Matrix::CreateFromQuaternion(interpolatedRot) *
		//	Matrix::CreateTranslation(interpolatedPos);

		////

		//transform = transform * baseTransform;

		//return transform;
	}



	math::Vector3 Animation3D::InterpolatePosition(const std::vector<VectorKey>& pos, float currentTime)
	{
		if (pos.size() == 1)
			return pos[0].value;

		for (size_t i = 0; i < pos.size() - 1; i++)
		{
			if (currentTime < pos[i + 1].time)
			{
				float t = (currentTime - pos[i].time) / (pos[i + 1].time - pos[i].time);
				
				return Vector3::Lerp(pos[i].value, pos[i + 1].value, t);
			}
		}

		return pos.back().value;
	}
	math::Quaternion Animation3D::InterpolateRotation(const std::vector<QuatKey>& rotation, float currentTime)
	{
		if (rotation.size() == 1)
			return rotation[0].value;

		for (size_t i = 0; i < rotation.size() - 1; i++)
		{
			if (currentTime < rotation[i + 1].time)
			{
				float t = (currentTime - rotation[i].time) / (rotation[i + 1].time - rotation[i].time);

				return Quaternion::Slerp(rotation[i].value, rotation[i + 1].value, t);
			}
		}

		return rotation.back().value;
	}
	math::Vector3 Animation3D::InterpolateScale(const std::vector<VectorKey>& scale, float currentTime)
	{
		if (scale.size() == 1)
			return scale[0].value;

		for (size_t i = 0; i < scale.size() - 1; i++)
		{
			if (currentTime < scale[i + 1].time)
			{
				float t = (currentTime - scale[i].time) / (scale[i + 1].time - scale[i].time);

				return Vector3::Lerp(scale[i].value, scale[i + 1].value, t);
			}
		}

		return scale.back().value;
	}
}