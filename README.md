## `j2_library` 라이브러리

<br />

---

### 1. 사전 준비 사항

- `C++` 컴파일러 설치
	- `Visual Studio`
		- [Visual Studio 다운로드](https://visualstudio.microsoft.com/ko/downloads/)
	- `MingW`
		- 권장: `Qt Creator`에서 `MingW`를 사용할 경우 [Qt 다운로드](https://www.qt.io/download) (오픈소스 버전)
		- [MingW 다운로드](http://mingw-w64.org/doku.php/download) 직접 설치도 가능 
	- `gcc`
		- Ubuntu/Debian : `sudo apt-get install build-essential`
		- Rocky/CentOS : `sudo dnf groupinstall "Development Tools"`
		- Arch : `sudo pacman -S base-devel`
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
		- `Qt Creator`
		- `VSCode`
	- `Linux`
		- `VSCode`

<br />

---

#### 1.1. `vcpkg` 설치 방식

##### 1.1.1. `Visual Studio`

- `vcpkg` 설치
    - `git clone https://github.com/microsoft/vcpkg.git`
	    - 보통 사용자 계정 폴더에 설치함 : `C:\Users\j2\vcpkg`
    - 명령을 실행하여 `vcpkg.exe`를 생성
	    - `cd vcpkg`
	    - `.\bootstrap-vcpkg.bat`
	- `vcpkg` 설치 경로를 윈도 횐경 변수 `PATH`에 추가
	- `vcpkg` 설치 경로를 윈도 횐경 변수 `VCPKG_ROOT`에 추가
- `Google Test` 설치
    - `vcpkg install gtest:x64-windows-static --vcpkg-root=C:\Users\j2\vcpkg`
	   - 설치 시 vcpkg 경로를 지정하지 않으면 `CMake`에서 `vcpkg`를 찾지 못하는 경우가 있음.

<br />

---

##### 1.1.2. `MingW` (`+ Qt Creator`)

- `vcpkg` 설치
    - `git clone https://github.com/microsoft/vcpkg.git`
		- 보통 사용자 계정 폴더에 설치함 : `C:\Users\j2\vcpkg`
    - 명령을 실행하여 `vcpkg.exe`를 생성
	    - `cd vcpkg`
	    - `.\bootstrap-vcpkg.bat`
	- `vcpkg` 설치 경로를 `PATH`에 추가
    - `vcpkg` 설치 경로를 윈도 횐경 변수 `VCPKG_ROOT`에 추가 
- `Google Test` 설치
	- `vcpkg install gtest:x64-mingw-static --vcpkg-root=C:\Users\j2\vcpkg` 
	   - 설치 시 vcpkg 경로를 지정하지 않으면 `CMake`에서 `vcpkg`를 찾지 못하는 경우가 있음.
		
<br />

---

#### 1.2. `non-vcpkg` 방식으로 설치

##### 1.2.1. `Linux` (non-vcpkg)

- 리눅스에서도 `vcpkg`로 설치 가능하지만, 패키지 관리자를 이용하는 것이 더 편리함.
- `Google Test` 설치
   - `Rocky/CentOS` : `sudo dnf install gtest gtest-devel -y`
   - `Ubuntu/Debian` : `sudo apt-get install libgtest-dev -y`
   - `Arch` : `sudo pacman -S gtest`

<br />

---

### 2. 빌드

#### 2.1. 라이브러리 빌드

---

##### 2.1.1. `Windows` (`MSVC`, `Visual Studio`)

```powershell
# 빌드 경로 생성
mkdir build 
cd build

# cmake 실행 (릴리즈 빌드, 설치 경로 지정)
cmake -S .. -B build -G "Visual Studio 17 2022" -A x64 -DCMAKE_BUILD_TYPE=Release  -DCMAKE_INSTALL_PREFIX="C:/install/j2_library"

# 빌드 (Release)
cmake --build build --config Release

# 설치
cmake --install build --config Release
```

<br />

---

##### 2.1.2. `Windows` (`MinGW g++`)

```bash
# 빌드 경로 생성
mkdir build 
cd build

# cmake 실행 (릴리즈 빌드, 설치 경로 지정)
cmake -S .. -B build -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX="C:/mingw-install/j2_library"

# 빌드
cmake --build build

# 설치
cmake --install build
```

<br />

---

##### 2.1.3. `Linux` (`g++`)

```bash
# 빌드 경로 생성
mkdir build 
cd build

# cmake 실행 (릴리즈 빌드, 설치 경로 지정)
cmake -S .. -B build -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX="/home/j2/workspace/dev/lib"

# 빌드 (cpu 코어 수 만큼 병렬 빌드)
cmake --build build -j$(nproc)

# 설치 
cmake --install build
# 설치 경로가 /usr/local/... 등의 시스템 경로인 경우, 설치 시 sudo(root 권한) 필요함.
#  sudo cmake --install build
```

<br />

---

 
#### 2.2. 설치된 라이브러리 사용하기

#### 2.2.1. `Windows` — `MSVC` (`Visual Studio cl.exe`)

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

<br />

---

#### 2.2.2. `Windows` — `MinGW` `g++`

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

#### 2.2.3. `Linux` — `g++`

```cmake
# CMakeLists.txt (Linux g++용)
cmake_minimum_required(VERSION 3.26)
project(j2_app_linux LANGUAGES CXX)

# 설치된 j2_library의 prefix 경로 (예: /opt/j2_library 또는 /usr/local/j2_library)
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



 


