cmake_minimum_required(VERSION 3.26)

project(j2_rest_post_client_project LANGUAGES CXX)

set(EXE_NAME "j2_rest_post_client")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# j2_library CMake helper 모듈 경로
list(PREPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/j2_library/cmake")

# 플랫폼별 네트워크 사전 모듈
include(j2_network_platform_pre_module)

# 소스 파일 및 실행 파일
file(GLOB SRC_FILES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
add_executable(${EXE_NAME} ${SRC_FILES})

# j2_library 타깃 링크
target_link_libraries(${EXE_NAME} PRIVATE j2_library::j2_library)
target_include_directories(${EXE_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")

# 플랫폼별 네트워크 후처리 모듈
include(j2_network_platform_post_module)
j2_target_network_platform_link(${EXE_NAME})

