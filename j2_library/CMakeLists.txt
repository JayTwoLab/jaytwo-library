# j2_library/CMakeLists.txt
cmake_minimum_required(VERSION 3.26)

# -------------------------
# 프로젝트 선언
# -------------------------
project(j2_library
  VERSION 0.1.0
  LANGUAGES CXX
)
# 이 서브프로젝트는 단독으로도 동작해야 하므로 project() 선언을 포함합니다.
# 프로젝트 VERSION은 주로 설치/패키징에 사용됩니다.
# cmake 파일에서 VERSION을 설정하면 헤더 파일(version.hpp, 자동 생성)에 자동으로 반영됩니다.
#  TODO: 버전이 변경되면 반영할 것. (예: VERSION 0.1.0)

# --- 중복 포함 가드: 이미 j2_library 타깃이 존재하면 재정의/재설치 로직을 건너뜁니다.
if (TARGET j2_library OR TARGET j2_library::j2_library)
  message(STATUS "[j2_library] Target already exists; skip duplicate definition.")
  return()
endif()

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
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake") #  CMAKE_CURRENT_SOURCE_DIR → 현재 처리 중인 CMakeLists.txt 파일이 위치한 디렉토리
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

################################
# 스레드 라이브러리
find_package(Threads REQUIRED) 

################################
# nlohmann_json 라이브러리 (include/j2_library/json/INSTALL.ko.md 참고)
# nlohmann_json: 패키지 우선, 실패 시 로컬 vendor 인터페이스 타깃 구성 + 같은 export set 포함
find_package(nlohmann_json CONFIG QUIET)
if (nlohmann_json_FOUND)
  message(STATUS "Found nlohmann_json from package manager")
else()
  message(WARNING "nlohmann_json not found, using third-party vendor (header-only)")

  # 로컬(서브모듈) 헤더 경로
  set(NLOHMANN_JSON_INC "${CMAKE_CURRENT_SOURCE_DIR}/third-party/nlohmann_json/include")
  if (NOT EXISTS "${NLOHMANN_JSON_INC}")
    message(FATAL_ERROR "Expected nlohmann_json include directory not found: ${NLOHMANN_JSON_INC}")
  endif()

  # 헤더 전용 타깃
  add_library(nlohmann_json_vendor INTERFACE)

  # BUILD/INSTALL 인터페이스 분리 (설치 시 소스 경로 금지)
  target_include_directories(nlohmann_json_vendor INTERFACE
    $<BUILD_INTERFACE:${NLOHMANN_JSON_INC}>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
  )

  # 표준 이름 별칭 제공
  add_library(nlohmann_json::nlohmann_json ALIAS nlohmann_json_vendor)

  # 헤더 설치 (INSTALL_INTERFACE 경로와 일치)
  install(DIRECTORY "${NLOHMANN_JSON_INC}/"
          DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
          FILES_MATCHING PATTERN "*.hpp" PATTERN "*.h")

  # export 세트에 포함(의존성 누락 방지)
  install(TARGETS nlohmann_json_vendor
          EXPORT j2_libraryTargets)
endif()

################################
# spdlog: 패키지 우선, 없으면 third-party 사용
find_package(spdlog CONFIG QUIET)
if (spdlog_FOUND)
  message(STATUS "Found spdlog from package manager")
else()
  message(WARNING "spdlog not found, using third-party vendor (header-only)")

  set(SPDLOG_INC "${CMAKE_CURRENT_SOURCE_DIR}/third-party/spdlog/include")
  if (NOT EXISTS "${SPDLOG_INC}")
    message(FATAL_ERROR "Expected spdlog include directory not found: ${SPDLOG_INC}")
  endif()

  add_library(spdlog_vendor INTERFACE)
  target_include_directories(spdlog_vendor INTERFACE
    $<BUILD_INTERFACE:${SPDLOG_INC}>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
  )

  add_library(spdlog::spdlog ALIAS spdlog_vendor)

  # 헤더 설치
  install(DIRECTORY "${SPDLOG_INC}/"
          DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
          FILES_MATCHING PATTERN "*.hpp" PATTERN "*.h")

  install(TARGETS spdlog_vendor
          EXPORT j2_libraryTargets)
endif()



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
configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/include/j2_library/version.hpp.in
  ${CMAKE_CURRENT_BINARY_DIR}/generated/j2_library/version.hpp
  @ONLY
)

# -------------------------
# 표준 요구 (라이브러리 사용자도 동일 표준 사용)
# -------------------------
target_compile_features(j2_library PUBLIC cxx_std_${J2_LIBRARY_CXX_STANDARD})

# -------------------------
# 필요한 시스템 라이브러리
#  TODO: 라이브러리 의존성이 추가되면 여기에 추가할 것
# -------------------------
target_link_libraries(j2_library PRIVATE Threads::Threads)  # 스레드 라이브러리
target_link_libraries(j2_library PUBLIC nlohmann_json::nlohmann_json)  # nlohmann_json 라이브러리
target_link_libraries(j2_library PUBLIC spdlog::spdlog) # spdlog 라이브러리

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

# -------------------------
# 패키지 Config 파일 생성
# -------------------------
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






