# test/CMakeLists.txt
cmake_minimum_required(VERSION 3.26)

# 프로젝트 선언
set(J2_TESTS_VERSION "0.1.0")
project(j2_test VERSION ${J2_TESTS_VERSION} LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# j2_library 루트 경로 설정 (전역 변수)
set(J2_LIBRARY_ROOT_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../j2_library" CACHE PATH "Path to j2_library root")

# j2_cmake 모듈 경로 추가 및 포함
list(PREPEND CMAKE_MODULE_PATH "${J2_LIBRARY_ROOT_PATH}/cmake")
include(j2_cmake)

# j2_library를 반드시 먼저 추가 (테스트 타깃보다 앞서야 함)
add_subdirectory("${J2_LIBRARY_ROOT_PATH}" "${CMAKE_CURRENT_BINARY_DIR}/j2_library_build")

# =========================
# 테스트 옵션 및 GTest 설정
# =========================
option(J2_ENABLE_TESTS "Build unit tests for j2_library" ON)

if (J2_ENABLE_TESTS)
  include(CTest)
  enable_testing()

  # 패키지 우선 (vcpkg, 시스템 등)
  find_package(GTest CONFIG QUIET)
  if(NOT GTest_FOUND)
    # MODULE 모드도 시도 (일부 배포판)
    find_package(GTest MODULE QUIET)
  endif()

  if (GTest_FOUND)
    message(STATUS "Found GoogleTest from package manager")
  else()
    message(WARNING "GoogleTest not found; using third-party submodule (googletest)")

    # gtest/gmock 빌드/설치 옵션
    set(BUILD_GTEST ON CACHE BOOL "" FORCE)
    set(BUILD_GMOCK ON CACHE BOOL "" FORCE)
    set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)

    # 서브모듈 폴백 추가(빌드 전용)
    add_subdirectory("${J2_LIBRARY_ROOT_PATH}/third-party/googletest"
                     "${CMAKE_CURRENT_BINARY_DIR}/googletest_build"
                     EXCLUDE_FROM_ALL)

    # 별칭 보정 (이미 있으면 재정의하지 않음)
    if (TARGET gtest AND NOT TARGET GTest::gtest)
      add_library(GTest::gtest ALIAS gtest)
    endif()
    if (TARGET gtest_main AND NOT TARGET GTest::gtest_main)
      add_library(GTest::gtest_main ALIAS gtest_main)
    endif()
    if (TARGET gmock AND NOT TARGET GTest::gmock)
      add_library(GTest::gmock ALIAS gmock)
    endif()
    if (TARGET gmock_main AND NOT TARGET GTest::gmock_main)
      add_library(GTest::gmock_main ALIAS gmock_main)
    endif()
  endif()

  # =========================
  # 테스트 타깃 생성
  # =========================
  file(GLOB SRC_FILES CONFIGURE_DEPENDS
       "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
       "${CMAKE_CURRENT_SOURCE_DIR}/*.c")

  add_executable(${PROJECT_NAME} ${SRC_FILES})

  target_include_directories(${PROJECT_NAME} PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${J2_LIBRARY_ROOT_PATH}/include"
  )

  target_link_libraries(${PROJECT_NAME}
    PRIVATE
      j2_library::j2_library
      GTest::gtest_main
  )

  include(GoogleTest)
  gtest_discover_tests(${PROJECT_NAME})

endif() # J2_ENABLE_TESTS
