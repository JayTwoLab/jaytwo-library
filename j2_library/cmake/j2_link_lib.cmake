
# 스레드 라이브러리
target_link_libraries(j2_library PRIVATE Threads::Threads)  

# nlohmann_json 라이브러리 (header-only library)
target_link_libraries(j2_library PUBLIC nlohmann_json::nlohmann_json)  

# spdlog 라이브러리 (header-only library)
target_link_libraries(j2_library PUBLIC spdlog::spdlog)

# Asio 라이브러리 (Boost.Asio 또는 standalone Asio)
# target_link_libraries(j2_library PUBLIC j2_asio)

# SimpleIni 라이브러리
# if (FALSE)  # simpleini 임시 비활성화
#  target_link_libraries(j2_library PUBLIC simpleini::simpleini)
# endif()

# curl 라이브러리 링크
if (TARGET CURL::libcurl)
    # 최신 CMake / FindCURL: import target 사용
    target_link_libraries(j2_library PUBLIC CURL::libcurl)
else()
    # 구 CMake 대비: 변수 방식 폴백
    if (CURL_INCLUDE_DIRS)
        target_include_directories(j2_library PUBLIC ${CURL_INCLUDE_DIRS})
    endif()
    if (CURL_LIBRARIES)
        target_link_libraries(j2_library PUBLIC ${CURL_LIBRARIES})
    endif()
endif()

# googletest 라이브러리 링크 추가
# if (GTest_FOUND)
#   target_link_libraries(j2_library PUBLIC GTest::gtest GTest::gtest_main)
# elseif (TARGET gtest)
#   target_link_libraries(j2_library PUBLIC gtest gtest_main)
# endif()

# immer 라이브러리 링크 
# if(FALSE)  # immer 임시 비활성화
#     target_link_libraries(j2_library PUBLIC j2_immer) 
# endif()

# tinyxml2 링크 (일관된 이름 사용: tinyxml2::tinyxml2)
target_link_libraries(j2_library PUBLIC tinyxml2::tinyxml2)

