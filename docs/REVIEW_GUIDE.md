# Reviewer Guide

이 문서는 모든 구현 파일을 읽지 않고도 프로젝트의 구조, 책임 경계, 핵심 설계 판단을 확인할 수 있도록 만든 코드 리뷰용 안내서입니다.

## 프로젝트 구성

```text
MyEngineW-DirectX11-Portfolio/
├─ myEngineforStudy/       Win32 실행 진입점과 Window Application 프로젝트
├─ MyEngine_W/             Client Gameplay·Scene·Presentation 코드
├─ MyEngine_Source/        공통 Engine Core와 Client Network/FSM
├─ GameServer/             TCP Dedicated Server와 60Hz World Simulation
├─ Shaders_SOURCE/         HLSL Shader와 공통 Shader Include
├─ Resources/              Resource/Bone/FSM JSON 데이터
├─ MYEngine_Window/        정적 라이브러리 프로젝트 메타데이터
├─ LICENCES/               라이선스 원문
├─ THIRD_PARTY_NOTICES.md  외부 라이브러리 고지
└─ myEngineforStudyDX.sln  Visual Studio Solution
```

- 실행 Host인 `myEngineforStudy`는 `MyEngine_W` 프로젝트를 참조합니다.
- `MyEngine_W`는 `MyEngine_Source`와 `Shaders_SOURCE`의 Shared Items를 사용합니다.
- `GameServer`는 서버 전용 코드와 함께 데이터 독립적인 FSM Core·Task·Decision 일부를 직접 재사용합니다.

## 검토 시간별 추천 경로

### 3분: 전체 구조만 확인

1. 루트 README의 **빠른 코드 리뷰 가이드**
2. [`MyEngine_Source/README.md`](../MyEngine_Source/README.md)
3. [`GameServer/README.md`](../GameServer/README.md)

### 10분: 엔진 아키텍처

1. [`MEApplication.cpp`](../MyEngine_Source/MEApplication.cpp) — 프레임 순서와 Service 호출
2. [`MESceneManager.cpp`](../MyEngine_Source/MESceneManager.cpp) — Active/Persistent Scene 관리
3. [`MEScenes.cpp`](../MyEngine_Source/MEScenes.cpp) — Scene이 Layer를 갱신하는 흐름
4. [`MEGameObject.h`](../MyEngine_Source/MEGameObject.h) — Component 조합 구조
5. [`MEComponent.h`](../MyEngine_Source/MEComponent.h) — Component 공통 계약

### 10분: 렌더링과 애니메이션

1. [`MEGraphicDevice_DX11.cpp`](../MyEngine_Source/MEGraphicDevice_DX11.cpp) — Device, Context, SwapChain
2. [`MERenderer.cpp`](../MyEngine_Source/MERenderer.cpp) — Render Pass와 공통 렌더 상태
3. [`MEModel.cpp`](../MyEngine_Source/MEModel.cpp) — Model과 Mesh/Material 데이터 연결
4. [`MESkeleton.cpp`](../MyEngine_Source/MESkeleton.cpp) — Bone 계층과 스켈레톤
5. [`MEAnimator3D.cpp`](../MyEngine_Source/MEAnimator3D.cpp) — 애니메이션 상태·이벤트 실행
6. [`ModelVS.hlsl`](../Shaders_SOURCE/ModelVS.hlsl) / [`ModelPS.hlsl`](../Shaders_SOURCE/ModelPS.hlsl)

### 10분: 네트워크와 서버

1. [`Protocol.h`](../MyEngine_Source/Protocol.h) — C_/S_ 패킷과 식별자
2. [`MENetworkManager.cpp`](../MyEngine_Source/MENetworkManager.cpp) — Client TCP Framing과 Packet Queue
3. [`GameServer/main.cpp`](../GameServer/main.cpp) — Accept, Per-Client Recv Thread, Decode
4. [`ServerWorld.cpp`](../GameServer/ServerWorld.cpp) — Command 소비와 60Hz Tick 순서
5. [`ServerCombatSystem.cpp`](../GameServer/ServerCombatSystem.cpp) — 서버 전투 판정
6. [`ServerWorldReplicator.cpp`](../GameServer/ServerWorldReplicator.cpp) — 결과 패킷 생성·복제

### 7분: Client Gameplay와 표현

1. [`MEPlayerScript.cpp`](../MyEngine_W/MEPlayerScript.cpp)
2. [`MEWeaponScript.h`](../MyEngine_W/MEWeaponScript.h)
3. [`MEGunScript.cpp`](../MyEngine_W/MEGunScript.cpp) / [`MESwordScript.cpp`](../MyEngine_W/MESwordScript.cpp)
4. [`MERemotePlayerScript.cpp`](../MyEngine_W/MERemotePlayerScript.cpp)
5. [`MERemoteMonsterScript.cpp`](../MyEngine_W/MERemoteMonsterScript.cpp)
6. [`MEProjectileVisualManager.cpp`](../MyEngine_W/MEProjectileVisualManager.cpp)

### 7분: FSM 공유 구조

1. [`FSMBrainCore.cpp`](../MyEngine_Source/FSMBrainCore.cpp)
2. [`FSMFactory.h`](../MyEngine_Source/FSMFactory.h)
3. [`IFSMContext.h`](../MyEngine_Source/IFSMContext.h)
4. [`MEClientFSMContext.cpp`](../MyEngine_Source/MEClientFSMContext.cpp)
5. [`MEServerMonsterFSMContext.cpp`](../GameServer/MEServerMonsterFSMContext.cpp)
6. [`EnemyFSMJson.json`](../Resources/EnemyFSMJson.json)

## 핵심 설계 판단

| 판단 | 구현 목적 | 대표 코드 |
|---|---|---|
| Recv Thread와 상태 변경 Thread 분리 | 네트워크 I/O가 Scene/World를 직접 수정하지 않도록 경계 설정 | [`MENetworkManager.cpp`](../MyEngine_Source/MENetworkManager.cpp), [`ServerWorld.cpp`](../GameServer/ServerWorld.cpp) |
| TCP Stream Framing | `recv()` 1회와 Packet 1개가 일치하지 않는 문제 처리 | [`MENetworkManager.cpp`](../MyEngine_Source/MENetworkManager.cpp), [`main.cpp`](../GameServer/main.cpp) |
| ServerWorld 단일 상태 소유 Thread | Player·Monster·Projectile 상태에 대한 복잡한 다중 Lock 축소 | [`ServerWorld.cpp`](../GameServer/ServerWorld.cpp), [`ServerWorldState.h`](../GameServer/ServerWorldState.h) |
| 전투 결과 서버 확정 | Client 충돌 결과가 아니라 서버 Sweep과 HP 상태로 결과 결정 | [`ServerCombatSystem.cpp`](../GameServer/ServerCombatSystem.cpp) |
| FSM Context 추상화 | 동일한 FSM Core를 Client Scene과 Server State에서 재사용 | [`IFSMContext.h`](../MyEngine_Source/IFSMContext.h), [`FSMBrainCore.cpp`](../MyEngine_Source/FSMBrainCore.cpp) |
| 연속 충돌 검사 | 빠른 투사체가 프레임 사이를 통과하는 문제를 Segment Sweep으로 보완 | [`ServerMath.cpp`](../GameServer/ServerMath.cpp), [`ServerCombatSystem.cpp`](../GameServer/ServerCombatSystem.cpp) |

## 전체 실행 흐름

### Client Frame

```text
Input / Time
→ Collision / UI / Scene Update
→ NetworkManager::Update
→ LateUpdate
→ Clear / Bind / Scene Render / UI Render
→ Present
→ Destroy 예약 개체 정리
```

### Client 수신

```text
Recv Thread
→ pendingBuffer에 Byte 누적
→ PacketHeader.size 기준으로 완성 Packet 분리
→ Packet Queue
→ Client Main Thread의 NetworkManager::Update
→ MEClientPacketHandler
→ Remote Object / Projectile Visual / UI 반영
```

### Server 수신과 Tick

```text
Accept Loop
→ Per-Client Recv Thread
→ TCP Framing / Decode
→ WorldCommand Queue
→ ServerWorld 60Hz Single Thread
→ Player / Combat / Monster System
→ ServerWorldState 변경
→ ServerWorldReplicator
→ PacketUtility
```

## 읽지 않아도 전체 구조 파악에 지장이 적은 항목

첫 검토에서는 다음 항목을 건너뛰어도 됩니다.

- 개별 Buffer·Texture Wrapper의 반복적인 생성/해제 코드
- 각 FSM Task/Decision의 짧은 파생 구현 전부
- 프로젝트 파일과 `.filters`
- 실험용 `myEngineforStudy` 보조 데이터
- 라이선스 원문과 외부 라이브러리 소스

대신 각 폴더 README의 **추천 검토 순서**와 **대표 파일**만 확인하면 전체 책임 구조를 파악할 수 있습니다.

## 현재 프로토타입의 경계

- 공격 쿨다운, 투사체, 근접 Sweep, HP, 사망은 서버가 확정합니다.
- 이동 위치는 현재 Client 보고값을 서버가 유한성 중심으로 검사한 뒤 복제합니다.
- 동기화 보간, 지연 보상, Client Prediction/Reconciliation은 후속 확장 영역입니다.
- TCP 연결은 학습과 구조 검증을 위한 프로토타입이며 대규모 상용 서버를 목표로 한 구현은 아닙니다.
