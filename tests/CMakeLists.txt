cmake_minimum_required(VERSION 3.26)

# 프로젝트 선언
project(j2_test
  VERSION 0.1.0
  LANGUAGES CXX
)

# j2_cmake 모듈 포함
#  CMAKE_CURRENT_SOURCE_DIR → 현재 처리 중인 CMakeLists.txt 파일이 위치한 디렉토리
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../j2_library/cmake")
include(j2_cmake OPTIONAL RESULT_VARIABLE _J2_CMAKE_FOUND)
if(NOT _J2_CMAKE_FOUND)
  message(FATAL_ERROR "[j2_test] Mandatory module j2_cmake.cmake not found, please check CMAKE_MODULE_PATH settings.")
endif()

# 테스트 빌드 설정
include(CTest)
if (NOT BUILD_TESTING)
  return()
endif()

# GTest 찾기
find_package(GTest CONFIG QUIET)
if (NOT GTest_FOUND AND DEFINED DCPV_EFFECTIVE_VCPKG_PREFIX AND EXISTS "${DCPV_EFFECTIVE_VCPKG_PREFIX}")
  find_package(GTest CONFIG QUIET HINTS "${DCPV_EFFECTIVE_VCPKG_PREFIX}/share/gtest")
endif()
if (NOT GTest_FOUND)
  find_package(GTest QUIET)
endif()
if (NOT GTest_FOUND)
  message(FATAL_ERROR "GTest not found. Install with vcpkg: gtest:x64-windows-static and set toolchain/triplet.")
endif()

# 실행 프로그램 테스트 타깃 생성
file(GLOB SRC_FILES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
)
add_executable(j2_tests ${SRC_FILES})

# 테스트 타깃에 포함할 디렉토리 설정
target_include_directories(j2_tests
  PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/../j2_library/include"
)

# 테스트 타깃에 라이브러리 링크
target_link_libraries(j2_tests
  PRIVATE
    j2_library::j2_library
    GTest::gtest_main
)

# 구글 테스트 등록
include(GoogleTest)
gtest_discover_tests(j2_tests)


