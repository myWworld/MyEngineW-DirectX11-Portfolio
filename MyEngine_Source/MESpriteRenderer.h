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

		void SetSprite(graphics::Texture* sprite) { mTextures = sprite; }
		void SetMaterial(Material* material) { mMaterial = material; }

	private:



		graphics::Texture* mTextures;
		Material* mMaterial;
		Mesh* mMesh;

	};

}
