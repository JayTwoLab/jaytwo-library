cmake_minimum_required(VERSION 3.26)

project(j2_log_manager_example LANGUAGES CXX)

set(EXE_NAME "j2_log_manager")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# j2_library CMake helper 모듈 경로
list(PREPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/j2_library/cmake")

# 소스 파일 및 실행 파일
file(GLOB SRC_FILES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
add_executable(${EXE_NAME} ${SRC_FILES})

# 루트에서 제공되는 ALIAS 타깃 사용
target_link_libraries(${EXE_NAME} PRIVATE j2_library::j2_library)

target_include_directories(${EXE_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")

# 현재 CMakeLists.txt 경로 매크로 (테스트용)
target_compile_definitions(${EXE_NAME} PRIVATE CMAKE_SOURCE_DIR_PATH="${CMAKE_CURRENT_LIST_DIR}")

