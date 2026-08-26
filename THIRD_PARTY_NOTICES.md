# Third-Party Notices

이 문서는 `myWworld/MyEngineW-DirectX11` 공개 포트폴리오 저장소에서 사용하거나 기반으로 삼은 외부 SDK·라이브러리와 그 적용 범위를 정리합니다.

> **적용 전 확인**  
> 이 문서는 공개용 정리본을 기준으로 작성되었습니다. `External/Include/Fmod`, `External/Include/fbksdk`, Assimp 전체 원본 복사본, 출처·재배포 조건을 확인하지 못한 `Resources` 원본 에셋은 공개 저장소에서 제거한 뒤 사용해야 합니다.

저장소 소유자가 직접 작성한 엔진·게임플레이·서버 코드는 별도 표기가 없는 한 저장소 소유자의 저작물입니다. 아래 구성요소는 각 원저작자의 라이선스와 이용약관을 따릅니다.

---

## 1. DirectX Tool Kit — SimpleMath

- **원저작자:** Microsoft Corporation
- **라이선스:** MIT License
- **공식 프로젝트:** https://github.com/microsoft/DirectXTK
- **저장소 내 적용 파일:**
  - `MyEngine_Source/MEMath.h`
  - `MyEngine_Source/MEMath.cpp`
- **사용 형태:** DirectXTK의 `SimpleMath` 구현을 기반으로 `ME::math` 네임스페이스, 엔진 타입 및 프로젝트 요구사항에 맞게 수정·통합했습니다.
- **변경 사항의 범위:** 네임스페이스 변경, 엔진용 상수·보조 연산 추가, 프로젝트 코드와의 통합을 위한 선언 및 구현 조정.
- **라이선스 전문:** [`LICENSES/DirectXTK-LICENSE.txt`](LICENSES/DirectXTK-LICENSE.txt)

`MEMath.h`와 `MEMath.cpp`에는 Microsoft 저작권 및 MIT 라이선스 기반 코드임을 알리는 헤더를 유지해야 합니다.

---

## 2. DirectXTex

- **원저작자:** Microsoft Corporation
- **라이선스:** MIT License
- **공식 프로젝트:** https://github.com/microsoft/DirectXTex
- **저장소 내 연동 파일:**
  - `MyEngine_Source/METexture.h`
  - `MyEngine_Source/METexture.cpp`
- **사용 목적:** DDS, TGA 및 WIC 기반 이미지 로딩과 Direct3D 11 Shader Resource View 생성.
- **배포 정책:** DirectXTex 원본 소스나 빌드 산출물은 공개 포트폴리오 저장소에 복사하지 않고, vcpkg 또는 별도 설치를 통해 의존성을 제공하는 것을 기준으로 합니다.
- **라이선스 전문:** [`LICENSES/DirectXTex-LICENSE.txt`](LICENSES/DirectXTex-LICENSE.txt)

---

## 3. JSON for Modern C++ — nlohmann/json 3.12.0

- **원저작자:** Niels Lohmann
- **라이선스:** MIT License
- **공식 프로젝트:** https://github.com/nlohmann/json
- **저장소 내 파일:**
  - `MyEngine_Source/json.hpp`
- **확인된 버전:** 3.12.0
- **사용 목적:** JSON 기반 FSM 상태·전이·Task·Decision 데이터 파싱.
- **배포 정책:** 단일 헤더 파일을 저장소에 포함할 수 있으며, 파일의 SPDX 저작권·라이선스 표기와 MIT 라이선스 전문을 유지합니다.
- **라이선스 전문:** [`LICENSES/nlohmann-json-LICENSE.MIT`](LICENSES/nlohmann-json-LICENSE.MIT)

---

## 4. Open Asset Import Library — Assimp

- **원저작자:** Assimp Team 및 각 기여자
- **라이선스:** Modified BSD 3-Clause License
- **공식 프로젝트:** https://github.com/assimp/assimp
- **저장소 내 사용 목적:** FBX 및 기타 3D 모델 데이터의 메시·머티리얼·본·애니메이션 정보 로딩.
- **배포 정책:** 공개 포트폴리오 저장소에는 Assimp 원본 저장소 전체를 복사하지 않고, vcpkg 또는 Git submodule로 의존성을 가져오는 것을 기준으로 합니다.
- **중요 예외:** Assimp의 `test/models-nonbsd`는 Assimp 본체의 BSD 라이선스만으로 일괄 재배포할 수 있는 폴더가 아닙니다. 각 테스트 모델의 별도 출처와 조건이 적용되므로 공개 포트폴리오 저장소에서 제외합니다.
- **라이선스 전문:** [`LICENSES/assimp-LICENSE.txt`](LICENSES/assimp-LICENSE.txt)

---

## 5. FMOD Engine API

- **원저작자:** Firelight Technologies Pty Ltd.
- **구성요소 성격:** 독점 SDK 및 런타임 오디오 엔진
- **공식 법적 고지:** https://www.fmod.com/legal
- **공식 다운로드:** https://www.fmod.com/download
- **저장소 내 연동 파일:**
  - `MyEngine_Source/MEFmod.h`
  - `MyEngine_Source/MEFmod.cpp`
  - `MyEngine_Source/MEAudioClip.*`
  - `MyEngine_Source/MEAudioSource.*`
  - `MyEngine_Source/MEAudioListener.*`
- **확인된 API 헤더 버전:** `FMOD_VERSION 0x00020214` — FMOD 2.02.20 계열
- **사용 목적:** FMOD Studio/Core System 초기화, 3D 사운드 생성·재생, Listener 갱신.
- **배포 정책:**
  - FMOD SDK 헤더, import library, 예제, 문서 및 설치 파일은 이 공개 저장소에 포함하지 않습니다.
  - 빌드 사용자는 FMOD 공식 사이트에서 SDK를 별도로 내려받아 로컬 경로를 설정해야 합니다.
  - FMOD 런타임 라이브러리의 제품 배포는 적용되는 FMOD EULA와 프로젝트 라이선스 조건을 따라야 합니다.
  - 실제 제품 배포 시 FMOD가 요구하는 크레딧 및 라이선스 등록 조건을 별도로 확인해야 합니다.

FMOD는 오픈소스 라이선스 구성요소가 아니므로 이 저장소의 `LICENSES` 폴더에 SDK EULA 전문을 복제하지 않습니다.

---

## 6. Autodesk FBX SDK 2020.3.7

- **원저작자:** Autodesk, Inc.
- **구성요소 성격:** Autodesk EULA가 적용되는 독점 SDK
- **공식 안내 및 다운로드:** https://aps.autodesk.com/developer/overview/fbx-sdk
- **확인된 버전:** 2020.3.7
- **저장소 내 사용 목적:** FBX scene, mesh, material, skeleton 및 animation 데이터의 import/export 처리.
- **배포 정책:**
  - FBX SDK의 `include`, `lib`, 설치 파일 및 SDK 패키지는 이 공개 저장소에 포함하지 않습니다.
  - 빌드 사용자는 Autodesk 공식 배포본을 별도로 설치하고 로컬 SDK 경로를 설정해야 합니다.
  - Autodesk SDK 파일의 사용과 배포는 다운로드 시 동의한 라이선스 계약을 따릅니다.

Autodesk FBX SDK는 오픈소스 라이선스 구성요소가 아니므로 이 저장소의 `LICENSES` 폴더에 SDK EULA 전문을 복제하지 않습니다.

---

## 7. Windows SDK 및 DirectX 플랫폼 구성요소

다음 구성요소는 Windows SDK 또는 Microsoft 플랫폼 SDK를 통해 참조하며, 해당 SDK 파일을 저장소에 복사하지 않습니다.

- Direct3D 11 / DXGI / HLSL compiler interfaces
- DirectXMath / DirectXCollision / DirectXPackedVector
- Win32 API / GDI / GDI+
- WinSock2 / WS2_32
- DirectInput
- Windows Imaging Component

이 저장소에는 위 SDK의 원본 헤더나 라이브러리를 재배포하지 않으며, Windows SDK가 설치된 개발 환경에서 빌드합니다.

---

## 8. 외부 모델·텍스처·애니메이션·오디오 에셋

공개 포트폴리오 저장소에는 다음 원본 에셋을 포함하지 않는 것을 원칙으로 합니다.

- 제3자 캐릭터·몬스터·무기 모델의 FBX/OBJ 파일
- 제3자 애니메이션 FBX 파일
- 출처·라이선스가 명시되지 않은 Texture, Material, Audio 파일
- Assimp 테스트 모델 및 샘플 에셋
- FMOD 예제 미디어

시연 영상에는 외부 에셋이 표시될 수 있지만, 해당 에셋은 기능 구현 시연을 위한 것이며 저장소 소유자가 에셋 저작권을 보유한다고 주장하지 않습니다. 공개 소스에는 원본 에셋을 재배포하지 않습니다.

직접 작성한 데이터 파일은 별도로 유지할 수 있습니다.

- `Resources/BoneMap.json`
- `Resources/ResourceList.json`
- `Resources/EnemyFSMJson.json`

위 파일도 실제로 직접 작성한 내용인지 최종 확인한 뒤 공개해야 합니다.

---

## 9. 공개 저장소에서 제외해야 하는 경로

다음 경로는 포트폴리오 공개본에 포함하지 않습니다.

```text
External/Include/Fmod/
External/Include/fbksdk/
External/Library/Fmod/
External/Library/fbksdk/
External/Include/assimp/test/
External/Include/assimp/samples/
External/Include/assimp/tools/
Resources/Animation/
Resources/**/*.fbx
Resources/**/*.FBX
Resources/**/*.obj
Resources/**/*.mtl
Resources/**/*.png
Resources/**/*.jpg
Resources/**/*.jpeg
Resources/**/*.tga
Resources/**/*.dds
Resources/**/*.wav
Resources/**/*.mp3
Resources/**/*.ogg
```

Assimp를 vcpkg 또는 submodule로 전환한 경우 `External/Include/assimp/` 전체를 제외합니다.

---

## 10. 라이선스 파일 목록

```text
LICENSES/
├─ DirectXTK-LICENSE.txt
├─ DirectXTex-LICENSE.txt
├─ nlohmann-json-LICENSE.MIT
└─ assimp-LICENSE.txt
```

각 파일은 해당 프로젝트가 제공한 라이선스 전문을 보존한 것입니다.

---

## 11. 면책

이 문서는 저장소의 제3자 구성요소와 공개 범위를 정리하기 위한 실무 문서이며 법률 자문이 아닙니다. 라이브러리 버전, 사용 방식, 제품 배포 형태가 변경되면 각 원저작자의 최신 라이선스 및 이용약관을 다시 확인해야 합니다.
