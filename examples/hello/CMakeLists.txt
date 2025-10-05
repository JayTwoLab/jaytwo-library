# examples/hello/CMakeLists.txt

cmake_minimum_required(VERSION 3.26)

project(j2_hello
  VERSION 1.0.0
  LANGUAGES CXX
)

# j2_cmake 모듈 포함
#  CMAKE_CURRENT_SOURCE_DIR → 현재 처리 중인 CMakeLists.txt 파일이 위치한 디렉토리
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../j2_library/cmake") 
include(j2_cmake OPTIONAL RESULT_VARIABLE _J2_CMAKE_FOUND)
if(NOT _J2_CMAKE_FOUND)
 message(FATAL_ERROR "[j2_hello] Mandatory module j2_cmake.cmake not found, please check CMAKE_MODULE_PATH settings.")
endif()

# 실행 프로그램 설정
add_executable(j2_hello
  main.cpp
)

# j2_library 라이브러리 링크
target_link_libraries(j2_hello PRIVATE
  j2_library::j2_library 
)

 
