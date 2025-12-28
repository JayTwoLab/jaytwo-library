#########################################
# include_guard(GLOBAL) # 전역 범위(Global scope) 에서 중복 include를 막습니다.
include_guard(DIRECTORY)

#
# j2_network_platform_post_module.cmake
# 플랫폼별 네트워크 라이브러리 설정 (사후(post-) 설정)
# 

# 플랫폼별 네트워크 라이브러리 링크 함수
function(j2_target_network_platform_link target)

    ###############################################
    # socket (tcp/udp)

    target_link_libraries(${target} PRIVATE ${NETWORK_PLATFORM_LIBS})

    if (WIN32)
        target_compile_definitions(${target} PRIVATE
            WIN32_LEAN_AND_MEAN
            _WIN32_WINNT=0x0A00
        )
        target_link_libraries(${target} PRIVATE ws2_32)
    endif()

    ###############################################
    # curl 

    target_include_directories(${target} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

    # libcurl 링크
    # - 새로운 CMake/FindCURL: CURL::libcurl 타겟 제공
    # - 예전 스타일: CURL_INCLUDE_DIRS / CURL_LIBRARIES 제공
    if (TARGET CURL::libcurl)
        target_link_libraries(${target} PRIVATE CURL::libcurl)
    else()
        target_include_directories(${target} PRIVATE ${CURL_INCLUDE_DIRS})
        target_link_libraries(${target} PRIVATE ${CURL_LIBRARIES})
    endif()

    # Windows 에서 static triplet( x64-windows-static ) 을 쓰는 경우
    # libcurl 정적 링크용 매크로 정의
    if (WIN32)
        target_compile_definitions(${target} PRIVATE CURL_STATICLIB)
    endif()

endfunction()
