cmake_minimum_required(VERSION 3.26)

project(j2_tcp_server LANGUAGES CXX) # 프로젝트 선언

set(EXE_NAME "j2_tcp_server")

set(CMAKE_CXX_STANDARD 17) # C++17
set(CMAKE_CXX_STANDARD_REQUIRED ON) # 지정한 표준을 반드시 사용
set(CMAKE_CXX_EXTENSIONS OFF) # OFF: 컴파일러 확장 기능을 쓰지 않고 순수한 표준 모드만 사용

########################################################
# {{

# j2_library 루트 경로 설정 (전역 변수)
set(J2_LIBRARY_ROOT_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../../j2_library" CACHE PATH "Path to j2_library root")

list(PREPEND CMAKE_MODULE_PATH "${J2_LIBRARY_ROOT_PATH}/cmake")
include(j2_cmake) # j2_cmake.cmake 모듈 포함 (필요한 모듈만 선택적으로 포함도 가능)

# j2_library 라이브러리 루트 디렉토리 설정 : j2_library 라이브러리를 같은 소스 트리에서 빌드
add_subdirectory("${J2_LIBRARY_ROOT_PATH}" "${CMAKE_CURRENT_BINARY_DIR}/j2_library_build")

# }} 
########################################################

########################################################
# {{
if(WIN32)
    # Windows 설정
    add_definitions(-D_WIN32_WINNT=0x0A00) # Win10=0x0A00, Win11=0x0A00
    set(PLATFORM_LIBS ws2_32)
elseif(UNIX)
    # Unix 계열 설정
    if(CMAKE_SYSTEM_NAME STREQUAL "Android")
        # Android
        set(PLATFORM_LIBS pthread)
    elseif(APPLE)
        # macOS, iOS
        set(PLATFORM_LIBS pthread)
    elseif(CMAKE_SYSTEM_NAME STREQUAL "FreeBSD" OR
           CMAKE_SYSTEM_NAME STREQUAL "NetBSD" OR
           CMAKE_SYSTEM_NAME STREQUAL "OpenBSD" OR
           CMAKE_SYSTEM_NAME STREQUAL "DragonFly")
        # BSD 계열
        set(PLATFORM_LIBS pthread)
    elseif(CMAKE_SYSTEM_NAME STREQUAL "AIX")
        # AIX에서는 `pthread`가 일부 환경에서 제한적일 수 있음
        set(PLATFORM_LIBS pthread)
    elseif(CMAKE_SYSTEM_NAME STREQUAL "HP-UX")
        # HP-UX는 POSIX 스레드 지원 여부 확인 필요
        set(PLATFORM_LIBS pthread)
    elseif(CMAKE_SYSTEM_NAME STREQUAL "QNX")
        # QNX
        set(PLATFORM_LIBS pthread)
    elseif(CMAKE_SYSTEM_NAME STREQUAL "Solaris")
        # Solaris
        set(PLATFORM_LIBS pthread)
    else()
        # 일반적인 Unix-like 시스템
        set(PLATFORM_LIBS pthread)
    endif()
else()
    message(WARNING "Unsupported platform: ${CMAKE_SYSTEM_NAME}")
    set(PLATFORM_LIBS "")
endif()

# }}
########################################################

file(GLOB SRC_FILES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
add_executable(${EXE_NAME} ${SRC_FILES}) 

########################################################
# {{

target_link_libraries(${EXE_NAME} PRIVATE j2_library) # j2_library를 링크

target_include_directories(${EXE_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}") # 인클루드 경로
# target_link_directories(${EXE_NAME} PRIVATE ...) # 링크 디렉토리 (필요 시)

# }}
########################################################

########################################################
# {{

target_link_libraries(${EXE_NAME} PRIVATE ${PLATFORM_LIBS})
 
# Windows 에서만 WinSock 링크 설정
if (WIN32)
  # 필요한 경우: 사용하려는 최소 Windows 버전(예: 0x0601=Win7, 0x0A00=Win10)
  target_compile_definitions(${EXE_NAME} PRIVATE
    WIN32_LEAN_AND_MEAN
    _WIN32_WINNT=0x0A00
  )

  # MSVC / MinGW 공통: WinSock2 링크
  target_link_libraries(${EXE_NAME} PRIVATE ws2_32)
endif()

# }}
########################################################

