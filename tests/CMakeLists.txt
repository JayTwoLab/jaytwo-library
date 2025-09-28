# tests/CMakeLists.txt
cmake_minimum_required(VERSION 3.26)

###############################################
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../j2_library/cmake")
#  CMAKE_CURRENT_SOURCE_DIR → 현재 처리 중인 CMakeLists.txt 파일이 위치한 디렉토리

# 경고/샌리타이저 모듈(있으면 사용)
if (COMMAND j2_enable_warnings)
  j2_enable_warnings(j2_library) # warnings.cmake
endif()
if (COMMAND j2_enable_sanitizers)
  j2_enable_sanitizers(j2_library) # sanitize.cmake
endif()

# 소스코드 인코딩 설정 모듈(있으면 사용)
include(utf8 OPTIONAL) # utf8.cmake

# vcpkg 설정 적용
if (MSVC)
  option(USE_VCPKG "[MSVC] vcpkg를 사용합니다." ON) 
endif()
if (MINGW)
  option(USE_VCPKG "[MINGW] vcpkg를 사용합니다." ON) 
endif()
if (USE_VCPKG)
  include(vcpkg OPTIONAL) # vcpkg.cmake
  setup_platform_defaults_with_vcpkg()
endif()
#################################################

# 상위 CMake에서 이미 add_subdirectory(tests) 했다면 생략 가능
include(CTest)
if (NOT BUILD_TESTING)
  return()
endif()

# Google Test 패키지 설치 필요

# set(CMAKE_TOOLCHAIN_FILE "C:/Users/j2dol/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")


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

# 실행 프로그램
add_executable(j2_tests
  test_basic.cpp
)

# 링크 라이브러리
target_link_libraries(j2_tests
  PRIVATE
    j2_library::j2_library
    GTest::gtest_main
)

# CMake 내장 GoogleTest 모듈 사용(테스트 자동 등록)
include(GoogleTest)
gtest_discover_tests(j2_tests)

