cmake_minimum_required(VERSION 3.26)

# 이 서브프로젝트는 단독으로도 동작해야 하므로 project() 선언을 포함합니다.
project(j2_library
  VERSION 0.2.3
  LANGUAGES CXX
)

# -------------------------
# C++ 표준 설정
# -------------------------
set(J2_LIBRARY_CXX_STANDARD 17 CACHE STRING "j2_library가 사용할 C++ 표준 (예: 17, 20, 23)")
set(CMAKE_CXX_STANDARD ${J2_LIBRARY_CXX_STANDARD})
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 가시성 최적화(GCC/Clang)
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
  add_compile_options(-fvisibility=hidden -fvisibility-inlines-hidden)
endif()

# Linux RPATH (설치 후 실행 편의)
if (UNIX AND NOT APPLE)
  set(CMAKE_INSTALL_RPATH "$ORIGIN/../lib")
  set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
endif()

# 스레드 라이브러리
find_package(Threads REQUIRED)

# -------------------------
# 라이브러리 정의
# -------------------------

# src 디렉터리 안의 모든 cpp 파일을 glob으로 수집
file(GLOB SRC_FILES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

# 라이브러리 추가
add_library(j2_library ${SRC_FILES})

# ALIAS 생성  
#  실제 타깃 이름: j2_library
#  별칭(ALIAS) 이름: j2_library::j2_library
add_library(j2_library::j2_library ALIAS j2_library)

# 정적/공유 타입에 따라 매크로 자동 전파
target_compile_definitions(j2_library
  PUBLIC  $<$<STREQUAL:$<TARGET_PROPERTY:j2_library,TYPE>,STATIC_LIBRARY>:J2LIB_STATIC>
  PRIVATE $<$<STREQUAL:$<TARGET_PROPERTY:j2_library,TYPE>,SHARED_LIBRARY>:J2LIB_BUILDING_DLL>
)

# Windows 특성
if (WIN32)
  target_compile_definitions(j2_library PUBLIC NOMINMAX)
endif()

# include 경로
target_include_directories(j2_library
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/generated>
    $<INSTALL_INTERFACE:include>
)

# 버전 헤더 생성
configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/include/j2_library/version.hpp.in
  ${CMAKE_CURRENT_BINARY_DIR}/generated/j2_library/version.hpp
  @ONLY
)

# 표준 요구(소비자도 동일 표준 사용)
target_compile_features(j2_library PUBLIC cxx_std_${J2_LIBRARY_CXX_STANDARD})

# 필요한 시스템 라이브러리
target_link_libraries(j2_library PRIVATE Threads::Threads)

# -------------------------
# 설치/패키징
# -------------------------
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

install(
  TARGETS j2_library
  EXPORT j2_libraryTargets
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/include/
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/generated/j2_library/version.hpp
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/j2_library)

# 패키지 Config 파일 생성
configure_package_config_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/cmake/j2_libraryConfig.cmake.in
  ${CMAKE_CURRENT_BINARY_DIR}/j2_libraryConfig.cmake
  INSTALL_DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/j2_library
)

write_basic_package_version_file(
  ${CMAKE_CURRENT_BINARY_DIR}/j2_libraryConfigVersion.cmake
  VERSION ${PROJECT_VERSION}
  COMPATIBILITY SameMajorVersion
)

install(FILES
  ${CMAKE_CURRENT_BINARY_DIR}/j2_libraryConfig.cmake
  ${CMAKE_CURRENT_BINARY_DIR}/j2_libraryConfigVersion.cmake
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/j2_library
)

install(EXPORT j2_libraryTargets
  NAMESPACE j2_library::
  DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/j2_library
)
