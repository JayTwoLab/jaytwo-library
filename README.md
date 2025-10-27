## `j2_library` 라이브러리

<br />

---

### 1. 사전 준비 사항

- `C++` 컴파일러 설치
	- `Visual Studio` (`Windows`)
		- [Visual Studio 다운로드](https://visualstudio.microsoft.com/ko/downloads/)
	- `MingW` (+ `gcc`) (`Windows`)
		- 권장: `Qt Creator`에서 `MingW`를 사용할 경우 [Qt 다운로드](https://www.qt.io/download) (오픈소스 버전)
		- [MingW 다운로드](http://mingw-w64.org/doku.php/download) 직접 설치도 가능 
	- `Linux gcc`
		- Ubuntu/Debian : `sudo apt-get install build-essential`
		- Rocky/CentOS : `sudo dnf groupinstall "Development Tools"`
		- Arch : `sudo pacman -S base-devel`
	- ⚠️ NOTICE: `clang` 에 대한 테스트는 하지 않음
- `CMake` 설치
	- [CMake 다운로드](https://cmake.org/download/) 직접 설치
	- 윈도우 scoop, chocolatey, winget 등 패키지 관리자를 이용하여 설치 가능
		- scoop : `scoop install cmake`	
	- 리눅스 패키지 관리자를 이용하여 설치 가능
		- Ubuntu/Debian : `sudo apt-get install cmake`
		- Rocky/CentOS : `sudo dnf install cmake`
		- Arch : `sudo pacman -S cmake`
- `ninja` 설치 (선택 사항)
	- [Ninja 다운로드](https://ninja-build.org/)
		- 윈도우: scoop, chocolatey, winget 등 패키지 관리자를 이용하여 설치 가능
	- `CMake`에서 `Ninja`를 빌드 도구로 사용할 경우 설치
- `IDE` 설치
	- `Windows`
	    - `Visual Studio`
		- `Qt Creator` (+ `MingW`)
		- `VSCode` (+ `MingW` or `MSVC`)
	- `Linux`
		- `VSCode` (+ `gcc` or `WSL`)

<br />

---

#### 1.1. `vcpkg` 를 설치하여 라이브러리를 사용하는 방법

##### 1.1.1. `Visual Studio` 에서 `vcpkg` 를 설치하여 라이브러리를 사용하는 방법

- `vcpkg` 설치
    - `git clone https://github.com/microsoft/vcpkg.git`
	    - 보통 사용자 계정 폴더에 설치함 : `C:\Users\j2\vcpkg`
    - 명령을 실행하여 `vcpkg.exe`를 생성
	    - `cd vcpkg`
	    - `.\bootstrap-vcpkg.bat`
	- `vcpkg` 설치 경로를 윈도 횐경 변수 `PATH`에 추가할 것
	- `vcpkg` 설치 경로를 윈도 횐경 변수 `VCPKG_ROOT`에 추가할 것
- `vcpkg` 로 종속성있는 패키지 설치
	```
	vcpkg install boost-asio:x64-windows-static
	vcpkg install gtest:x64-windows-static
	vcpkg install nlohmann-json:x64-windows-static
	vcpkg install simpleini:x64-windows-static
	vcpkg install spdlog:x64-windows-static
	```
<br />

---

##### 1.1.2. `MingW` 에서 `vcpkg` 를 설치하여 라이브러리를 사용하는 방법

- `vcpkg` 설치
    - `git clone https://github.com/microsoft/vcpkg.git`
		- 보통 사용자 계정 폴더에 설치함 : `C:\Users\j2\vcpkg`
    - 명령을 실행하여 `vcpkg.exe`를 생성
	    - `cd vcpkg`
	    - `.\bootstrap-vcpkg.bat`
	- `vcpkg` 설치 경로를 `PATH`에 추가할 것
    - `vcpkg` 설치 경로를 윈도 횐경 변수 `VCPKG_ROOT`에 추가할 것
- `vcpkg` 로 종속성있는 패키지 설치
	```
	vcpkg install boost-asio:x64-mingw-static
	vcpkg install gtest:x64-mingw-static
	vcpkg install nlohmann-json:x64-mingw-static
	vcpkg install simpleini:x64-mingw-static
	vcpkg install spdlog:x64-mingw-static
	```
		
<br />

---

#### 1.2. `vcpkg` 를 설치하지 않는 방식

##### 1.2.1. `Linux` (`non-vcpkg`)

- 리눅스에서도 `vcpkg`로 설치 가능하지만, 패키지 관리자를 이용하는 것이 더 편리함.

###### `Rocky/CentOS`
- `sudo dnf install boost-devel gtest-devel json-devel spdlog-devel`
- `simpleini` 패키지 없음 → `GitHub`에서 수동 설치 필요.
   - `j2_library_/third-party/simpleini` 에서 소스코드 복사하여 사용 가능.

###### `Ubuntu/Debian`
- `sudo apt install libboost-all-dev libgtest-dev nlohmann-json3-dev libspdlog-dev`

###### `openSUSE / SUSE`
- `sudo zypper install boost-devel gtest-devel nlohmann_json spdlog-devel`
- `simpleini` 패키지 없음 → `GitHub`에서 수동 설치 필요.

<br />

---

### 2. 빌드

#### 2.1. 라이브러리 빌드

---

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

- `CMakeSettings.json` 셋팅 (`MSVC`)
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



 


