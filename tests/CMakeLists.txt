# tests/CMakeLists.txt
cmake_minimum_required(VERSION 3.26)

project(j2_test
  VERSION 0.1.0
  LANGUAGES CXX
)

###############################################
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../j2_library/cmake")
#  CMAKE_CURRENT_SOURCE_DIR → 현재 처리 중인 CMakeLists.txt 파일이 위치한 디렉토리

# j2_cmake.cmake
include(j2_cmake OPTIONAL RESULT_VARIABLE _J2_CMAKE_FOUND)
if(NOT _J2_CMAKE_FOUND)
 message(FATAL_ERROR "[j2_test] Mandatory module j2_cmake.cmake not found, please check CMAKE_MODULE_PATH settings.")
endif()
#################################################

# 상위 CMake에서 이미 add_subdirectory(tests) 했다면 생략 가능
include(CTest)
if (NOT BUILD_TESTING)
  return()
endif()

###################################
# Google Test
#
# Rocky/CentOS/Fedora Linux 패키지 설치 : sudo yum install -y gtest-devel (dnf로도 설치 가능)
# Ubunut/Debian Linux 패키지 설치 : sudo apt install -y libgtest-dev
#
# MSYS 패키지 설치 : pacman -S mingw-w64-x86_64-gtest (64비트)
# MingW 패키지 설치 : vcpkg install gtest:x64-mingw-static (static은 옵션)
#
# Visual Studio : vcpkg install gtest:x64-windows (64비트)

# 1) CMake 패키지 구성(Config mode) 우선 시도  
find_package(GTest CONFIG QUIET)

# 2) 모듈 모드(배포판의 FindGTest.cmake)도 함께 시도
if (NOT GTest_FOUND)
  find_package(GTest QUIET)
endif()

if (NOT GTest_FOUND)
  message(FATAL_ERROR "GTest package not found. System package installation or CMAKE_PREFIX_PATH setting is required.")
endif()
###################################

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

