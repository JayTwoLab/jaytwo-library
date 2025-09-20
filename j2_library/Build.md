# 빌드

- 아래 예시들은 **라이브러리 폴더만 단독 빌드** 합니다.

# Linux (GCC/Clang)

## 정적 라이브러리 (.a)

```bash
cmake -S j2_library \
  -B build-lib-static \
  -G Ninja \
  -DCMAKE_BUILD_TYPE=Release \
  -DBUILD_SHARED_LIBS=OFF
  
cmake --build build-lib-static -j
# 산출물: build-lib-static/libj2_library.a
```

## 공유 라이브러리 (.so)

```bash
cmake -S j2_library \
  -B build-lib-shared \
  -G Ninja \
  -DCMAKE_BUILD_TYPE=Release \
  -DBUILD_SHARED_LIBS=ON
  
cmake --build build-lib-shared -j
# 산출물: build-lib-shared/libj2_library.so
```

# Windows (MinGW)

## 정적 라이브러리 (.a)

```bat
cmake -S j2_library \
  -B build-lib-static \
  -G "MinGW Makefiles" \
  -DCMAKE_BUILD_TYPE=Release \
  -DBUILD_SHARED_LIBS=OFF

cmake --build build-lib-static -j
REM 산출물: build-lib-static\libj2_library.a
```

## 공유 라이브러리 (.dll)

```bat
cmake -S j2_library \
  -B build-lib-shared \
  -G "MinGW Makefiles" \
  -DCMAKE_BUILD_TYPE=Release \
  -DBUILD_SHARED_LIBS=ON
  
cmake --build build-lib-shared -j
REM 산출물: build-lib-shared\j2_library.dll (+ import lib: libj2_library.dll.a)
```

# 프리셋을 쓰신다면

```bash
# Windows 정적(라이브러리만)
cmake --preset windows-mingw-lib-release
cmake --build --preset windows-mingw-lib-release -j

# Linux 공유(라이브러리만)
cmake --preset linux-lib-release-shared
cmake --build --preset linux-lib-release-shared -j
```

# 참고

* 위 명령은 **라이브러리 전용 서브프로젝트(`j2_library/`)** 만 설정합니다. 저장소 루트 전체를 빌드하고 싶다면 `-S .` 로 바꾸면 됩니다.
* 정적/공유 전환 시, 현재 CMake 설정이 **타깃 타입에 맞춰 `J2LIB_STATIC`/`J2LIB_BUILDING_DLL` 매크로를 자동 전파**하므로 헤더/소스는 수정할 필요가 없습니다.
* 설치까지 하려면:

  ```bash
  cmake --install build-lib-static  --prefix /usr/local      # Linux 예시
  cmake --install build-lib-shared  --prefix "C:/dev/prefix" # Windows 예시
  ```

