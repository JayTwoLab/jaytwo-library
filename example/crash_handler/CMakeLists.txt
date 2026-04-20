cmake_minimum_required(VERSION 3.10)
project(j2_crash_handler_example)
set(EXE_NAME "j2_crash_handler_example")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# j2_library CMake helper 모듈 경로
list(PREPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/j2_library/cmake")

# 예제 실행 파일
add_executable(${EXE_NAME} main.cpp)

# Windows/Unix별 디버깅 설정
if(WIN32)
    target_link_libraries(${EXE_NAME} PRIVATE dbghelp)
    target_compile_options(${EXE_NAME} PRIVATE /Zi)
    set_target_properties(${EXE_NAME} PROPERTIES LINK_FLAGS "/DEBUG")
endif()

if(UNIX)
    target_compile_options(${EXE_NAME} PRIVATE -g -O0)
    target_link_options(${EXE_NAME} PRIVATE -rdynamic)
endif()

target_include_directories(${EXE_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/../../include)

# 루트에서 제공되는 ALIAS 타깃 사용
target_link_libraries(${EXE_NAME} PRIVATE j2_library::j2_library)

target_include_directories(${EXE_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")
