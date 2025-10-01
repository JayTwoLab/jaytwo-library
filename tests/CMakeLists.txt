cmake_minimum_required(VERSION 3.26)

project(j2_test
  VERSION 0.1.0
  LANGUAGES CXX
)


list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../j2_library/cmake")
include(j2_cmake OPTIONAL RESULT_VARIABLE _J2_CMAKE_FOUND)
if(NOT _J2_CMAKE_FOUND)
  message(FATAL_ERROR "[j2_test] Mandatory module j2_cmake.cmake not found, please check CMAKE_MODULE_PATH settings.")
endif()

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

add_executable(j2_tests
  test_basic.cpp
)

# ★ 안전벨트: 테스트 타깃에 정적 CRT 강제\
### if (MSVC)
###   set_property(TARGET j2_tests PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
### endif()


target_link_libraries(j2_tests
  PRIVATE
    j2_library::j2_library
    GTest::gtest_main
)

include(GoogleTest)
gtest_discover_tests(j2_tests)
