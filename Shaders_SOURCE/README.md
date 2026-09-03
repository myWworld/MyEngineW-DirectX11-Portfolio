# Shaders_SOURCE — HLSL Map

`Shaders_SOURCE`는 Engine Renderer가 사용하는 HLSL Shader와 공통 Include를 모아 둔 Shared Items 프로젝트입니다.

[Engine Core로 이동](../MyEngine_Source/README.md) · [전체 Reviewer Guide로 이동](../docs/REVIEW_GUIDE.md)

## 추천 검토 순서

| 순서 | 파일 | 역할 |
|---:|---|---|
| 1 | [`ConstantBuffers.hlsli`](./ConstantBuffers.hlsli) | Transform, Animation, UI 등 공통 Constant Buffer 정의 |
| 2 | [`ModelVS.hlsl`](./ModelVS.hlsl) | Skinned Model Vertex 변환 |
| 3 | [`ModelPS.hlsl`](./ModelPS.hlsl) | Model Texture와 Lighting 계산 |
| 4 | [`StaticModelVS.hlsl`](./StaticModelVS.hlsl) | Static Model Vertex 변환 |
| 5 | [`StaticModelPS.hlsl`](./StaticModelPS.hlsl) | Static Model Pixel 출력 |
| 6 | [`SpriteDefaultVS.hlsl`](./SpriteDefaultVS.hlsl) / [`SpriteDefaultPS.hlsl`](./SpriteDefaultPS.hlsl) | Sprite·UI 기본 렌더링 |

## 파일 분류

| 분류 | 파일 |
|---|---|
| 공통 Buffer | [`ConstantBuffers.hlsli`](./ConstantBuffers.hlsli) |
| Texture/Sampler | [`Textures.hlsli`](./Textures.hlsli), [`Samplers.hlsli`](./Samplers.hlsli) |
| Skinned Model | [`ModelVS.hlsl`](./ModelVS.hlsl), [`ModelPS.hlsl`](./ModelPS.hlsl) |
| Static Model | [`StaticModelVS.hlsl`](./StaticModelVS.hlsl), [`StaticModelPS.hlsl`](./StaticModelPS.hlsl) |
| Sprite/UI | [`SpriteDefaultVS.hlsl`](./SpriteDefaultVS.hlsl), [`SpriteDefaultPS.hlsl`](./SpriteDefaultPS.hlsl) |
| Debug | [`WireFrameVS.hlsl`](./WireFrameVS.hlsl), [`WireFramePS.hlsl`](./WireFramePS.hlsl) |
| 최소 Rendering 예제 | [`TriangleVS.hlsl`](./TriangleVS.hlsl), [`TrianglePS.hlsl`](./TrianglePS.hlsl) |

C++ 측 Shader·Buffer 연결은 다음 파일에서 확인할 수 있습니다.

- [`../MyEngine_Source/MEShader.cpp`](../MyEngine_Source/MEShader.cpp)
- [`../MyEngine_Source/MEInputLayout.cpp`](../MyEngine_Source/MEInputLayout.cpp)
- [`../MyEngine_Source/MEConstantBuffer.cpp`](../MyEngine_Source/MEConstantBuffer.cpp)
- [`../MyEngine_Source/MERenderer.cpp`](../MyEngine_Source/MERenderer.cpp)
