# tests/CMakeLists.txt
cmake_minimum_required(VERSION 3.20)

##########################################################################
# 모듈 경로 등록 후 include
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
#  CMAKE_SOURCE_DIR → 항상 최상위 CMakeLists.txt 디렉토리 (프로젝트 루트)
#  CMAKE_CURRENT_SOURCE_DIR → 현재 처리 중인 CMakeLists.txt 파일이 위치한 디렉토리
include(DetectAndConfigurePlatformWithVcpkg)

# 선택 옵션 (필요 시)
# set(DCPV_VCPKG_TRIPLET "x64-mingw")  # 정적 대신 동적 MinGW 트립렛 사용
# set(DCPV_SILENT ON)                  # 상태 메시지 숨김
# set(DCPV_ENABLE_THREADS OFF)         # 스레드 미사용

# 설정 적용
setup_platform_defaults_with_vcpkg()

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

