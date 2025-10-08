# test/CMakeLists.txt
cmake_minimum_required(VERSION 3.26)

project(j2_test VERSION 0.1.0 LANGUAGES CXX) # 프로젝트 선언

set(CMAKE_CXX_STANDARD 17) # C++17
set(CMAKE_CXX_STANDARD_REQUIRED ON) # 지정한 표준을 반드시 사용
set(CMAKE_CXX_EXTENSIONS OFF) # OFF: 컴파일러 확장 기능을 쓰지 않고 순수한 표준 모드만 사용

list(PREPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/../j2_library/cmake")
include(j2_cmake) # j2_cmake 모듈 포함 (필요한 모듈만 선택적으로 포함도 가능)

# j2_library 라이브러리를 같은 소스 트리에서 빌드
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/../j2_library" "${CMAKE_CURRENT_BINARY_DIR}/j2_library_build")

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
file(GLOB SRC_FILES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/*.c")
add_executable(${PROJECT_NAME} ${SRC_FILES})

# 테스트 타깃에 포함할 디렉토리 설정
target_include_directories(${PROJECT_NAME} PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/../j2_library/include"
)

# 테스트 타깃에 라이브러리 링크
target_link_libraries(${PROJECT_NAME} PRIVATE
    j2_library::j2_library
    GTest::gtest_main
)

# 구글 테스트 등록
include(GoogleTest)
gtest_discover_tests(${PROJECT_NAME})



