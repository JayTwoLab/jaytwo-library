# hello-template/CMakeLists.txt
cmake_minimum_required(VERSION 3.26)

project(j2_hello_world_example LANGUAGES CXX) # 프로젝트 선언

set(EXE_NAME "j2_hello_world") # 실행 파일 이름 설정

set(CMAKE_CXX_STANDARD 17) # C++17
set(CMAKE_CXX_STANDARD_REQUIRED ON) # 지정한 표준을 반드시 사용
set(CMAKE_CXX_EXTENSIONS OFF) # OFF: 컴파일러 확장 기능을 쓰지 않고 순수한 표준 모드만 사용

# j2_library CMake helper 모듈 경로
list(PREPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/j2_library/cmake")

# 소스 파일 및 실행 파일
file(GLOB SRC_FILES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
add_executable(${EXE_NAME} ${SRC_FILES})

# 루트에서 제공되는 ALIAS 타깃 사용
target_link_libraries(${EXE_NAME} PRIVATE j2_library::j2_library)

target_include_directories(${EXE_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")


