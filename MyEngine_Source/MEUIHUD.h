#pragma once
#include "MEUIBase.h"
#include "METexture.h"
#include "CommonInclude.h"
#include "MEGameObject.h"

namespace ME
{
	class Material;
	class Mesh;

	class UIHUD :public UIBase
	{
	public:
		UIHUD();
		~UIHUD();


		void OnInit() override;
		void OnActive()override;
		void OnInActive()override;
		void OnUpdate()override;
		void OnLateUpdate()override;
		void OnRender()override;
		void OnClear()override;

	private:
		std::shared_ptr<graphics::Texture> mTexture;

		std::shared_ptr<Material> mMaterial;
		std::shared_ptr<Mesh> mMesh;
		std::shared_ptr<graphics::Texture> mHpFillTex;
		std::shared_ptr<graphics::Texture> mHpFrameTex;
	};

}

