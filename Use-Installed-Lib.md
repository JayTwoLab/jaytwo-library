
#### 설치된 라이브러리 사용하기

---

#### `Visual Studio` 에서 설치된 라이브러리 사용하기

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

#### `MinGW`(`g++`) 에서 설치된 라이브러리 사용하기

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

#### `Linux` (`g++`) 에서 설치된 라이브러리 사용하기

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

