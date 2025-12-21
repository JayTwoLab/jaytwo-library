
#### 라이브러리 빌드하기

<br />

---

##### `Visual Studio` 에서 라이브러리 빌드 및 설치

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

##### `MinGW` (`g++`) 에서 라이브러리 빌드 및 설치

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

#####  `Linux` (`g++`)  에서 라이브러리 빌드 및 설치

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
