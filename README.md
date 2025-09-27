# j2_library (split CMake project)

- `Linux`와 `Windows MinGW`를 모두 지원하는 `C++` 라이브러리 템플릿입니다.  
- 라이브러리 전용 `CMake`(`j2_library/`)와 상위(루트) `CMake`(`./`)를 **분리 구성** 했습니다.

## 특징
- **분리형 CMake**
  - `j2_library/` : 라이브러리 정의/설치/패키징(`find_package`)
  - 루트 : 옵션/프리셋/예제/테스트 오케스트레이션
- **정적/공유 자동 처리** : `export` 매크로(`J2LIB_STATIC` / `J2LIB_BUILDING_DLL`)를 타깃 타입에 맞게 자동 전파
- **C++ 표준 옵션** : `-DJ2_LIBRARY_CXX_STANDARD=17|20|23` (기본 17)
- **경고/샌리타이저 모듈** (선택) : `cmake/warnings.cmake`, `cmake/sanitize.cmake`
- **패키징 지원** : `find_package(j2_library CONFIG REQUIRED)`로 재사용 가능
- **예제/테스트 포함** : `examples/hello`, `tests`(`GoogleTest FetchContent`)

## 요구 사항
- `CMake ≥ 3.20`
- (`Linux`) `GCC/Clang`
- (`Windows`) `MinGW`(예: `C:\Qt\Tools\mingw1310_64\bin` 가 `PATH`에 있어야 함)
- (권장) `Ninja` 빌드 툴

### 패키지 설치 

#### `Google C++`

- `MingW` + `vcpkg` : `vcpkg install gtest:x64-mingw-static`
- `Ubuntu/Debian` : `sudo apt-get install libgtest-dev`
- `Fedora/CentOS/RHEL` : `sudo yum install gtest-devel`
- `Arch Linux` : `sudo pacman -S gtest`
- `Visual Studio` 
   - `vcpkg install gtest:x64-windows`
   - `vcpkg install gtest:x64-windows-static` 


---

## 디렉터리 구조

```
your-repo/
├─ CMakeLists.txt          # 상위(전체) CMake
├─ cmake/
│  ├─ warnings.cmake
│  └─ sanitize.cmake
├─ examples/hello/
│  ├─ CMakeLists.txt
│  └─ main.cpp
├─ tests/
│  ├─ CMakeLists.txt
│  └─ test_basic.cpp
└─ j2_library/             # 라이브러리 전용 CMake
   ├─ CMakeLists.txt
   ├─ include/j2_library/
   │  ├─ export.hpp
   │  ├─ j2_library.hpp
   │  └─ version.hpp.in
   ├─ src/
   │  ├─ j2_library.cpp
   │  └─ version.cpp
   └─ cmake/j2_libraryConfig.cmake.in
```

---

## 빠른 시작

### 1) 프리셋으로 전체(루트) 프로젝트 빌드
> 루트에 있는 `CMakePresets.json` 사용

```bash
# Windows MinGW Debug (정적)
cmake --preset windows-mingw-debug
cmake --build --preset windows-mingw-debug -j
ctest --preset windows-mingw-debug

# Linux Release (정적)
cmake --preset linux-release
cmake --build --preset linux-release -j
ctest --preset linux-release
```

### 2) 라이브러리만 단독 빌드
```bash
# 수동
cmake -S j2_library -B build-lib -DCMAKE_BUILD_TYPE=Release
cmake --build build-lib -j

# 또는 프리셋
cmake --preset windows-mingw-lib-release   # Windows
cmake --preset linux-lib-release           # Linux
cmake --build --preset windows-mingw-lib-release -j
```

---

## 정적 vs 공유(DLL/.so)

- 전역 제어(권장): `-DBUILD_SHARED_LIBS=ON/OFF`
  - ON: 공유 라이브러리(DLL/.so), OFF: 정적(.a/.lib)
- 본 템플릿은 **타깃 타입**에 따라 자동으로 매크로를 전파합니다.
  - 정적 빌드: `J2LIB_STATIC` (PUBLIC 정의) → 헤더의 `J2LIB_API`는 공백 처리
  - 공유 빌드: `J2LIB_BUILDING_DLL` (PRIVATE 정의) → 빌드 중 라이브러리는 `dllexport`, 소비자는 `dllimport`

> 위 처리는 CMake 제너레이터 표현식으로 구현되어 있어, 정적/공유 전환 시 헤더 수정이 필요 없습니다.

---

## `C++` 표준 변경

```bash
# C++20으로 빌드
cmake -S . -B build -DJ2_LIBRARY_CXX_STANDARD=20
cmake --build build -j
```

---

## 설치 및 사용

### 설치

```bash
# Linux 예시
cmake --install build --prefix /usr/local

# Windows 예시
cmake --install build --prefix "C:/dev/j2_prefix"
```

### 다른 프로젝트에서 사용

```cmake
# CMakeLists.txt
find_package(j2_library CONFIG REQUIRED)
target_link_libraries(your_app PRIVATE j2_library::j2_library)
```

---

## 자주 쓰는 팁
- **클린/재설정**: 빌드 폴더를 삭제하고 `cmake --preset ...`을 다시 실행하면 캐시 문제를 줄일 수 있습니다.
- **Sanitizer**: `-DJ2_LIBRARY_ENABLE_SANITIZERS=ON` (`GCC`/`Clang`)
- **경고 강화**: `-DJ2_LIBRARY_ENABLE_WARNINGS=ON`
- **IDE 프리셋**: `Qt Creator`/`VS Code`/`CLion`/`VS 2022`는 `CMakePresets.json`을 인식합니다.

---

## `CMakePresets.json` 요약
- `configurePresets`: 소스/빌드 디렉터리, 제너레이터(`Ninja`), 기본 캐시 변수
- `buildPresets`: 어떤 `configure` 프리셋을 빌드할지 지정
- `testPresets`: `ctest` 실행 프리셋

- 루트에 아래와 같은 프리셋을 두는 것을 권장합니다.

```json
{
  "version": 6,
  "configurePresets": [
    { "name": "windows-mingw-debug",   "generator": "Ninja",
      "binaryDir": "${sourceDir}/build/${presetName}",
      "cacheVariables": { "CMAKE_BUILD_TYPE": "Debug", "BUILD_SHARED_LIBS": "OFF" } },
    { "name": "windows-mingw-release", "generator": "Ninja",
      "binaryDir": "${sourceDir}/build/${presetName}",
      "cacheVariables": { "CMAKE_BUILD_TYPE": "Release", "BUILD_SHARED_LIBS": "OFF" } },
    { "name": "linux-debug",           "generator": "Ninja",
      "binaryDir": "${sourceDir}/build/${presetName}",
      "cacheVariables": { "CMAKE_BUILD_TYPE": "Debug", "BUILD_SHARED_LIBS": "OFF" } },
    { "name": "linux-release",         "generator": "Ninja",
      "binaryDir": "${sourceDir}/build/${presetName}",
      "cacheVariables": { "CMAKE_BUILD_TYPE": "Release", "BUILD_SHARED_LIBS": "OFF" } },
    { "name": "windows-mingw-lib-release", "inherits": "windows-mingw-release",
      "sourceDir": "${sourceDir}/j2_library" },
    { "name": "linux-lib-release",          "inherits": "linux-release",
      "sourceDir": "${sourceDir}/j2_library" },
    { "name": "windows-mingw-release-shared", "inherits": "windows-mingw-release",
      "cacheVariables": { "BUILD_SHARED_LIBS": "ON" } },
    { "name": "linux-release-shared",          "inherits": "linux-release",
      "cacheVariables": { "BUILD_SHARED_LIBS": "ON" } }
  ],
  "buildPresets": [
    { "name": "windows-mingw-debug",          "configurePreset": "windows-mingw-debug" },
    { "name": "windows-mingw-release",        "configurePreset": "windows-mingw-release" },
    { "name": "linux-debug",                  "configurePreset": "linux-debug" },
    { "name": "linux-release",                "configurePreset": "linux-release" },
    { "name": "windows-mingw-lib-release",    "configurePreset": "windows-mingw-lib-release" },
    { "name": "linux-lib-release",            "configurePreset": "linux-lib-release" },
    { "name": "windows-mingw-release-shared", "configurePreset": "windows-mingw-release-shared" },
    { "name": "linux-release-shared",         "configurePreset": "linux-release-shared" }
  ],
  "testPresets": [
    { "name": "windows-mingw-debug", "configurePreset": "windows-mingw-debug" },
    { "name": "linux-debug",         "configurePreset": "linux-debug" },
    { "name": "windows-mingw-release", "configurePreset": "windows-mingw-release" },
    { "name": "linux-release",         "configurePreset": "linux-release" }
  ]
}
```

- Visual Studio 2022용 `CMakeSettings.json`
```json
{
  "version": 3,
  "cmakeMinimumRequired": {
    "major": 3,
    "minor": 23
  },
  "configurePresets": [
    {
      "name": "x64-debug",
      "generator": "Ninja",
      "binaryDir": "${sourceDir}/build/x64-debug",
      "cacheVariables": {
        "CMAKE_TOOLCHAIN_FILE": "C:/Users/j2dol/vcpkg/scripts/buildsystems/vcpkg.cmake",
        "CMAKE_BUILD_TYPE": "Debug"
      }
    }
  ]
}
```
   - `CMAKE_TOOLCHAIN_FILE` 에 `vcpkg.cmake` 파일 경로를 설정한다. 
