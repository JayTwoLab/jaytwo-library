cmake_minimum_required(VERSION 3.20)

project(j2_library
  VERSION 0.1.0
  LANGUAGES CXX
)

# Windows MinGW + vcpkg 설정
if (WIN32 AND MINGW)
  set(VCPKG_ROOT $ENV{USERPROFILE})
  set(CMAKE_PREFIX_PATH    "${VCPKG_ROOT}/vcpkg/installed/x64-mingw-static")
  set(CMAKE_TOOLCHAIN_FILE "${VCPKG_ROOT}/vcpkg/scripts/buildsystems/vcpkg.cmake" CACHE FILEPATH "" FORCE)
endif()

# 옵션
option(J2_LIBRARY_BUILD_TESTS "단위 테스트를 빌드합니다." ON)
option(J2_LIBRARY_ENABLE_WARNINGS "컴파일 경고를 설정합니다." ON)
option(J2_LIBRARY_ENABLE_SANITIZERS "Address/UB Sanitizer를 활성화합니다." OFF)
option(J2_LIBRARY_MINGW_STATIC_RUNTIME "MinGW에서 libstdc++/libgcc 정적 연결" OFF)

# -------------------------
# C++ 표준 설정
# -------------------------
set(J2_LIBRARY_CXX_STANDARD 17 CACHE STRING "j2_library가 사용할 C++ 표준 (예: 17, 20, 23)")
set(CMAKE_CXX_STANDARD ${J2_LIBRARY_CXX_STANDARD})
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

message(STATUS "j2_library: C++${CMAKE_CXX_STANDARD} 표준 사용")

# 가시성 및 RPATH
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
  add_compile_options(-fvisibility=hidden -fvisibility-inlines-hidden)
endif()
if (UNIX AND NOT APPLE)
  set(CMAKE_BUILD_RPATH "${CMAKE_BINARY_DIR}")
  set(CMAKE_INSTALL_RPATH "$ORIGIN/../lib")
  set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
endif()

# 모듈 불러오기
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
include(warnings)
include(sanitize)

find_package(Threads REQUIRED)

# 라이브러리
add_library(j2_library
  src/j2_library.cpp
  src/version.cpp
)
add_library(j2_library::j2_library ALIAS j2_library)

target_include_directories(j2_library
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

if (WIN32)
  target_compile_definitions(j2_library PUBLIC NOMINMAX)
endif()

if (MINGW AND J2_LIBRARY_MINGW_STATIC_RUNTIME)
  target_link_options(j2_library PRIVATE -static -static-libgcc -static-libstdc++)
endif()

if (J2_LIBRARY_ENABLE_WARNINGS)
  j2_enable_warnings(j2_library)
endif()

if (J2_LIBRARY_ENABLE_SANITIZERS)
  j2_enable_sanitizers(j2_library)
endif()

configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/include/j2_library/version.hpp.in
  ${CMAKE_CURRENT_BINARY_DIR}/generated/j2_library/version.hpp
  @ONLY
)

target_include_directories(j2_library
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/generated>
)

# 소비자도 동일한 C++ 표준 요구
target_compile_features(j2_library PUBLIC cxx_std_${J2_LIBRARY_CXX_STANDARD})

target_link_libraries(j2_library PRIVATE Threads::Threads)

# 설치/패키징
include(GNUInstallDirs)

install(
  TARGETS j2_library
  EXPORT j2_libraryTargets
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

install(
  DIRECTORY include/
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

install(
  FILES
    ${CMAKE_CURRENT_BINARY_DIR}/generated/j2_library/version.hpp
  DESTINATION
    ${CMAKE_INSTALL_INCLUDEDIR}/j2_library
)

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
  ${CMAKE_CURRENT_BINARY_DIR}/j2_libraryConfigVersion.cmake
  VERSION ${PROJECT_VERSION}
  COMPATIBILITY SameMajorVersion
)

configure_package_config_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/cmake/j2_libraryConfig.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/j2_libraryConfig.cmake
  INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/j2_library
)

install(
  FILES
    ${CMAKE_CURRENT_BINARY_DIR}/j2_libraryConfig.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/j2_libraryConfigVersion.cmake
  DESTINATION
    ${CMAKE_INSTALL_LIBDIR}/cmake/j2_library
)

install(
  EXPORT j2_libraryTargets
  NAMESPACE j2_library::
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/j2_library
)

# 예제/테스트
add_subdirectory(examples/hello)
if (J2_LIBRARY_BUILD_TESTS)
  enable_testing()
  add_subdirectory(tests)
endif()

set(CPACK_PACKAGE_NAME "j2_library")
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
include(CPack)
