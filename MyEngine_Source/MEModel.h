#pragma once
#include "../MyEngine_Source/CommonInclude.h"
#include <assimp/scene.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <fbxsdk.h>
#include <fbxsdk/scene/geometry/fbxnode.h>

#include "METexture.h"
#include "MESkeleton.h"
#include "MEResource.h"

namespace ME
{

    class Mesh;
  
    class Model : public Resource
    {
    public:
        Model();
        ~Model();

        HRESULT Save(const std::wstring& path) override;
        HRESULT Load(const std::wstring& path) override; //추상함수의 함수를 상속받아야 하지만 모델에선 쓰이지 않음 -> ...Args를 사용한 대신 딜레마..
		HRESULT Load(const std::wstring& path, enums::eBoneProfile profile); //resource -> Load를 한 시점에서 이미 shared_ptr<Model>의 형태이므로 오버로딩을 사용 
    
        const std::vector<Mesh*>& GetMeshes() { return mMeshes; }
        const std::vector<graphics::Texture*>&GetTextures() { return mTextures; }
        Skeleton& GetSkeleton() { return mSkeleton; }

        enums::eModelType GetModelType() { return mModelType; }

    private:

        void ProcessNode(aiNode* node, const aiScene* scene);
        Mesh* ProcessMesh(aiMesh* mesh, const aiScene* scene);
        std::vector<std::shared_ptr<graphics::Texture>> LoadMaterialTextures(aiMaterial* mat, aiTextureType type, const std::wstring& typeName);

        std::unordered_map<std::string, math::Matrix> LoadPreRotationsWithFBXSDK(const std::wstring& wpath);


    

        math::Matrix ConvertAIMatrixToMatrix(aiMatrix4x4& mat);

    private:
        std::vector<Mesh*> mMeshes;
        std::vector<graphics::Texture *> mTextures;
        Skeleton mSkeleton;

        enums::eModelType mModelType;

        bool mbModelFixed = false;

     

    };

}