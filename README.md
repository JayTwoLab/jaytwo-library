## `j2_library` 라이브러리

<br />

---

### 0. `j2_library` 소개

- `j2_library`는 `C++` 표준을 기반으로 작성된 크로스 플랫폼 라이브러리입니다.
   - 현재 `C++17` 표준 사용
   - `Windows`, `Linux` 지원 (`MacOS` 지원 예정)
   - `cmake`, `ninja` 빌드 시스템 사용

- 홈페이지
   - https://github.com/JayTwoLab/jaytwo-library

<br />

---

### 1. 사전 준비

- [사전 설치 항목](Pre-Preparation.md)
   - `C++` 컴파일러 설치
   - `CMake` 설치
   - `ninja` 설치
- [사전 준비 사항](Pre-Installation.md)

<br />

---

### 2. 빌드

- 관련 소스 코드 모두 받기 권장
   - `git clone --recurse-submodules https://github.com/JayTwoLab/jaytwo-library.git`

- 써드 파티는 패키지를 직접 설치하여 사용할 수도 있음.
	- `Windows` : `vcpkg` 를 사용하여 설치
	- `Linux` : 패키지 관리자를 사용하여 설치 (vcpkg를 사용할 수도 있음)

- `third_party` 를 `git` `sub module` 로 받지 않고 빌드하는 경우, `CMake` 옵션에서 `-DJ2_THIRD_PARTY_ROOT_PATH=/user/hello/third-party` 등과 같이 경로를 지정하여 빌드할 수도 있음.

<br />

---

#### 2.1. 라이브러리 빌드하기 

##### 2.1.1. `Visual Studio` 에서 라이브러리 빌드 및 설치

```cmd
@REM 라이브러리 경로로 이동
cd j2_library

@REM 빌드 경로 생성
mkdir build 
cd build

@REM cmake 실행 (릴리즈 빌드, 설치 경로 지정)
cmake -S .. ^
 -B build ^
 -G "Visual Studio 17 2022" ^
 -A x64 ^
 -DCMAKE_BUILD_TYPE=Release ^
 -DCMAKE_INSTALL_PREFIX="C:/install/j2_library"
@REM -DBUILD_SHARED_LIBS=ON 옵션을 주면 DLL 로 빌드됨

@REM 라이브러리 빌드
cmake --build build --config Release

@REM 라이브러리 설치
cmake --install build --config Release
```

<br />

---

##### 2.1.2. `MinGW` (`g++`) 에서 라이브러리 빌드 및 설치

```cmd
@REM 라이브러리 경로로 이동
cd j2_library

@REM 빌드 경로 생성
mkdir build 
cd build

@REM cmake 실행 (릴리즈 빌드, 설치 경로 지정)
cmake -S .. ^
 -B build ^
 -G "MinGW Makefiles" ^
 -DCMAKE_BUILD_TYPE=Release ^
 -DCMAKE_INSTALL_PREFIX="C:/mingw-install/j2_library"
@REM -DBUILD_SHARED_LIBS=ON 옵션을 주면 DLL 로 빌드됨

@REM 라이브러리 빌드
cmake --build build

@REM 라이브러리 설치
cmake --install build
```

<br />

---

##### 2.1.3. `Linux` (`g++`)  에서 라이브러리 빌드 및 설치

```bash
# 라이브러리 경로로 이동
cd j2_library

# 빌드 경로 생성
mkdir build 
cd build

# cmake 실행 (릴리즈 빌드, 설치 경로 지정)
cmake -S .. \
 -B build \
 -DCMAKE_BUILD_TYPE=Release \
 -DCMAKE_INSTALL_PREFIX="/home/j2/workspace/dev/lib"
# -DBUILD_SHARED_LIBS=ON 옵션을 주면 so 로 빌드됨

# 라이브러리 빌드 (cpu 코어 수 만큼 병렬 빌드)
cmake --build build -j$(nproc)

# 라이브러리 설치 
cmake --install build
# 설치 경로가 /usr/local/... 등의 시스템 경로인 경우, 설치 시 sudo(root 권한) 필요함.
#  sudo cmake --install build
```

<br />

---
 
#### 2.2. 설치된 라이브러리 사용하기

---

#### 2.2.1. `Visual Studio` 에서 설치된 라이브러리 사용하기

```cmake
# CMakeLists.txt (MSVC용)
cmake_minimum_required(VERSION 3.26)
project(j2_app_msvc LANGUAGES CXX)

# 설치된 j2_library의 prefix 경로를 지정합니다. (예: C:/install/j2_library)
# 원하는 경로로 바꾸십시오.
set(j2_library_ROOT "C:/install/j2_library" CACHE PATH "Prefix of installed j2_library")

# find_package가 Config 파일을 찾을 수 있도록 CMAKE_PREFIX_PATH에 추가합니다.
list(APPEND CMAKE_PREFIX_PATH "${j2_library_ROOT}")

# 설치 시 생성된 Config 모드 패키지를 찾습니다.
find_package(j2_library REQUIRED CONFIG)

add_executable(j2_app main.cpp)
target_link_libraries(j2_app PRIVATE j2_library::j2_library)

# MSVC 환경에서는 구성별(Release/Debug) 빌드를 사용합니다.
set_property(TARGET j2_app PROPERTY CXX_STANDARD 17)
```

- 빌드 예:

```powershell
cmake -S . -B build -G "Visual Studio 17 2022" -A x64 -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release
```

- `CMakeSettings.json` 셋팅 (`MSVC` 권장)
	- `Visual Studio` IDE에서 빌드 구성을 쉽게 전환할 수 있도록 `CMakeSettings.json` 파일을 프로젝트 루트에 생성합니다.
	```json
	{
	  "configurations": [
		{
		  "name": "x64-Debug",
		  "generator": "Ninja",
		  "configurationType": "Debug",
		  "inheritEnvironments": [ "msvc_x64_x64" ],
		  "buildRoot": "${projectDir}\\out\\build\\${name}",
		  "installRoot": "${projectDir}\\out\\install\\${name}",
		  "cmakeCommandArgs": "",
		  "buildCommandArgs": "",
		  "ctestCommandArgs": ""
		},
		{
		  "name": "x64-Release",
		  "generator": "Ninja",
		  "configurationType": "Release",
		  "inheritEnvironments": [ "msvc_x64_x64" ],
		  "buildRoot": "${projectDir}\\out\\build\\${name}",
		  "installRoot": "${projectDir}\\out\\install\\${name}",
		  "cmakeCommandArgs": "",
		  "buildCommandArgs": "",
		  "ctestCommandArgs": ""
		}	
	  ]
	}
	```
	- `CMakeSettings.json` 파일은 `git` 등 버전 관리 시스템에 포함시키는 것이 좋습니다. (`.gitignore` 에 추가하여 관리)
	
<br />

---

#### 2.2.2. `MinGW`(`g++`) 에서 설치된 라이브러리 사용하기

```cmake
# CMakeLists.txt (MinGW용)
cmake_minimum_required(VERSION 3.26)
project(j2_app_mingw LANGUAGES CXX)

# 설치된 j2_library의 prefix 경로 (예: C:/mingw-install/j2_library)
set(j2_library_ROOT "C:/mingw-install/j2_library" CACHE PATH "Prefix of installed j2_library")

# find_package 경로 힌트
list(APPEND CMAKE_PREFIX_PATH "${j2_library_ROOT}")

find_package(j2_library REQUIRED CONFIG)

add_executable(j2_app main.cpp)
target_link_libraries(j2_app PRIVATE j2_library::j2_library)

set_property(TARGET j2_app PROPERTY CXX_STANDARD 17)
```

- 빌드 예:

```bash
cmake -S . -B build -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release
cmake --build build
```

<br />

---

#### 2.2.3. `Linux` (`g++`) 에서 설치된 라이브러리 사용하기

```cmake
# CMakeLists.txt (Linux g++용)
cmake_minimum_required(VERSION 3.26)
project(j2_app_linux LANGUAGES CXX)

# 설치된 j2_library의 prefix 경로 (예: /opt/j2_library 또는 /usr/local/j2_library 등)
set(j2_library_ROOT "/opt/j2_library" CACHE PATH "Prefix of installed j2_library")

# find_package 경로 힌트
list(APPEND CMAKE_PREFIX_PATH "${j2_library_ROOT}")

# 공유 라이브러리(rpath) 실행 편의: 런타임에 prefix/lib를 자동 탐색
# 공유로 설치하셨다면 유용합니다. (정적으로 링크한 경우 생략 가능)
set(CMAKE_BUILD_RPATH "${j2_library_ROOT}/lib")
set(CMAKE_INSTALL_RPATH "$ORIGIN;$ORIGIN/../lib")
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

find_package(j2_library REQUIRED CONFIG)

add_executable(j2_app main.cpp)
target_link_libraries(j2_app PRIVATE j2_library::j2_library)

set_property(TARGET j2_app PROPERTY CXX_STANDARD 17)
```

- 빌드 예:

```bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j"$(nproc)"
```

<br />

---



 


