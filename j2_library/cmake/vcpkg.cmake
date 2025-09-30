#########################################
# 전역 범위(Global scope) 에서 중복 include를 막습니다.
include_guard(GLOBAL)

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

  # vcpkg 루트를 추론한다.
  if (WIN32) # Windows
   set(_VCPKG_ROOT "C:/Users/${_CURRENT_USER}/vcpkg")
  elseif(APPLE) # macOS
   set(_VCPKG_ROOT "/Users/${_CURRENT_USER}/vcpkg")
  else() # Linux 및 기타 UNIX 
   set(_VCPKG_ROOT "/home/${_CURRENT_USER}/vcpkg")
  endif()

  # vcpkg 루트를 환경변수 우선하는 방법은 다음과 같다.
  # set(_VCPKG_ROOT "$ENV{VCPKG_ROOT}")
  # if (NOT _VCPKG_ROOT)
  # ....
  # endif()

  # 기본 트립렛 자동 추론
  set(_AUTO_TRIPLET "")
  if (MSVC) # Visual Studio
    set(_AUTO_TRIPLET "x64-windows")
    # set(_AUTO_TRIPLET "x64-windows-static")
  elseif(MINGW) # MinGW
    # 정적/동적 필요에 따라 교체 가능: x64-mingw 또는 x64-mingw-static
    set(_AUTO_TRIPLET "x64-mingw-static")
  elseif(APPLE) # macOS
    set(_AUTO_TRIPLET "x64-osx")
  elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND UNIX AND NOT APPLE) # Linux + g++
    set(_AUTO_TRIPLET "x64-linux")
  elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang" AND UNIX AND NOT APPLE) # Linux + clang++
    set(_AUTO_TRIPLET "x64-linux")
  endif()

  # 사용자 지정 트립렛 우선
  if (DEFINED DCPV_VCPKG_TRIPLET AND NOT "${DCPV_VCPKG_TRIPLET}" STREQUAL "") # 사용자가 지정한 트립렛
    set(_TRIPLET "${DCPV_VCPKG_TRIPLET}")
  else()
    set(_TRIPLET "${_AUTO_TRIPLET}")
  endif()

  # 플랫폼/컴파일러별 공통 옵션
  if (MSVC) # Visual Studio
    _dcpv_msg("[MSVC] Visual C++ 설정 적용")
    add_compile_options(/W4 /permissive-)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
    # 필요시 런타임 라이브러리 정책:
    # set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    # set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreadedDLL$<$<CONFIG:Debug>:Debug>")
    # 
    # 모든 타겟에 공통 적용하려면:
    add_compile_options(
      # MSVC: 소스/실행 문자셋을 모두 UTF-8로 고정
      $<$<CXX_COMPILER_ID:MSVC>:/utf-8>

      # GCC/Clang: 소스 입력/실행 문자셋을 UTF-8로 고정
      $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:-finput-charset=UTF-8>
      $<$<OR:$<CXX_COMPILER_ID:GNU>,$<CXX_COMPILER_ID:Clang>>:-fexec-charset=UTF-8>
    )
    # (선택) 리소스(.rc) 파일을 쓴다면 MSVC rc.exe에서 안전하게 처리
    #  └ rc.exe는 케이스가 다양하므로 .rc 파일 맨 위에 다음 지시문을 추천:
    #     #pragma code_page(65001)
    #  CMake로 일괄 플래그 주입이 필요하면(환경에 따라 동작 다름, 지양):
    # set(CMAKE_RC_FLAGS "${CMAKE_RC_FLAGS} /nologo")
  elseif(MINGW)
    _dcpv_msg("[MinGW] Apply MinGW settings")
    add_compile_options(-Wall -Wextra -Wpedantic -O2)
  elseif(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" AND UNIX AND NOT APPLE)
    _dcpv_msg("[Linux/GNU] Apply g++ settings")
    add_compile_options(-Wall -Wextra -Wpedantic -O2)
  elseif(APPLE AND (CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang"))
    _dcpv_msg("[macOS/Clang] Apply AppleClang/Clang settings")
    add_compile_options(-Wall -Wextra -Wpedantic -O2)
    # 필요시 libc++ 강제:
    # add_compile_options(-stdlib=libc++)
    # set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -stdlib=libc++")
  elseif(CMAKE_CXX_COMPILER_ID STREQUAL "Clang" AND UNIX AND NOT APPLE)
    _dcpv_msg("[Linux/Clang] Apply clang++ settings")
    add_compile_options(-Wall -Wextra -Wpedantic -O2)
  else()
    _dcpv_msg("[Other] Apply General Settings (e.g., Custom Toolchain)")
    add_compile_options(-Wall -Wextra -Wpedantic -O2)
  endif()

  # Threads (기본 ON)
  if (NOT DEFINED DCPV_ENABLE_THREADS OR DCPV_ENABLE_THREADS) # 기본값 ON
    set(THREADS_PREFER_PTHREAD_FLAG ON)
    find_package(Threads REQUIRED)
  endif()

  # CMAKE_PREFIX_PATH에 vcpkg 경로 추가 (존재 시, 누적 방식)
  if (_TRIPLET) # 트립렛이 유효할 때만
    file(TO_CMAKE_PATH "${_VCPKG_ROOT}/installed/${_TRIPLET}" _VCPKG_PREFIX)
    if (EXISTS "${_VCPKG_PREFIX}")
      list(APPEND CMAKE_PREFIX_PATH "${_VCPKG_PREFIX}")
      _dcpv_msg("Add vcpkg Path: ${_VCPKG_PREFIX}")
    else()
      _dcpv_msg("vcpkg path does not exist: ${_VCPKG_PREFIX}")
    endif()
  else()
    _dcpv_msg("Triplet auto inference failure or unspecified: omit adding vcpkg path")
  endif()

  # parent scope로 결과 노출
  set(CURRENT_USER "${_CURRENT_USER}" PARENT_SCOPE)
  set(ARCH_BITS "${_ARCH_BITS}" PARENT_SCOPE)
  set(DCPV_EFFECTIVE_VCPKG_ROOT "${_VCPKG_ROOT}" PARENT_SCOPE)
  set(DCPV_EFFECTIVE_TRIPLET "${_TRIPLET}" PARENT_SCOPE)

endfunction()
