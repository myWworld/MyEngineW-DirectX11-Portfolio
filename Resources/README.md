# Resources — Data Map

이 폴더에는 실행 Binary Asset이 아니라 Engine과 FSM 구성을 설명하는 JSON 데이터가 포함되어 있습니다.

[전체 Reviewer Guide로 이동](../docs/REVIEW_GUIDE.md)

| 파일 | 역할 | 연결 코드 |
|---|---|---|
| [`ResourceList.json`](./ResourceList.json) | Model, Texture, Animation 등 Resource 로딩 목록 | [`../MyEngine_Source/MEResources.h`](../MyEngine_Source/MEResources.h), [`../MyEngine_Source/MEModel.cpp`](../MyEngine_Source/MEModel.cpp) |
| [`BoneMap.json`](./BoneMap.json) | 모델 간 Bone 이름 차이를 공통 의미로 매핑 | [`../MyEngine_Source/BoneMapManager.h`](../MyEngine_Source/BoneMapManager.h), [`../MyEngine_Source/MESkeleton.cpp`](../MyEngine_Source/MESkeleton.cpp) |
| [`EnemyFSMJson.json`](./EnemyFSMJson.json) | Monster FSM의 State, Task, Decision과 전이 정의 | [`../MyEngine_Source/FSMFactory.h`](../MyEngine_Source/FSMFactory.h), [`../MyEngine_Source/FSMBrainCore.cpp`](../MyEngine_Source/FSMBrainCore.cpp) |

## 데이터 흐름

```text
ResourceList.json
→ Resource Cache
→ Model / Mesh / Material / Texture / Animation

BoneMap.json
→ BoneMapManager
→ 모델별 Skeleton 이름 차이 정규화

EnemyFSMJson.json
→ FSMFactory
→ FSM Runtime
→ Client Context 또는 Server Context에서 실행
```
