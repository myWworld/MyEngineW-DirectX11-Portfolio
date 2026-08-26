#pragma once
#include "MEComponent.h"
#include "MEEntity.h"
#include "METexture.h"

namespace ME
{
	using namespace math;

	class TileMapRenderer : public Component
	{
	public:
		TileMapRenderer();
		~TileMapRenderer();

		void Initialize() override;
		void Update()  override;
		void LateUpdate()  override;
		void Render()  override;

		void SetTexture(graphics::Texture* texture) { mTexture = texture; }
		void SetSize(math::Vector2 size) { mSize = size; }

		Vector2 GetIndex() const{ return mIndex; }
		void SetIndex(Vector2 index) { mIndex = index; }

	public:
		static Vector2 TileSize;
		static Vector2 OriginTileSize;
		static Vector2 SelectedIndex;

	private:

		Vector2 mTileSize;

		graphics::Texture* mTexture;
		math::Vector2 mSize;

		Vector2 mIndex;


	};

}