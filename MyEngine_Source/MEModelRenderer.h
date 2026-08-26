#pragma once

#include "CommonInclude.h"
#include "MEComponent.h"
#include "METexture.h"


namespace ME
{

    class Mesh;
    class Material;

    class ModelRenderer : public Component
    {
    public:

        ModelRenderer();
        ~ModelRenderer();

        void Initialize() override;
        void Update() override;
        void LateUpdate() override;
        void Render() override;
    

        void setModelMatrix(const Matrix& model);
        void SetMesh(const std::vector<Mesh*>& meshes_);

  

    private:
        void render();


    private:

        Matrix mModelMatrix;

        std::vector<Mesh*> mMeshes;
        
        bool mbHasEmbeddedTextures = false; // Flag to check if textures are embedded in the model
  
       
    };

}