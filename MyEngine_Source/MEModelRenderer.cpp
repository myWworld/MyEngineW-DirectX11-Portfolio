#include "MEModelRenderer.h"
#include "MESceneManager.h"
#include "MEMesh.h"
#include "MECamera.h"
#include "MERenderer.h"
#include "METransform.h"
#include "MEMaterial.h"
#include "MEFrustumCulling.h"
#include "MEResources.h"

namespace ME
{
ModelRenderer::ModelRenderer()
    :Component(enums::eComponentType::ModelRenderer)
{
	mMeshes.clear();
}
ModelRenderer::~ModelRenderer()
{

}

void ModelRenderer::Initialize()
{
}
void ModelRenderer::Update()
{

}
void ModelRenderer::LateUpdate()
{
   
}
void ModelRenderer::Render()
{
    render();
}

void ModelRenderer::setModelMatrix(const Matrix& model)
{
    mModelMatrix = model;
}
void ModelRenderer::SetMesh(const std::vector<Mesh*>& meshes_)
{
    mMeshes = meshes_;
}
void ModelRenderer::render()
{
 
    Transform* tr = GetOwner()->GetComponent<Transform>();
    if (tr)
        tr->Bind();

    if (mMeshes.empty())
        return;


    math::Matrix worldMatrix = tr->GetWorldMatrix();
    FrustumCulling* frustumCulling = renderer::mainCamera->GetFrustumCulling();

    // 하나의 루프로 통합
    for (auto mesh : mMeshes)
    {
        if (mesh == nullptr)
            continue;


        math::Vector3 localMinBounds = mesh->GetMinBounds();
        math::Vector3 localMaxBounds = mesh->GetMaxBounds();

        DirectX::BoundingBox localBox;
        DirectX::BoundingBox::CreateFromPoints(localBox, localMinBounds, localMaxBounds);

        DirectX::BoundingBox worldBox;
        localBox.Transform(worldBox, worldMatrix);

       if (!frustumCulling->CheckAABB(worldBox))
       {
           continue;
       }

        std::shared_ptr<Material> mat = mesh->GetMaterial();
        
        if (mat != nullptr)
        {
            mat->Bind(); // 이 안에서 셰이더(Skinned/Static)와 텍스처 통합
        }

        mesh->Bind();        
        graphics::GetDevice()->DrawIndexed(mesh->GetIndexCount(), 0, 0);
    }


}

}
