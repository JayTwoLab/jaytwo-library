# tests/CMakeLists.txt
cmake_minimum_required(VERSION 3.20)

##########################################################################
#  사용자 계정 이름 가져오기 (윈도우/리눅스/맥 공통)
set(CURRENT_USER $ENV{USERNAME})  # Windows
if (NOT CURRENT_USER)
  set(CURRENT_USER $ENV{USER})    # Linux/macOS용
endif()

message(STATUS "현재 사용자 계정: ${CURRENT_USER}")

# 아키텍처 판별 (64비트/32비트)
if (CMAKE_SIZEOF_VOID_P EQUAL 8)
  set(ARCH_BITS 64)
else()
  set(ARCH_BITS 32)
endif()

# OS / 플랫폼 정보 (참고용)
message(STATUS "System: ${CMAKE_SYSTEM_NAME}, Compiler: ${CMAKE_CXX_COMPILER_ID}, ARCH: ${ARCH_BITS}-bit")

# ------------------------------------------------------------
# 컴파일러/플랫폼 별 분기
# ------------------------------------------------------------
# 1) MSVC (Visual C++)
#   - clang-cl 로 MSVC 툴체인 사용하는 경우도 MSVC로 분류됨(MSVC 변수가 TRUE).
if (MSVC)
  message(STATUS "[MSVC] Visual C++ 설정 적용")

  # 경고 레벨 및 엄격 모드
  add_compile_options(/W4 /permissive-)
  add_definitions(-D_CRT_SECURE_NO_WARNINGS)   # 필요 시 사용

  # 런타임 라이브러리(/MD, /MT) 선택 예시 (CMake 3.15+)
  # set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")       # /MT, /MTd
  # set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreadedDLL$<$<CONFIG:Debug>:Debug>")   # /MD, /MDd

  # vcpkg 경로
  set(CMAKE_PREFIX_PATH "C:/Users/${CURRENT_USER}/vcpkg/installed/x64-windows") # visual studio triplet
  # set(CMAKE_PREFIX_PATH "C:/Users/${CURRENT_USER}/vcpkg/installed/x64-windows-static") # 정적 라이브러리 triplet

# 2) MinGW (Windows용 GNU 계열: g++ or clang with MinGW)
elseif (MINGW)
  message(STATUS "[MinGW] MinGW 설정 적용")

  add_compile_options(-Wall -Wextra -Wpedantic -O2)

  # POSIX 스레드 등 링크가 필요한 경우
  set(THREADS_PREFER_PTHREAD_FLAG ON)
  find_package(Threads REQUIRED)

  # vcpkg 경로 (예: 정적 트립렛)
  # set(CMAKE_PREFIX_PATH "C:/Users/${CURRENT_USER}/vcpkg/installed/x64-mingw") # MinGW triplet
  set(CMAKE_PREFIX_PATH "C:/Users/${CURRENT_USER}/vcpkg/installed/x64-mingw-static") # 정적 라이브러리 triplet

# 3) GNU on Linux (리눅스 g++)
#    - GNU 컴파일러이면서 UNIX, 그리고 Apple이 아닌 경우를 Linux g++로 간주
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND UNIX AND NOT APPLE)
  message(STATUS "[Linux/GNU] g++ 설정 적용")

  add_compile_options(-Wall -Wextra -Wpedantic -O2)

  set(THREADS_PREFER_PTHREAD_FLAG ON)
  find_package(Threads REQUIRED)

  # 필요 시 vcpkg 경로
  set(CMAKE_PREFIX_PATH "/home/${CURRENT_USER}/vcpkg/installed/x64-linux")

# 4) Clang on macOS (AppleClang 또는 Clang + APPLE)
elseif (APPLE AND (CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang"))
  message(STATUS "[macOS/Clang] AppleClang/Clang 설정 적용")

  # macOS 기본 표준 라이브러리는 libc++ (대부분 기본값이므로 -stdlib 지정 생략 가능)
  add_compile_options(-Wall -Wextra -Wpedantic -O2)
  # 필요 시 다음을 명시적으로 사용할 수 있습니다:
  # add_compile_options(-stdlib=libc++)
  # set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -stdlib=libc++")

  set(THREADS_PREFER_PTHREAD_FLAG ON)
  find_package(Threads REQUIRED)

  # vcpkg 경로 (기본 트립렛 예: x64-osx)
  set(CMAKE_PREFIX_PATH "/Users/${CURRENT_USER}/vcpkg/installed/x64-osx")

# 5) Clang on Linux (리눅스 clang++)
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "Clang" AND UNIX AND NOT APPLE)
  message(STATUS "[Linux/Clang] clang++ 설정 적용")

  add_compile_options(-Wall -Wextra -Wpedantic -O2)
  # 리눅스에서는 보통 libstdc++과 링크되므로 -stdlib 지정 불필요(필요 시 -stdlib=libstdc++)

  set(THREADS_PREFER_PTHREAD_FLAG ON)
  find_package(Threads REQUIRED)

  # 필요 시 vcpkg 경로
  set(CMAKE_PREFIX_PATH "/home/${CURRENT_USER}/vcpkg/installed/x64-linux")

# 6) 기타 (그 외 환경)
else()
  message(STATUS "[기타] 일반 설정 적용 (예: 기타 툴체인)")

  add_compile_options(-Wall -Wextra -Wpedantic -O2)
endif()


##########################################################################

# 상위 CMake에서 이미 add_subdirectory(tests) 했다면 생략 가능
include(CTest)
if (NOT BUILD_TESTING)
  return()
endif()

# Google Test 패키지 설치 필요	

# 1) CMake 패키지 구성(Config mode) 우선 시도 (MSYS2, vcpkg 등에서 흔함)
find_package(GTest CONFIG QUIET)
# find_package(GTest CONFIG ON)

# 2) 모듈 모드(배포판의 FindGTest.cmake)도 함께 시도
if (NOT GTest_FOUND)
  find_package(GTest QUIET)
endif()

if (NOT GTest_FOUND)
  message(FATAL_ERROR "GTest 패키지를 찾을 수 없습니다. 시스템 패키지 설치 또는 CMAKE_PREFIX_PATH 설정이 필요합니다.")
endif()

add_executable(j2_tests test_basic.cpp)
target_link_libraries(j2_tests
  PRIVATE
    j2_library::j2_library
    GTest::gtest_main
)

# CMake 내장 GoogleTest 모듈 사용(테스트 자동 등록)
include(GoogleTest)
gtest_discover_tests(j2_tests)

