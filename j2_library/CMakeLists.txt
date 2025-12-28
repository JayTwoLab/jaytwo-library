# j2_library/CMakeLists.txt
cmake_minimum_required(VERSION 3.26)

# -------------------------
# 프로젝트 선언
# -------------------------
set(J2_LIBRARY_VERSION "0.1.0") # j2 라이브러리 프로젝트 버전 
project(j2_library  VERSION ${J2_LIBRARY_VERSION}  LANGUAGES CXX)
# 이 서브프로젝트는 단독으로도 동작해야 하므로 project() 선언을 포함합니다.
# 프로젝트 VERSION은 주로 설치/패키징에 사용됩니다.
# cmake 파일에서 VERSION을 설정하면 헤더 파일(version.hpp, 자동 생성)에 자동으로 반영됩니다.
#  TODO: 버전이 변경되면 반영할 것. (예: VERSION 0.1.0)

# --- 중복 포함 가드: 이미 j2_library 타깃이 존재하면 재정의/재설치 로직을 건너뜁니다.
if (TARGET j2_library OR TARGET j2_library::j2_library)
  message(STATUS "[j2_library] Target already exists; skip duplicate definition.")
  return()
endif()

# j2_library 루트 경로 설정 (전역 변수)
set(J2_LIBRARY_ROOT_PATH "${CMAKE_CURRENT_SOURCE_DIR}" CACHE PATH "Path to j2_library root")

# -------------------------
# C++ 표준 설정 (예: 17, 20, 23)
# -------------------------
set(J2_LIBRARY_CXX_STANDARD 17
   CACHE STRING "C++ standards for use by j2_library (e.g., 17, 20, 23)")
set(CMAKE_CXX_STANDARD ${J2_LIBRARY_CXX_STANDARD})

set(CMAKE_CXX_STANDARD_REQUIRED ON) # 지정한 표준을 반드시 사용

set(CMAKE_CXX_EXTENSIONS OFF) # OFF: 컴파일러 확장 기능(GNU/Clang의 gnu++11 같은 확장 표준 모드) 을 쓰지 않고 순수한 표준 모드(c++11, c++14 등) 만 사용


# -------------------------
# 설치 경로 변수(예: CMAKE_INSTALL_INCLUDEDIR 등) 활성화
# ※ 반드시 install() 호출보다 먼저 와야 함
# -------------------------
include(GNUInstallDirs)


# -------------------------
# j2_cmake 모듈 포함
# -------------------------
list(APPEND CMAKE_MODULE_PATH "${J2_LIBRARY_ROOT_PATH}/cmake") 
include(j2_cmake OPTIONAL RESULT_VARIABLE _J2_CMAKE_FOUND) # j2_cmake.cmake 모듈 포함
if(NOT _J2_CMAKE_FOUND)
 message(FATAL_ERROR "[j2_libarary] Mandatory module j2_cmake.cmake not found, please check CMAKE_MODULE_PATH settings.")
endif()

# -------------------------
# 가시성 최적화(GCC/Clang)
# -------------------------
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
  add_compile_options(-fvisibility=hidden -fvisibility-inlines-hidden) # 기본 가시성 숨김
endif()

# -------------------------
# Linux RPATH (설치 후 실행 편의)
# -------------------------
if (UNIX AND NOT APPLE)
  set(CMAKE_INSTALL_RPATH "$ORIGIN/../lib")
  set(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
endif()

# -------------------------
# 외부 패키지 찾기
#  TODO: 패키지 의존성이 추가되면 여기에 추가할 것
# -------------------------

# third-party 라이브러리 루트 경로 설정
if(NOT DEFINED J2_THIRD_PARTY_ROOT_PATH)
  set(J2_THIRD_PARTY_ROOT_PATH "${J2_LIBRARY_ROOT_PATH}/third-party")
endif()

# 외부 패키지 찾기 모듈 포함
include(j2_find_package)

# -------------------------
# 라이브러리 정의
#  src 모든 하위 디렉토리를 자동으로 추가.
#  src/ 전체를 재귀적으로 뒤져서 .cpp .c 전부 포함.
# -------------------------
file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS
  "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
  "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c"
)

# -------------------------
# 라이브러리 추가
# -------------------------
add_library(j2_library ${SRC_FILES})

# -------------------------
# 라이브러리 별칭(alias) 설정
# -------------------------
add_library(j2_library::j2_library ALIAS j2_library)
#  실제 타깃 이름: j2_library
#  별칭(ALIAS) 이름: j2_library::j2_library

# -------------------------
# 정적/공유 타입에 따라 매크로 자동 전파
# -------------------------
target_compile_definitions(j2_library
  PUBLIC  $<$<STREQUAL:$<TARGET_PROPERTY:j2_library,TYPE>,STATIC_LIBRARY>:J2LIB_STATIC>
  PRIVATE $<$<STREQUAL:$<TARGET_PROPERTY:j2_library,TYPE>,SHARED_LIBRARY>:J2LIB_BUILDING_DLL>
)

# -------------------------
# Windows 특성
# -------------------------
if (WIN32)
  target_compile_definitions(j2_library PUBLIC NOMINMAX)
endif()

# -------------------------
# include 경로
# -------------------------
target_include_directories(j2_library
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}/generated>
    $<INSTALL_INTERFACE:include>
)

# -------------------------
# 버전 헤더 생성
# -------------------------
# 이전: configure_file(...) 로 generated/version.hpp 를 만들던 방식
# 이제는 수동 관리형 헤더(j2_library/include/j2_library/version.hpp)를 사용합니다.
# 만약 자동화가 필요하면 configure_file() 을 복원하세요.
# configure_file(
#   ${CMAKE_CURRENT_SOURCE_DIR}/include/j2_library/version.hpp.in
#   ${CMAKE_CURRENT_BINARY_DIR}/generated/j2_library/version.hpp
#   @ONLY
# )

# -------------------------
# 표준 요구 (라이브러리 사용자도 동일 표준 사용)
# -------------------------
target_compile_features(j2_library PUBLIC cxx_std_${J2_LIBRARY_CXX_STANDARD})

# -------------------------
# 필요한 시스템 라이브러리 및 외부 라이브러리 링크
#   라이브러리 의존성이 추가되면 여기에 추가할 것
# -------------------------
include(j2_link_lib)
j2_link_lib(j2_library)



######################################################
# GCC 버전에 따라 <filesystem> 라이브러리 링크 설정
if (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    # GCC 8.x 에서는 std::filesystem 이 별도 라이브러리(stdc++fs)에 들어 있는 경우가 많음
    if (CMAKE_CXX_COMPILER_VERSION VERSION_LESS 9)
        # GCC 8.x 전용 설정: -lstdc++fs 링크
        target_link_libraries(j2_library PRIVATE stdc++fs)
    endif()
endif()

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

# 이전에는 generated/version.hpp 를 설치했음.
# 이제는 소스 트리의 수동 관리형 헤더를 설치하도록 변경합니다.
install(FILES ${CMAKE_CURRENT_SOURCE_DIR}/include/j2_library/version.hpp
        DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/j2_library)

# -------------------------
# 패키지 Config 파일 생성
# -------------------------
configure_package_config_file(
  ${J2_LIBRARY_ROOT_PATH}/cmake/j2_libraryConfig.cmake.in
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

# Install shipped CMake helper modules (public cmake scripts)
install(
    DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/cmake/"
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/cmake/j2_library"
    FILES_MATCHING
      PATTERN "*.cmake"
      PATTERN "*.cmake.in"
)


# install third-party: 존재하는 하위 폴더만 설치하도록 안전하게 처리

if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/third-party/include")
  install(
    DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/third-party/include/"
    DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}/j2_library/third-party"
    FILES_MATCHING
      PATTERN "*.h" PATTERN "*.hpp" PATTERN "*.hh" PATTERN "*.ipp"
  )
endif()

if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/third-party/lib")
  install(
    DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/third-party/lib/"
    DESTINATION "${CMAKE_INSTALL_LIBDIR}/j2_library/third-party"
    FILES_MATCHING
      PATTERN "*.lib" PATTERN "*.a" PATTERN "*.la"
  )
endif()

if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/third-party/bin")
  install(
    DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/third-party/bin/"
    DESTINATION "${CMAKE_INSTALL_BINDIR}/j2_library/third-party"
    FILES_MATCHING
      PATTERN "*.dll" PATTERN "*.so" PATTERN "*.dylib" PATTERN "*.exe"
  )
endif()

if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/third-party/share")
  install(
    DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/third-party/share/"
    DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/j2_library/third-party"
    PATTERN ".git" EXCLUDE
    PATTERN ".git/*" EXCLUDE
    PATTERN "CMakeLists.txt" EXCLUDE
  )
endif()

# (선택) 전체 트리를 항상 복사하고 싶다면 위 대신 다음처럼 루트 존재만 검사할 수 있음:
# if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/third-party")
#   install(
#     DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/third-party/"
#     DESTINATION "${CMAKE_INSTALL_DATAROOTDIR}/j2_library/third-party"
#     PATTERN ".git" EXCLUDE
#     PATTERN ".git/*" EXCLUDE
#     PATTERN "CMakeLists.txt" EXCLUDE
#   )
# endif()










