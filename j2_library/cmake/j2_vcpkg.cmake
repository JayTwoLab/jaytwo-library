#########################################
# include_guard(GLOBAL) # 전역 범위(Global scope) 에서 중복 include를 막습니다.
include_guard(DIRECTORY)

###################################
# vcpkg를 활용하는 공통 플랫폼/컴파일러 감지 및 기본 옵션 적용 모듈
# 사용자는 DCPV_* 변수로 동작을 커스터마이즈할 수 있습니다.
###################################

# 옵션 예시 (include 전/후 어디서든 설정 가능)
# set(DCPV_VCPKG_TRIPLET "x64-windows")  # 강제 트립렛 지정
# set(DCPV_SILENT ON)                    # 상태 메시지 억제
# set(DCPV_ENABLE_THREADS OFF)           # Threads 비활성(기본 ON)

function(setup_platform_defaults_with_vcpkg) # vcpkg 설정 적용
  # 사용자 계정 감지
  set(_CURRENT_USER "$ENV{USERNAME}")
  if (NOT _CURRENT_USER)
    set(_CURRENT_USER "$ENV{USER}")
  endif()
  if (NOT _CURRENT_USER)
    set(_CURRENT_USER "unknown")
  endif()

  # 아키텍처 비트수 (32bit/64bit)
  if (CMAKE_SIZEOF_VOID_P EQUAL 8)
    set(_ARCH_BITS 64)
  else()
    set(_ARCH_BITS 32)
  endif()

  # 메시지 헬퍼 (조용한 모드 지원)
  if (DCPV_SILENT)
    function(_dcpv_msg)
    endfunction()
  else()
    function(_dcpv_msg)
      message(STATUS "${ARGN}")
    endfunction()
  endif()

  _dcpv_msg("Current User Account: ${_CURRENT_USER}")
  _dcpv_msg("System: ${CMAKE_SYSTEM_NAME}, Compiler: ${CMAKE_CXX_COMPILER_ID}, ARCH: ${_ARCH_BITS}-bit")

  # vcpkg 루트 추론 (원하시면 ENV 우선으로 바꿔도 됨)
  if (WIN32)
    set(_VCPKG_ROOT "C:/Users/${_CURRENT_USER}/vcpkg")
  elseif(APPLE)
    set(_VCPKG_ROOT "/Users/${_CURRENT_USER}/vcpkg")
  else()
    set(_VCPKG_ROOT "/home/${_CURRENT_USER}/vcpkg")
  endif()

  # 기본 트립렛 자동 추론
  set(_AUTO_TRIPLET "")
  if (MSVC)
    set(_AUTO_TRIPLET "x64-windows-static")  # 필요시 x64-windows
  elseif(MINGW)
    set(_AUTO_TRIPLET "x64-mingw-static") # 필요시 x64-mingw
  elseif(APPLE)
    set(_AUTO_TRIPLET "x64-osx")
  elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND UNIX AND NOT APPLE)
    set(_AUTO_TRIPLET "x64-linux")
  elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang" AND UNIX AND NOT APPLE)
    set(_AUTO_TRIPLET "x64-linux")
  endif()

  # 사용자 지정 트립렛 우선
  if (DEFINED DCPV_VCPKG_TRIPLET AND NOT "${DCPV_VCPKG_TRIPLET}" STREQUAL "")
    set(_TRIPLET "${DCPV_VCPKG_TRIPLET}")
  else()
    set(_TRIPLET "${_AUTO_TRIPLET}")
  endif()

  # 플랫폼/컴파일러별 공통 옵션 (발췌)
  if (MSVC)
    _dcpv_msg("[MSVC] Visual C++ 설정 적용")
    add_compile_options(/W4 /permissive-)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
    add_compile_options(
      $<$<CXX_COMPILER_ID:MSVC>:/utf-8>
      $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:-finput-charset=UTF-8>
      $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:-fexec-charset=UTF-8>
    )
  elseif(MINGW)
    _dcpv_msg("[MinGW] Apply MinGW settings")
    add_compile_options(-Wall -Wextra -Wpedantic -O2)
  elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND UNIX AND NOT APPLE)
    _dcpv_msg("[Linux/GNU] Apply g++ settings")
    add_compile_options(-Wall -Wextra -Wpedantic -O2)
  elseif(APPLE AND (CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang"))
    _dcpv_msg("[macOS/Clang] Apply AppleClang/Clang settings")
    add_compile_options(-Wall -Wextra -Wpedantic -O2)
  elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang" AND UNIX AND NOT APPLE)
    _dcpv_msg("[Linux/Clang] Apply clang++ settings")
    add_compile_options(-Wall -Wextra -Wpedantic -O2)
  else()
    _dcpv_msg("[Other] Apply General Settings (e.g., Custom Toolchain)")
    add_compile_options(-Wall -Wextra -Wpedantic -O2)
  endif()

  # Threads (기본 ON)
  if (NOT DEFINED DCPV_ENABLE_THREADS OR DCPV_ENABLE_THREADS)
    set(THREADS_PREFER_PTHREAD_FLAG ON)
    find_package(Threads REQUIRED)
  endif()

  # ----------------------------------------------------
  # CMAKE_PREFIX_PATH 캐시와 "병합"하여 vcpkg prefix 반영 (중요)
  # ----------------------------------------------------
  set(_VCPKG_PREFIX "")
  if (_TRIPLET)
    file(TO_CMAKE_PATH "${_VCPKG_ROOT}/installed/${_TRIPLET}" _VCPKG_PREFIX)
    if (EXISTS "${_VCPKG_PREFIX}")
      # 1) 기존 캐시값 읽어오기
      if (DEFINED CMAKE_PREFIX_PATH)
        set(_CPP_OLD "${CMAKE_PREFIX_PATH}")
      else()
        set(_CPP_OLD "")
      endif()

      # 2) 이미 포함되어 있지 않다면 병합
      set(_NEED_APPEND TRUE)
      if (_CPP_OLD)
        string(FIND "${_CPP_OLD}" "${_VCPKG_PREFIX}" _found)
        if (NOT _found EQUAL -1)
          set(_NEED_APPEND FALSE)
        endif()
      endif()

      if (_NEED_APPEND)
        if (_CPP_OLD STREQUAL "")
          set(_CPP_NEW "${_VCPKG_PREFIX}")
        else()
          set(_CPP_NEW "${_CPP_OLD};${_VCPKG_PREFIX}")
        endif()
      else()
        set(_CPP_NEW "${_CPP_OLD}")
      endif()

      # 3) 캐시에 FORCE로 반영 (Qt가 준 -D 값을 덮는 핵심)
      set(CMAKE_PREFIX_PATH "${_CPP_NEW}" CACHE STRING "Prefix paths for find_package()" FORCE)

      # 필요시, 현재 디렉터리 스코프도 동기화(가독용)
      set(CMAKE_PREFIX_PATH "${_CPP_NEW}")

      _dcpv_msg("Add vcpkg Path: ${_VCPKG_PREFIX}")
      _dcpv_msg("Merged CMAKE_PREFIX_PATH=${CMAKE_PREFIX_PATH}")
    else()
      _dcpv_msg("vcpkg path does not exist: ${_VCPKG_PREFIX}")
    endif()
  else()
    _dcpv_msg("Triplet auto inference failure or unspecified: omit adding vcpkg path")
  endif()

  # parent scope로 결과 노출 (힌트/로그용)
  set(CURRENT_USER "${_CURRENT_USER}" PARENT_SCOPE)
  set(ARCH_BITS "${_ARCH_BITS}" PARENT_SCOPE)
  set(DCPV_EFFECTIVE_VCPKG_ROOT "${_VCPKG_ROOT}" PARENT_SCOPE)
  set(DCPV_EFFECTIVE_TRIPLET "${_TRIPLET}" PARENT_SCOPE)
  set(DCPV_EFFECTIVE_VCPKG_PREFIX "${_VCPKG_PREFIX}" PARENT_SCOPE)
endfunction()
