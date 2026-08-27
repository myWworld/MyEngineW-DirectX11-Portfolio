#pragma once
#include "MEComponent.h"
#include "MEEntity.h"
#include "METexture.h"
#include "MEMaterial.h"
#include "MEMesh.h"

namespace ME
{

	class SpriteRenderer : public Component
	{
	public:
		SpriteRenderer();
		~SpriteRenderer();

		void Initialize() override;
		void Update()  override;
		void LateUpdate()  override;
		void Render()  override;

		void SetSprite(std::shared_ptr<graphics::Texture> sprite) { mTextures = sprite; }
		void SetMaterial(std::shared_ptr<Material> material) { mMaterial = material; }

	private:



		std::shared_ptr<graphics::Texture> mTextures;
		std::shared_ptr<Material> mMaterial;
		std::shared_ptr<Mesh> mMesh;

	};

}
