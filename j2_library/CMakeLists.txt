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
  set(NLOHMANN_JSON_INC "${J2_THIRD_PARTY_ROOT_PATH}/nlohmann_json/include")
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

  set(SPDLOG_INC "${J2_THIRD_PARTY_ROOT_PATH}/spdlog/include")
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

###################################
# simpleini: 패키지 우선, 실패 시 시스템 헤더 또는 서브모듈(로컬 코드) 폴백 

# 1) vcpkg/Conan 등에서 CONFIG 패키지 시도 (있으면 simpleini::simpleini 타깃을 씁니다)

find_package(simpleini CONFIG QUIET)

message(STATUS "simpleini_FOUND = ${simpleini_FOUND}")
message(STATUS "simpleini_DIR = ${simpleini_DIR}")

if (simpleini_FOUND)
  message(STATUS "Found simpleini from package manager")
  # 단일 공개 타깃(j2_simpleini)로 래핑해 일관된 링크 사용
  add_library(j2_simpleini INTERFACE)
  target_link_libraries(j2_simpleini INTERFACE simpleini::simpleini)

else()
  message(WARNING "simpleini not found; trying system include then third-party fallback")

  # 2) 시스템 헤더(예: /usr/include, vcpkg include 등)로 설치돼 있는지 먼저 탐색
  #    찾히면 그 경로로 인터페이스 타깃 구성
  find_path(SIMPLEINI_INCLUDE_DIR
    NAMES SimpleIni.h
    PATH_SUFFIXES include
  )

  if (SIMPLEINI_INCLUDE_DIR)
    message(STATUS "Found SimpleIni.h in system include: ${SIMPLEINI_INCLUDE_DIR}")

    add_library(j2_simpleini INTERFACE)
    target_include_directories(j2_simpleini INTERFACE
      $<BUILD_INTERFACE:${SIMPLEINI_INCLUDE_DIR}>
      $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    )

    # 소비 코드에서 일관된 이름을 쓰도록 표준 별칭 제공
    add_library(simpleini::simpleini ALIAS j2_simpleini)

    # 시스템 헤더는 우리가 설치하지 않습니다(이미 시스템에 있음). export만 함께.
    install(TARGETS j2_simpleini
            EXPORT j2_libraryTargets)

  else()
    # 3) 최종 폴백: git submodule 등 로컬 코드 사용
    #    예) j2_library/third-party/simpleini/SimpleIni.h
    set(SIMPLEINI_LOCAL_INC "${J2_THIRD_PARTY_ROOT_PATH}/simpleini")
    if (NOT EXISTS "${SIMPLEINI_LOCAL_INC}/SimpleIni.h")
      message(FATAL_ERROR "SimpleIni.h not found. Expected at: ${SIMPLEINI_LOCAL_INC}/SimpleIni.h")
    endif()

    add_library(j2_simpleini INTERFACE)
    target_include_directories(j2_simpleini INTERFACE
      $<BUILD_INTERFACE:${SIMPLEINI_LOCAL_INC}>
      $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    )
    add_library(simpleini::simpleini ALIAS j2_simpleini)

    # 로컬 헤더를 설치(설치 후 INSTALL_INTERFACE 경로가 유효해짐)
    install(FILES "${SIMPLEINI_LOCAL_INC}/SimpleIni.h"
            DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

    # export 세트에 포함 (다른 타깃들과 동일 export로 묶음)
    install(TARGETS j2_simpleini
            EXPORT j2_libraryTargets)
  endif()
endif()

###############################
# ---- Boost.Asio 우선, 실패 시 standalone Asio 폴백 ----

# vcpkg/MSVC 환경이면 통상 CONFIG 모드가 잘 동작합니다.
# 필요 시 MODULE 모드도 병행하거나 최소 버전을 지정하세요.
find_package(Boost 1.74 CONFIG QUIET COMPONENTS asio system)

add_library(j2_asio INTERFACE)

if (Boost_FOUND)
  message(STATUS "Boost found. Using Boost headers (+ Boost.System if available).")

  # 1) 헤더 타깃 우선(Boost::headers가 있으면 그것을, 없으면 Boost::boost 대안)
  if (TARGET Boost::headers)
    target_link_libraries(j2_asio INTERFACE Boost::headers)
  elseif (TARGET Boost::boost)
    target_link_libraries(j2_asio INTERFACE Boost::boost)
  elseif (DEFINED Boost_INCLUDE_DIRS)
    target_include_directories(j2_asio INTERFACE ${Boost_INCLUDE_DIRS})
  endif()

  # 2) system 라이브러리는 실제로 필요하므로 타깃이 있으면 연결
  if (NOT TARGET Boost::system)
    # 어떤 배포판은 CONFIG에 system 타깃이 없을 수 있어 보완
    find_package(Boost 1.74 REQUIRED COMPONENTS system)
  endif()
  target_link_libraries(j2_asio INTERFACE Boost::system)

else()
  message(WARNING "Boost not found; falling back to standalone Asio (ASIO_STANDALONE)")

  set(ASIO_STANDALONE_INC "${J2_THIRD_PARTY_ROOT_PATH}/asio/asio/include")
  if (NOT EXISTS "${ASIO_STANDALONE_INC}")
    message(FATAL_ERROR "Standalone Asio include path not found: ${ASIO_STANDALONE_INC}")
  endif()

  target_include_directories(j2_asio INTERFACE
    $<BUILD_INTERFACE:${ASIO_STANDALONE_INC}>
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
  )
  target_compile_definitions(j2_asio INTERFACE ASIO_STANDALONE)
  if (WIN32)
    target_link_libraries(j2_asio INTERFACE ws2_32)
  endif()

  # (원하면) standalone 헤더 동반 설치
  install(DIRECTORY "${ASIO_STANDALONE_INC}/"
          DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
          FILES_MATCHING PATTERN "*.hpp" PATTERN "*.h")
endif()

target_link_libraries(j2_asio INTERFACE Threads::Threads)

install(TARGETS j2_asio
        EXPORT j2_libraryTargets)


  

################################
# curl 라이브러리 (필수: 반드시 find_package(CURL REQUIRED)로 탐색, 없으면 에러)

find_package(CURL REQUIRED CONFIG)
if(NOT CURL_FOUND)
    find_package(CURL REQUIRED MODULE)
endif()

if (CURL_FOUND)
  message(STATUS "Found CURL from package manager")
else()
  message(FATAL_ERROR "CURL not found: install via package manager or add to third-party/curl")
endif()


################################
# googletest 라이브러리

# GTest 임시 비활성화 (해당 프로그램에서 직접 정의하여 사용)
if(FALSE) 

    find_package(GTest CONFIG QUIET)
    if (GTest_FOUND)
      message(STATUS "Found GTest from package manager")
      # add_subdirectory를 호출하지 않음!
    else()
      message(WARNING "GTest not found, using third-party vendor")
      set(GTEST_THIRD_PARTY_PATH "${J2_THIRD_PARTY_ROOT_PATH}/googletest")
      if (EXISTS "${GTEST_THIRD_PARTY_PATH}/CMakeLists.txt")
        add_subdirectory(${GTEST_THIRD_PARTY_PATH} EXCLUDE_FROM_ALL)
      else()
        message(FATAL_ERROR "GTest not found: install via package manager or    add to third-party/googletest")
      endif()
    endif()
endif()

#######################################
# --- immer: 패키지 우선, 실패 시 서브모듈/로컬 헤더 폴백 ---
# 
# if(FALSE) # # immer 임시 비활성화
# 
#   # 1) 패키지 매니저(vcpkg/Conan 등)에서 CONFIG 패키지 시도
#   find_package(immer CONFIG QUIET)
# 
#   # immer 타깃 이름은 배포에 따라 다릅니다(immer::immer 또는 immer).
#   # 단일 공개 래퍼 타깃(j2_immer)을 항상 제공해, 소비 측은 j2_immer만 링크하면 됩니다.
#   add_library(j2_immer INTERFACE)
# 
#   if (immer_FOUND)
#     message(STATUS "Found immer from package manager")
# 
#     if (TARGET immer::immer)
#       target_link_libraries(j2_immer INTERFACE immer::immer)
#     elseif (TARGET immer)
#       target_link_libraries(j2_immer INTERFACE immer)
#     else()
#       message(FATAL_ERROR
#         "immer package was found, but no known target appeared (expected 'immer::immer'  #or 'immer').")
#     endif()
# 
#   else()
#     message(WARNING "immer not found; using third-party vendor (header-only)")
# 
#     # 2) 서브모듈/로컬 경로에서 헤더 루트 자동 탐지
#     #    - 레포 구조가 'third-party/immer/immer/...'
#     #      또는 'third-party/immer/include/immer/...' 인 두 경우를 모두 지원
#     set(_IMMER_ROOT "${J2_THIRD_PARTY_ROOT_PATH}/immer")
#     set(IMMER_LOCAL_INC "")
#     if (EXISTS "${_IMMER_ROOT}/immer/vector.hpp" OR EXISTS "${_IMMER_ROOT}/immer/ #immer.hpp")
#       set(IMMER_LOCAL_INC "${_IMMER_ROOT}")                # .../third-party/immer
#     elseif (EXISTS "${_IMMER_ROOT}/include/immer/vector.hpp" OR EXISTS "${_IMMER_ROOT}/ #include/immer/immer.hpp")
#       set(IMMER_LOCAL_INC "${_IMMER_ROOT}/include")       # .../third-party/immer/ include
#     else()
#       message(FATAL_ERROR
#         "immer headers not found under: ${_IMMER_ROOT}\n"
#         "Expected 'immer/...' or 'include/immer/...'.")
#     endif()
# 
#     # 빌드/설치 인터페이스 분리 (설치 산출물에서 소스 경로가 보이지 않도록)
#     target_include_directories(j2_immer INTERFACE
#       $<BUILD_INTERFACE:${IMMER_LOCAL_INC}>
#       $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
#     )
# 
#     # immer는 최소 C++14 요구 (프로젝트 전역이 C++17이라도 명시해 두면 안전)
#     target_compile_features(j2_immer INTERFACE cxx_std_14)
# 
#     # 소비 코드가 'immer::immer'를 기대하는 경우를 대비해 별칭 제공(이미 있으면 생략)
#     if (NOT TARGET immer::immer)
#       add_library(immer::immer ALIAS j2_immer)
#     endif()
# 
#     # 로컬 헤더 설치 (include/immer/* 로 배치)
#     install(DIRECTORY "${IMMER_LOCAL_INC}/immer"
#             DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
#   endif()
# 
#   # 항상 j2_immer를 export 세트에 포함 (j2_library가 PUBLIC으로 링크해도 export 누락 #에 러 방지)
#   install(TARGETS j2_immer
#           EXPORT j2_libraryTargets)
# 
# endif()


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

# 스레드 라이브러리
target_link_libraries(j2_library PRIVATE Threads::Threads)  

# nlohmann_json 라이브러리
target_link_libraries(j2_library PUBLIC nlohmann_json::nlohmann_json)  

# spdlog 라이브러리
target_link_libraries(j2_library PUBLIC spdlog::spdlog)

# Asio 라이브러리 (Boost.Asio 또는 standalone Asio)
target_link_libraries(j2_library PUBLIC j2_asio)

# SimpleIni 라이브러리
target_link_libraries(j2_library PUBLIC simpleini::simpleini)

# curl 라이브러리 링크
if (CURL_FOUND)
  target_link_libraries(j2_library PUBLIC CURL::libcurl)
elseif (TARGET libcurl)
  target_link_libraries(j2_library PUBLIC libcurl)
endif()

# googletest 라이브러리 링크 추가
if (GTest_FOUND)
  target_link_libraries(j2_library PUBLIC GTest::gtest GTest::gtest_main)
elseif (TARGET gtest)
  target_link_libraries(j2_library PUBLIC gtest gtest_main)
endif()


# immer 라이브러리
if(FALSE)  # immer 임시 비활성화
    target_link_libraries(j2_library PUBLIC j2_immer) 
endif()

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

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/generated/j2_library/version.hpp
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










