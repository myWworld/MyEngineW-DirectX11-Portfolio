#pragma once

namespace ME::enums
{
	enum class eLayerType
	{
		None,
		BackGround,
		Block,
		Floor,
		Player,
		Items,
		Weapon,
		Bullet,
		Particle,
		Monster,
		Max = 20
	};

	enum class eModelType
	{
		Static,
		StaticBone,
		SkinnedMesh,
		End,
	};

	enum class eResourceType
	{
		Texture,
		Mesh,
		Material,
		Animation,
		Animation3D,
		Skeleton,
		Model,
		AudioClip,
		Shader,
		Prefab, //구현 x
		End
	};

	enum class eColliderType
	{
		Rect2D,
		Circle2D,
		Box3D,
		Sphere3D,
		Capsule3D,
		End,
	};

	enum class eUIType
	{
		HpBar,
		Score,
		Button,
		End,
	};

	enum class eComponentType
	{
		// 사용자 로직이 가장 먼저 실행되어 위치나 상태를 결정
		Script,
		FSMBrain,

		// 스크립트가 움직인 위치를 바탕으로 물리 연산(충돌, 중력)을 처리
		Rigidbody,
		Collider,

		//  결정된 위치를 바탕으로 애니메이션
		Animator,
		Animator3D,

		// 스크립트, 물리, 애니메이션이 모두 끝난 최종 데이터로 월드 행렬을 만들기
		Transform, 

		//만들어진 월드 행렬을 화면에 그리기
		SpriteRenderer,
		ModelRenderer,

		//  모든 물체가 제자리를 찾은 후, 마지막으로 카메라가 쫓아가기
		Camera,

		AudioListener,
		AudioSource,
		End,
	};

	enum class eBoneProfile : int
	{
		None = 0,
		Humanoid,      // 주인공, 인간형 적
		Quadruped,     // 4족 보행 몬스터
		End,          // 배열 크기 측정을 위한 더미값
	};
}