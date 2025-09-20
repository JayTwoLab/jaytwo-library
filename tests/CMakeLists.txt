# tests/CMakeLists.txt
cmake_minimum_required(VERSION 3.20)

# 상위 CMake에서 이미 add_subdirectory(tests) 했다면 생략 가능
include(CTest)
if (NOT BUILD_TESTING)
  return()
endif()

# Google Test 패키지 설치 필요	

# 1) CMake 패키지 구성(Config mode) 우선 시도 (MSYS2, vcpkg 등에서 흔함)
find_package(GTest CONFIG QUIET)

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

