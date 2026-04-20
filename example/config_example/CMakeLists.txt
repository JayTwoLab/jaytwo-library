cmake_minimum_required(VERSION 3.10)
project(j2_config_example)
set(EXE_NAME j2_config_example)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# j2_library CMake helper 모듈 경로
list(PREPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/j2_library/cmake")

# 소스 파일 및 실행 파일
set(SOURCE_FILES main.cpp)
add_executable(${EXE_NAME} ${SOURCE_FILES})

# 루트에서 제공되는 ALIAS 타깃 사용
target_link_libraries(${EXE_NAME} PRIVATE j2_library::j2_library)

target_include_directories(${EXE_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")

# 빌드 폴더로 설정 파일 복사
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/app_config.conf"
    "${CMAKE_CURRENT_BINARY_DIR}/app_config.conf"
    COPYONLY
)

