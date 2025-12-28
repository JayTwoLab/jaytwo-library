# j2_link_lib.cmake
# 사용법: j2_link_lib(<target> [<target2> ...])
# 지정한 하나 이상의 CMake 타겟에 대해 라이브러리 링크와 include 디렉토리를 적용합니다.

function(j2_link_lib)
    if (ARGC EQUAL 0)
        message(FATAL_ERROR "j2_link_lib: 하나 이상의 대상 이름을 인수로 전달해야 합니다.")
    endif()

    foreach(_target IN LISTS ARGV)
        if (NOT TARGET ${_target})
            message(FATAL_ERROR "j2_link_lib: 대상 '${_target}'을(를) 찾을 수 없습니다.")
        endif()

        # 스레드 라이브러리
        target_link_libraries(${_target} PRIVATE Threads::Threads)

        # nlohmann_json 라이브러리 (header-only)
        target_link_libraries(${_target} PUBLIC nlohmann_json::nlohmann_json)

        # spdlog 라이브러리 (header-only)
        target_link_libraries(${_target} PUBLIC spdlog::spdlog)

        # Asio 라이브러리 (Boost.Asio 또는 standalone Asio)
        # target_link_libraries(${_target} PUBLIC j2_asio)

        # SimpleIni 라이브러리
        # if (FALSE)  # simpleini 임시 비활성화
        #     target_link_libraries(${_target} PUBLIC simpleini::simpleini)
        # endif()

        # curl 라이브러리 링크
        if (TARGET CURL::libcurl)
            # 최신 CMake / FindCURL: import target 사용
            target_link_libraries(${_target} PUBLIC CURL::libcurl)
        else()
            # 구 CMake 대비: 변수 방식 폴백
            if (CURL_INCLUDE_DIRS)
                target_include_directories(${_target} PUBLIC ${CURL_INCLUDE_DIRS})
            endif()
            if (CURL_LIBRARIES)
                target_link_libraries(${_target} PUBLIC ${CURL_LIBRARIES})
            endif()
        endif()

        # googletest 라이브러리 링크 추가
        # if (GTest_FOUND)
        #   target_link_libraries(${_target} PUBLIC GTest::gtest GTest::gtest_main)
        # elseif (TARGET gtest)
        #   target_link_libraries(${_target} PUBLIC gtest gtest_main)
        # endif()

        # immer 라이브러리 링크 
        # if(FALSE)  # immer 임시 비활성화
        #     target_link_libraries(${_target} PUBLIC j2_immer) 
        # endif()

        # tinyxml2 링크 (일관된 이름 사용: tinyxml2::tinyxml2)
        # target_link_libraries(${_target} PUBLIC tinyxml2::tinyxml2)

    endforeach()
endfunction()

