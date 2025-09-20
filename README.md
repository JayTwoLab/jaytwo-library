# `j2_library`

- `Linux`와 MinGW(Windows) 양쪽을 지원하는 `C++` 라이브러리 템플릿입니다.  
- `C++` 표준은 기본 `C++17`이며, `-DJ2_LIBRARY_CXX_STANDARD=20` 등으로 조정할 수 있습니다.  
- 가시성 제어(`export.hpp`), `CMake` 패키징/설치, 예제/테스트, 경고/샌리타이저, `RPATH`(`Linux`) 등을 포함합니다.

## 빌드
```bash
# Linux
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release -DJ2_LIBRARY_CXX_STANDARD=20
cmake --build build -j
ctest --test-dir build

# Windows (MinGW)
cmake -S . -B build -G "MinGW Makefiles" -DCMAKE_BUILD_TYPE=Release ^
      -DJ2_LIBRARY_BUILD_TESTS=ON -DJ2_LIBRARY_CXX_STANDARD=17
cmake --build build -j
ctest --test-dir build
```

## 설치
```bash
sudo cmake --install build --prefix /usr/local        # Linux
cmake --install build --prefix "C:/dev/j2_prefix"     # Windows
```

## 사용
```cmake
find_package(j2_library CONFIG REQUIRED)
target_link_libraries(your_app PRIVATE j2_library::j2_library)
```

