#########################################
# include_guard(GLOBAL) # 전역 범위(Global scope) 에서 중복 include를 막습니다.
include_guard(DIRECTORY)

#########################################
# 소스코드 인코딩 설정 모듈 (UTF-8 사용)
include(j2_utf8) # j2_utf8.cmake 모듈 포함

#########################################
# MSVC 런타임 선택 정책 활성화
if (MSVC)
  cmake_policy(SET CMP0091 NEW)
  add_compile_options(/wd4251)
endif()

#########################################
# MSVC 전역 정적 CRT 강제(/MT, /MTd)
if (MSVC)
  set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>" CACHE STRING "" FORCE)
endif()

#########################################
# 경고/샌리타이저 모듈(있으면 사용)
if (COMMAND j2_enable_warnings)
  j2_enable_warnings(j2_library) # j2_warnings.cmake 모듈 포함
endif()

if (COMMAND j2_enable_sanitizers)
  j2_enable_sanitizers(j2_library) # j2_sanitize.cmake 모듈 포함
endif()

#########################################
# vcpkg 설정 적용

if (MSVC) # Visual Studio 인 경우 vcpkg 사용 (테스트 환경: VS2022 x64)
  option(USE_VCPKG "[MSVC] Use vcpkg." ON)
endif()

if (MINGW) #MingW 인 경우 vcpkg 사용 (테스트 환경: MingW 13.1.0 x64)
  option(USE_VCPKG "[MINGW] Use vcpkg." ON)
endif()

# 현재 리눅스 환경에서는 vcpkg 사용 안함. 
# if (UNIX AND NOT MSVC AND NOT MINGW)
#   option(USE_VCPKG "[LINUX] Use vcpkg." ON)
#
#	# /etc/os-release 파일에서 ID 값 읽기
#	file(READ "/etc/os-release" OS_RELEASE_CONTENT)
#	string(REGEX MATCH "ID=([a-zA-Z0-9_-]+)" _ ${OS_RELEASE_CONTENT})
#	set(OS_ID "${CMAKE_MATCH_1}")
#
#	# 배포판별 조건 분기
#	if(OS_ID STREQUAL "ubuntu")
#	  message(STATUS "Detected Ubuntu Linux")
#	elseif(OS_ID STREQUAL "rocky" OR OS_ID STREQUAL "centos" OR OS_ID STREQUAL "rhel")
#	  message(STATUS "Detected Rocky/CentOS/RedHat Linux")
#	else()
#	  message(STATUS "Detected other Linux: ${OS_ID}")
#	endif
# 
# endif()

if (USE_VCPKG)
  message("-- Use vcpkg. --")
  include(j2_vcpkg) # j2_vcpkg.cmake 모듈 포함
  setup_platform_defaults_with_vcpkg() # vcpkg를 사용한 플랫폼 기본 설정
endif()

