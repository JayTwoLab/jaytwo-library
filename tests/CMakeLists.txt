# test/CMakeLists.txt
cmake_minimum_required(VERSION 3.26)

# 프로젝트 선언
set(J2_TESTS_VERSION "0.1.0")
project(j2_test VERSION ${J2_TESTS_VERSION} LANGUAGES CXX) 

set(CMAKE_CXX_STANDARD 17)                 # C++17
set(CMAKE_CXX_STANDARD_REQUIRED ON)        # 지정한 표준을 반드시 사용
set(CMAKE_CXX_EXTENSIONS OFF)              # 순수 표준 모드 사용

# j2_library 루트 경로 설정 (전역 변수)
set(J2_LIBRARY_ROOT_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../j2_library" CACHE PATH "Path to j2_library root")

# j2_cmake 모듈 경로 추가 및 포함
list(PREPEND CMAKE_MODULE_PATH "${J2_LIBRARY_ROOT_PATH}/cmake")
include(j2_cmake) # 필요한 경우 선택적으로 가져와도 됨

# 같은 소스 트리의 j2_library를 빌드(별도 바이너리 디렉터리 지정)
add_subdirectory("${J2_LIBRARY_ROOT_PATH}" "${CMAKE_CURRENT_BINARY_DIR}/j2_library_build")

# =========================
# 테스트 옵션 및 GTest 설정
# =========================
option(J2_ENABLE_TESTS "Build unit tests for j2_library" ON) # 디폴트로 테스트 기능 활성화(ON)
# cmake 명령어 인자에 -DJ2_ENABLE_TESTS=OFF 로 구글 테스트 비활성 가능

if (J2_ENABLE_TESTS)
  include(CTest)
  enable_testing()

  # 패키지 우선 (vcpkg 등)
  find_package(GTest CONFIG QUIET)

  if (GTest_FOUND)
    message(STATUS "Found GoogleTest from package manager") # 패키지 관리자에서 GTest 발견
  else()
    message(WARNING "GoogleTest not found; using third-party submodule (googletest)")

    # gtest/gmock 빌드/설치 옵션 (빌드만, 설치는 비활성)
    set(BUILD_GTEST ON CACHE BOOL "" FORCE)
    set(BUILD_GMOCK ON CACHE BOOL "" FORCE)
    set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)

    # MSVC 런타임 일관성 필요 시 주석 해제 (/MD 강제)
    # set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

    # 서브모듈 폴백 추가(빌드 전용)
    add_subdirectory("${J2_LIBRARY_ROOT_PATH}/third-party/googletest"
                     "${CMAKE_CURRENT_BINARY_DIR}/googletest_build"
                     EXCLUDE_FROM_ALL)

    # 별칭 보정 (버전에 따라 이미 제공될 수 있음)
    if (NOT TARGET GTest::gtest)
      add_library(GTest::gtest ALIAS gtest)
    endif()
    if (NOT TARGET GTest::gtest_main)
      add_library(GTest::gtest_main ALIAS gtest_main)
    endif()
    if (NOT TARGET GTest::gmock)
      add_library(GTest::gmock ALIAS gmock)
    endif()
    if (NOT TARGET GTest::gmock_main)
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

  # include 경로(테스트 소스 및 라이브러리 헤더)
  target_include_directories(${PROJECT_NAME} PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}"
    "${J2_LIBRARY_ROOT_PATH}/include"
  )

  # 라이브러리 링크
  target_link_libraries(${PROJECT_NAME}
    PRIVATE
      j2_library::j2_library
      GTest::gtest_main
  )

  # 구글 테스트 등록
  include(GoogleTest)
  gtest_discover_tests(${PROJECT_NAME})

endif() # J2_ENABLE_TESTS
