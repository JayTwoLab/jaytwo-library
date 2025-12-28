#########################################
# include_guard(GLOBAL) # 전역 범위(Global scope) 에서 중복 include를 막습니다.
include_guard(DIRECTORY)

###########################################################
# 스레드 라이브러리

find_package(Threads REQUIRED) 

###########################################################
# nlohmann_json {{
#
# nlohmann_json 패키지 우선, 실패 시 로컬 vendor 인터페이스 타깃 구성 + 같은 export set 포함

# prefer package >= 3.10.0; if not available fall back to bundled vendor
find_package(nlohmann_json CONFIG QUIET)

if (nlohmann_json_FOUND)
  message(STATUS "Found nlohmann_json from package manager")
 
else()
  message(WARNING "nlohmann_json not found, using third-party vendor (header-only)")
  
  # 로컬(서브모듈) 헤더 경로
  set(NLOHMANN_JSON_INC "${J2_THIRD_PARTY_ROOT_PATH}/json/include")
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

# nlohmann_json }}
###########################################################

###########################################################
# spdlog {{
#
# 패키지 우선, 없으면 third-party 사용

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

# spdlog }}
###########################################################


###########################################################
# simpleini {{
# 
# 패키지 우선, 실패 시 시스템 헤더 또는 서브모듈(로컬 코드) 폴백 
#
# 1) vcpkg/Conan 등에서 CONFIG 패키지 시도 (있으면 simpleini::simpleini 타깃을 씁니다)
#
# if (FALSE) # simpleini 임시 비활성화
# 
#   find_package(simpleini CONFIG QUIET)
# 
#   message(STATUS "simpleini_FOUND = ${simpleini_FOUND}")
#   message(STATUS "simpleini_DIR = ${simpleini_DIR}")
# 
#   if (simpleini_FOUND)
#     message(STATUS "Found simpleini from package manager")
#     # add_subdirectory를 호출하지 않음!
#   else()
#     message(WARNING "simpleini not found; trying system include then third-party fallback")
# 
#     # 2) 시스템 헤더(예: /usr/include, vcpkg include 등)로 설치돼 있는지 먼저 탐색
#     #    찾히면 그 경로로 인터페이스 타깃 구성
#     find_path(SIMPLEINI_INCLUDE_DIR
#       NAMES SimpleIni.h
#       PATH_SUFFIXES include
#     )
# 
#     if (SIMPLEINI_INCLUDE_DIR)
#       message(STATUS "Found SimpleIni.h in system include: ${SIMPLEINI_INCLUDE_DIR}")
# 
#       add_library(j2_simpleini INTERFACE)
#       target_include_directories(j2_simpleini INTERFACE
#         $<BUILD_INTERFACE:${SIMPLEINI_INCLUDE_DIR}>
#         $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
#       )
# 
#       # 소비 코드에서 일관된 이름을 쓰도록 표준 별칭 제공
#       add_library(simpleini::simpleini ALIAS j2_simpleini)
# 
#       # 시스템 헤더는 우리가 설치하지 않습니다(이미 시스템에 있음). export만 함께.
#       install(TARGETS j2_simpleini
#               EXPORT j2_libraryTargets)
# 
#     else()
#       # 3) 최종 폴백: git submodule 등 로컬 코드 사용
#       #    예) j2_library/third-party/simpleini/SimpleIni.h
#       set(SIMPLEINI_LOCAL_INC "${J2_THIRD_PARTY_ROOT_PATH}/simpleini")
#       if (NOT EXISTS "${SIMPLEINI_LOCAL_INC}/SimpleIni.h")
#         message(FATAL_ERROR "SimpleIni.h not found. Expected at: ${SIMPLEINI_LOCAL_INC}/ #SimpleIni.h")
#       endif()
# 
#       add_library(j2_simpleini INTERFACE)
#       target_include_directories(j2_simpleini INTERFACE
#         $<BUILD_INTERFACE:${SIMPLEINI_LOCAL_INC}>
#         $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
#       )
#       add_library(simpleini::simpleini ALIAS j2_simpleini)
# 
#       # 로컬 헤더를 설치(설치 후 INSTALL_INTERFACE 경로가 유효해짐)
#       install(FILES "${SIMPLEINI_LOCAL_INC}/SimpleIni.h"
#               DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})
# 
#       # export 세트에 포함 (다른 타깃들과 동일 export로 묶음)
#       install(TARGETS j2_simpleini
#               EXPORT j2_libraryTargets)
#     endif()
#   endif()
# endif()
#
# simpleini }}
###########################################################

###########################################################
# asio {{
# 
# Boost.Asio 우선, 실패 시 standalone Asio 폴백 ----
#
# vcpkg/MSVC 환경이면 통상 CONFIG 모드가 잘 동작합니다.
# 필요 시 MODULE 모드도 병행하거나 최소 버전을 지정하세요.
#
# if (FALSE)
# 
#       find_package(Boost 1.74 CONFIG QUIET COMPONENTS asio system)
# 
#       add_library(j2_asio INTERFACE)
# 
#       if (Boost_FOUND)
#         message(STATUS "Boost found. Using Boost headers (+ Boost.System if available).")
# 
#         # 1) 헤더 타깃 우선(Boost::headers가 있으면 그것을, 없으면 Boost::boost 대안)
#         if (TARGET Boost::headers)
#           target_link_libraries(j2_asio INTERFACE Boost::headers)
#         elseif (TARGET Boost::boost)
#           target_link_libraries(j2_asio INTERFACE Boost::boost)
#         elseif (DEFINED Boost_INCLUDE_DIRS)
#           target_include_directories(j2_asio INTERFACE ${Boost_INCLUDE_DIRS})
#         endif()
# 
#         # 2) system 라이브러리는 실제로 필요하므로 타깃이 있으면 연결
#         if (NOT TARGET Boost::system)
#           # 어떤 배포판은 CONFIG에 system 타깃이 없을 수 있어 보완
#           find_package(Boost 1.74 REQUIRED COMPONENTS system)
#         endif()
#         target_link_libraries(j2_asio INTERFACE Boost::system)
# 
#       else()
#         message(WARNING "Boost not found; falling back to standalone Asio (ASIO_STANDALONE)")
# 
#         set(ASIO_STANDALONE_INC "${J2_THIRD_PARTY_ROOT_PATH}/asio/asio/include")
#         if (NOT EXISTS "${ASIO_STANDALONE_INC}")
#           message(FATAL_ERROR "Standalone Asio include path not found: ${ASIO_STANDALONE_INC}")
#         endif()
# 
#         target_include_directories(j2_asio INTERFACE
#           $<BUILD_INTERFACE:${ASIO_STANDALONE_INC}>
#           $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
#         )
#         target_compile_definitions(j2_asio INTERFACE ASIO_STANDALONE)
#         if (WIN32)
#           target_link_libraries(j2_asio INTERFACE ws2_32)
#         endif()
# 
#         # (원하면) standalone 헤더 동반 설치
#         install(DIRECTORY "${ASIO_STANDALONE_INC}/"
#                 DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
#                 FILES_MATCHING PATTERN "*.hpp" PATTERN "*.h")
#       endif()
# 
#       target_link_libraries(j2_asio INTERFACE Threads::Threads)
# 
#       install(TARGETS j2_asio
#               EXPORT j2_libraryTargets)
# 
# endif()
#
# asio }}
###########################################################

###########################################################
# curl {{
#
# 필수: 반드시 find_package(CURL REQUIRED)로 탐색, 없으면 에러)

# find_package(CURL REQUIRED)
# 
# if (CURL_FOUND)
#   message(STATUS "Found CURL (system libcurl)")
# else()
#   message(FATAL_ERROR "CURL not found: install 'libcurl-devel' via package manager")
# endif()

# curl }}
###########################################################


###########################################################
# googletest {{
# 
# GTest 임시 비활성화 (해당 프로그램에서 직접 정의하여 사용)
# if(FALSE) 
# 
#     find_package(GTest CONFIG QUIET)
#     if (GTest_FOUND)
#       message(STATUS "Found GTest from package manager")
#       # add_subdirectory를 호출하지 않음!
#     else()
#       message(WARNING "GTest not found, using third-party vendor")
#       set(GTEST_THIRD_PARTY_PATH "${J2_THIRD_PARTY_ROOT_PATH}/googletest")
#       if (EXISTS "${GTEST_THIRD_PARTY_PATH}/CMakeLists.txt")
#         add_subdirectory(${GTEST_THIRD_PARTY_PATH} EXCLUDE_FROM_ALL)
#       else()
#         message(FATAL_ERROR "GTest not found: install via package manager or    add to third-party/ #googletest")
#       endif()
#     endif()
# endif()
#
# googletest }}
###########################################################

###########################################################
# immer {{
# 
# 패키지 우선, 실패 시 서브모듈/로컬 헤더 폴백
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
#
# immer }}
###########################################################


###########################################################
# tinyxml2 {{
# 
# tinyxml2: prefer package (vcpkg/Conan), else use third-party fallback
#
# Behavior:
# 1) If CONFIG package found (tinyxml2::tinyxml2), use it.
# 2) Else if third-party submodule provides CMakeLists.txt, add_subdirectory().
# 3) Else if third-party contains tinyxml2.cpp/.h, build a vendor static target.
# 4) Expose a consistent alias tinyxml2::tinyxml2 for consumer code.
#
#   find_package(tinyxml2 CONFIG QUIET)
#   
#   if (tinyxml2_FOUND)
#     message(STATUS "Found tinyxml2 from package manager")
#   else()
#     message(WARNING "tinyxml2 not found; trying third-party fallback")
#   
#     set(TINYXML2_THIRD_PARTY_PATH "${J2_THIRD_PARTY_ROOT_PATH}/tinyxml2")
#   
#     if (EXISTS "${TINYXML2_THIRD_PARTY_PATH}/CMakeLists.txt")
#       add_subdirectory("${TINYXML2_THIRD_PARTY_PATH}" EXCLUDE_FROM_ALL)
#       # Expect subproject to create a 'tinyxml2' target.
#     elseif (EXISTS "${TINYXML2_THIRD_PARTY_PATH}/tinyxml2.h" AND EXISTS "${TINYXML2_THIRD_PARTY_PATH}/  #tinyxml2.cpp")
#       # Build vendor static library(*.a *.lib) from vendor source
#       add_library(tinyxml2_vendor STATIC
#         "${TINYXML2_THIRD_PARTY_PATH}/tinyxml2.cpp"
#       )
#   
#       target_include_directories(tinyxml2_vendor PUBLIC
#         $<BUILD_INTERFACE:${TINYXML2_THIRD_PARTY_PATH}>
#         $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
#       )
#   
#       # Provide consistent alias used by consumers
#       add_library(tinyxml2::tinyxml2 ALIAS tinyxml2_vendor)
#   
#       # Install vendor target and headers
#       install(TARGETS tinyxml2_vendor
#               EXPORT j2_libraryTargets
#               ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
#               LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
#               RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
#       )
#   
#       install(DIRECTORY "${TINYXML2_THIRD_PARTY_PATH}/"
#               DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
#               FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp")
#     else()
#       message(FATAL_ERROR "tinyxml2 fallback not found at: ${TINYXML2_THIRD_PARTY_PATH}")
#     endif()
#   endif()
#   
#   # Ensure consistent alias if upstream package created an un-namespaced target
#   if (NOT TARGET tinyxml2::tinyxml2)
#     if (TARGET tinyxml2)
#       add_library(tinyxml2::tinyxml2 ALIAS tinyxml2)
#     endif()
#   endif()

# }} tinyxml2
###########################################################

# 플랫폼별 네트워크 라이브러리 링크 추가
include(j2_network_platform_pre_module)


