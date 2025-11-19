#
# j2_network_platform_pre_module.cmake
# 플랫폼별 네트워크 라이브러리 설정 (사전(pre-) 설정)
# 

if(WIN32)
    # Windows 설정
    add_definitions(-D_WIN32_WINNT=0x0A00) # Win10=0x0A00, Win11=0x0A00
    set(NETWORK_PLATFORM_LIBS ws2_32)
elseif(UNIX)
    # Unix 계열 설정
    if(CMAKE_SYSTEM_NAME STREQUAL "Android")
        # Android
        set(NETWORK_PLATFORM_LIBS pthread)
    elseif(APPLE)
        # macOS, iOS
        set(NETWORK_PLATFORM_LIBS pthread)
    elseif(CMAKE_SYSTEM_NAME STREQUAL "FreeBSD" OR
           CMAKE_SYSTEM_NAME STREQUAL "NetBSD" OR
           CMAKE_SYSTEM_NAME STREQUAL "OpenBSD" OR
           CMAKE_SYSTEM_NAME STREQUAL "DragonFly")
        # BSD 계열
        set(NETWORK_PLATFORM_LIBS pthread)
    elseif(CMAKE_SYSTEM_NAME STREQUAL "AIX")
        # AIX에서는 `pthread`가 일부 환경에서 제한적일 수 있음
        set(NETWORK_PLATFORM_LIBS pthread)
    elseif(CMAKE_SYSTEM_NAME STREQUAL "HP-UX")
        # HP-UX는 POSIX 스레드 지원 여부 확인 필요
        set(NETWORK_PLATFORM_LIBS pthread)
    elseif(CMAKE_SYSTEM_NAME STREQUAL "QNX")
        # QNX
        set(NETWORK_PLATFORM_LIBS pthread)
    elseif(CMAKE_SYSTEM_NAME STREQUAL "Solaris")
        # Solaris
        set(NETWORK_PLATFORM_LIBS pthread)
    else()
        # 일반적인 Unix-like 시스템
        set(NETWORK_PLATFORM_LIBS pthread)
    endif()
else()
    message(WARNING "Unsupported platform: ${CMAKE_SYSTEM_NAME}")
    set(NETWORK_PLATFORM_LIBS "")
endif()
