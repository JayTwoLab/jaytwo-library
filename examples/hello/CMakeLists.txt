# examples/hello/CMakeLists.txt

cmake_minimum_required(VERSION 3.26)

project(j2_hello
  VERSION 1.0.0
  LANGUAGES CXX
)

if (MSVC)
  add_compile_options(/utf-8) # MSVC 컴파일러에서 UTF-8 인코딩 사용
endif()

# [최상위 프로젝트 사용시 쉽게 사용하는 방법] j2_cmake 모듈 직접 포함
#  CMAKE_CURRENT_SOURCE_DIR → 현재 처리 중인 CMakeLists.txt 파일이 위치한 디렉토리
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../j2_library/cmake") 
include(j2_cmake OPTIONAL RESULT_VARIABLE _J2_CMAKE_FOUND)
if(NOT _J2_CMAKE_FOUND)
  message(FATAL_ERROR "Mandatory module j2_cmake.cmake not found, please check CMAKE_MODULE_PATH settings.")
endif()

add_executable(${PROJECT_NAME} main.cpp) # 실행 프로그램 설정
target_include_directories(${PROJECT_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}" ) # 인클루드 경로 설정
target_link_libraries(${PROJECT_NAME} PRIVATE j2_library) # j2_library 라이브러리 링크


 
