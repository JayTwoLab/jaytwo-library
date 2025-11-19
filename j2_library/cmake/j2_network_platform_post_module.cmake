#
# j2_network_platform_post_module.cmake
# 플랫폼별 네트워크 라이브러리 설정 (사후(post-) 설정)
# 

# 플랫폼별 네트워크 라이브러리 링크 함수
function(j2_target_network_platform_link target)
    target_link_libraries(${target} PRIVATE ${NETWORK_PLATFORM_LIBS})

    if (WIN32)
        target_compile_definitions(${target} PRIVATE
            WIN32_LEAN_AND_MEAN
            _WIN32_WINNT=0x0A00
        )
        target_link_libraries(${target} PRIVATE ws2_32)
    endif()
endfunction()
