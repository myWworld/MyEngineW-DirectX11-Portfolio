#include "MEModel.h"
#include "MEGraphics.h"
#include "MEMesh.h"
#include "MEMaterial.h"
#include "MERenderer.h"
#include "MEBone.h"
#include "MEResources.h"
#include <float.h>
#include <fstream>


namespace ME
{
    using namespace graphics;
    using namespace math;

    Model::Model()
        :Resource(enums::eResourceType::Model),mModelType(enums::eModelType::Static)
    {}

    Model::~Model()
    {
        for (auto mesh : mMeshes)
        {
            if (mesh == nullptr)
                continue;
            delete mesh;
	        mesh = nullptr;
        }
    }

    HRESULT Model::Load(const std::wstring& path, enums::eBoneProfile profile)
    {
        Assimp::Importer importer;
        std::string path_str = std::string(path.begin(), path.end());
        importer.SetPropertyBool(AI_CONFIG_IMPORT_FBX_PRESERVE_PIVOTS, false);

        auto preRot = LoadPreRotationsWithFBXSDK(path);

        const aiScene* scene = importer.ReadFile(path_str,
            aiProcess_Triangulate |
            aiProcess_ConvertToLeftHanded |
            aiProcess_CalcTangentSpace |
            aiProcess_FlipUVs |
            aiProcess_RemoveRedundantMaterials | //  쓰레기 재질 제거
            //  잘못된 데이터 자동 제거
  //  aiProcess_OptimizeMeshes |           //  메쉬 최적화
            aiProcess_ImproveCacheLocality |   //  GPU 캐시 최적화
            // aiProcess_MakeLeftHanded |
            aiProcess_JoinIdenticalVertices |
            //aiProcess_FlipWindingOrder | //  좌표계 변환  
            aiProcessPreset_TargetRealtime_Quality
        );

        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
        {
            std::string str = importer.GetErrorString();

            return E_FAIL;
        }


        std::wstring directory = path.substr(0, path.find_last_of('/') + 1);

        mSkeleton.RegisterBone(scene->mRootNode, preRot);
        mSkeleton.BuildSkeleton(scene->mRootNode);
        mSkeleton.SetBoneProfile(profile);
        ProcessNode(scene->mRootNode, scene);


        if (mModelType == enums::eModelType::SkinnedMesh)
        {
            mSkeleton.RegisterSkinData(scene);

            //std::ofstream modelFile("SkeletonData2.txt", std::ios::out);

            //if (modelFile.is_open())
            //{
            //	modelFile << "--- 모델 뼈 리스트 ---" << std::endl;
            //    for (const auto& bone : mSkeleton.mBones)
            //    {

            //        modelFile << bone.mName << std::endl;

            //    }
            //}

        }


        return S_OK;

    }
    HRESULT Model::Load(const std::wstring& path)
    {
		return Load(path, enums::eBoneProfile::None);
    }

    HRESULT Model::Save(const std::wstring& path)
    {
		return E_NOTIMPL;
    }

    void Model::ProcessNode(aiNode* node, const aiScene* scene)
    {
        bool hasSkinnedBone = false;


        for (unsigned int i = 0; i < node->mNumMeshes; i++)
        {
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];

            std::string meshName = mesh->mName.C_Str();      

            Mesh* mMesh = ProcessMesh(mesh, scene);

            if (mesh->mNumBones > 0)
            {
                hasSkinnedBone = true;
                renderer::LoadModels(mMesh);
                mMesh->SetSkinned(true);
            }
            else if (mesh->mNumBones == 0)
            {   
                renderer::LoadStaticModels(mMesh);   
                mMesh->SetSkinned(false);
            }
            
            mMeshes.push_back(mMesh);
        }

        for (unsigned int i = 0; i < node->mNumChildren; i++)
        {
            ProcessNode(node->mChildren[i], scene);
        }


        if(hasSkinnedBone && mbModelFixed == false)
		{
            mbModelFixed = true;
			mSkeleton.SetModelType(enums::eModelType::SkinnedMesh);
			mModelType = enums::eModelType::SkinnedMesh;
		}
		else if(hasSkinnedBone == false && mbModelFixed == false)
		{
			mSkeleton.SetModelType(enums::eModelType::Static);
			mModelType = enums::eModelType::Static;
		}
    }
    Mesh* Model::ProcessMesh(aiMesh* mesh, const aiScene* scene)
    {
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
      
        std::vector<Bone> bones; 

        Vector3 minBounds(FLT_MAX, FLT_MAX, FLT_MAX);
        Vector3 maxBounds(-FLT_MAX, -FLT_MAX, -FLT_MAX);

        for (unsigned int i = 0; i < mesh->mNumVertices; i++)
        {
            Vertex vertex;
            vertex.pos = Vector3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);


            if (vertex.pos.x < minBounds.x) minBounds.x = vertex.pos.x;
            if (vertex.pos.y < minBounds.y) minBounds.y = vertex.pos.y;
            if (vertex.pos.z < minBounds.z) minBounds.z = vertex.pos.z;

            if (vertex.pos.x > maxBounds.x) maxBounds.x = vertex.pos.x;
            if (vertex.pos.y > maxBounds.y) maxBounds.y = vertex.pos.y;
            if (vertex.pos.z > maxBounds.z) maxBounds.z = vertex.pos.z;


            if (mesh->HasNormals())
            {
                vertex.normal = Vector3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
            }

            if (mesh->HasTangentsAndBitangents())
            {
				vertex.tangent = Vector3(mesh->mTangents[i].x, mesh->mTangents[i].y, mesh->mTangents[i].z);
            }
            else //임시용
            {
                
                vertex.tangent = Vector3(1.0f, 0.0f, 0.0f);
            }

            if (mesh->HasTextureCoords(0))
            {
                vertex.uv = Vector2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
            }

            if(mesh->HasVertexColors(0))
			{
                aiColor4D c = mesh->mColors[0][i];
				vertex.color = Vector4(c.r, c.g, c.b, c.a);
			}
			else
			{
				vertex.color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
			}

         
            vertices.push_back(vertex);

        }

        for (unsigned int i = 0; i < mesh->mNumBones; i++)
        {
            aiBone* bone = mesh->mBones[i];
            std::string boneName = bone->mName.C_Str();
           boneName = mSkeleton.ExtractBoneName(boneName);

            int boneIndex = mSkeleton.GetBoneIndex(boneName);
            if (boneIndex == -1)
            {
                assert(false && "뼈 인덱스 매칭 실패, 메쉬의 뼈가 스켈레톤 트리에 없다 ");
                continue;
            }
            
            Bone newBone(boneName,boneIndex, math::Matrix::Identity);
            newBone.mOffsetMatrix = ConvertAIMatrixToMatrix(bone->mOffsetMatrix);
            bones.push_back(newBone);

            for (unsigned int j = 0; j < bone->mNumWeights; j++)
            {
                unsigned int vertexID = bone->mWeights[j].mVertexId;
                float weight = bone->mWeights[j].mWeight;

                vertices[vertexID].AddBoneData(boneIndex, weight);
                
            }
   
        }

        for (auto& vertex : vertices)
        {
            vertex.NormalizeBoneWeights();
        }

        for (unsigned int i = 0; i < mesh->mNumFaces; i++)
        {
            aiFace face = mesh->mFaces[i];

            for (unsigned int j = 0; j < face.mNumIndices; j++)
            {
                indices.push_back(face.mIndices[j]);
            }
        }

        Mesh* newMesh = new Mesh(vertices, indices,bones);
        newMesh->SetBoundingBox(minBounds, maxBounds);

        if (mesh->mMaterialIndex >= 0)
        {
           aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

		   std::shared_ptr<Material> newMaterial = std::make_shared<Material>();

           if (mesh->mNumBones > 0)
           {
               newMaterial->SetShader(Resources::Find<graphics::Shader>(L"ModelShader"));
           }
           else
           {
               newMaterial->SetShader(Resources::Find<graphics::Shader>(L"StaticModelShader"));
           }

           auto diffuseMaps = LoadMaterialTextures(material, aiTextureType_DIFFUSE, L"texture_diffuse");
           if (!diffuseMaps.empty())
               newMaterial->SetAlbedoTexture(diffuseMaps[0]);

           auto normalMaps = LoadMaterialTextures(material, aiTextureType_NORMALS, L"texture_normal");
           if (!normalMaps.empty())
               newMaterial->SetNormalTexture(normalMaps[0]);

  
           auto specularMaps = LoadMaterialTextures(material, aiTextureType_SPECULAR, L"texture_specular");
           if (!specularMaps.empty())
               newMaterial->SetSpecularTexture(specularMaps[0]);

           auto smoothMaps = LoadMaterialTextures(material, aiTextureType_SHININESS, L"texture_smoothness");
           if (!smoothMaps.empty())
               newMaterial->SetRoughnessTexture(smoothMaps[0]);

		   newMesh->SetMaterial(newMaterial);
        }

     
        return newMesh;
    }

     std::vector<std::shared_ptr<graphics::Texture>> Model::LoadMaterialTextures(aiMaterial* mat, aiTextureType type, const std::wstring& typeName)
     {
         std::vector<std::shared_ptr<graphics::Texture>> textures;
         for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
         {
             aiString str;
             mat->GetTexture(type, i, &str);

             std::filesystem::path fullPathFromFBX(str.C_Str()); // FBX에 적힌 경로
             std::wstring fileName = fullPathFromFBX.filename().wstring(); // 경로 다 떼고 "M9_BaseColor.png"만 추출
           
             std::filesystem::path myEnginePath = L"..\\Resources\\Textures\\";
             myEnginePath.append(fileName);

             auto texture = Resources::Load<graphics::Texture>(fileName, myEnginePath.wstring());

             if (texture)
             {
                 textures.push_back(texture);
			 }
             
         }
         return textures;
    }

     std::unordered_map<std::string, math::Matrix> Model::LoadPreRotationsWithFBXSDK(const std::wstring& wpath)
     {
         std::unordered_map<std::string, math::Matrix> bonePreRotations;

         FbxManager* pManager = FbxManager::Create();
         FbxIOSettings* ios = FbxIOSettings::Create(pManager, IOSROOT);
         pManager->SetIOSettings(ios);

         FbxImporter* pImporter = FbxImporter::Create(pManager, "");
         std::string path = std::string(wpath.begin(), wpath.end());

         FbxScene* pScene = FbxScene::Create(pManager, "TempScene");

         if (!pImporter->Initialize(path.c_str(), -1, pManager->GetIOSettings()) || !pImporter->Import(pScene)) {
             std::cerr << "FBX SDK Load Error: " << pImporter->GetStatus().GetErrorString() << std::endl;
             return bonePreRotations;
         }

         pImporter->Destroy();

         std::function<void(FbxNode*)> Traverse;
         Traverse = [&](FbxNode* node) {

             std::string nodeName = node->GetName();
             
             FbxVector4 preRot = node->GetPreRotation(FbxNode::eSourcePivot);
             
             FbxAMatrix localMat = node->EvaluateLocalTransform();

             Matrix local;
             for (int row = 0; row < 4; ++row)
                 for (int col = 0; col < 4; ++col)
                     (&local._11)[row * 4 + col] = static_cast<float>(localMat.Get(row, col));

             local._13 = -local._13; // 3번째 열 부호 반전 (Z축 관련 회전)
             local._23 = -local._23;
             local._31 = -local._31; // 3번째 행 부호 반전 (Z축 관련 회전)
             local._32 = -local._32;
             local._34 = -local._34;
             local._43 = -local._43; // Z축 이동(Translation) 반전

             bonePreRotations[nodeName] = local;

             for (int i = 0; i < node->GetChildCount(); ++i)
                 Traverse(node->GetChild(i));
             };

         Traverse(pScene->GetRootNode());

         pManager->Destroy();
         return bonePreRotations;
     }

     math::Matrix Model::ConvertAIMatrixToMatrix(aiMatrix4x4& aiMat)
     {
      
         math::Matrix mat;

         mat._11 = aiMat.a1; mat._12 = aiMat.b1; mat._13 = aiMat.c1; mat._14 = aiMat.d1;
         mat._21 = aiMat.a2; mat._22 = aiMat.b2; mat._23 = aiMat.c2; mat._24 = aiMat.d2;
         mat._31 = aiMat.a3; mat._32 = aiMat.b3; mat._33 = aiMat.c3; mat._34 = aiMat.d3;
         mat._41 = aiMat.a4; mat._42 = aiMat.b4; mat._43 = aiMat.c4; mat._44 = aiMat.d4;

         return mat;
     }
}