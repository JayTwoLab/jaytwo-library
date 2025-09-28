# hello/CMakeLists.txt
cmake_minimum_required(VERSION 3.26)

project(j2_hello
  VERSION 0.1.0
  LANGUAGES CXX
)

###############################################
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../j2_library/cmake")
#  CMAKE_CURRENT_SOURCE_DIR → 현재 처리 중인 CMakeLists.txt 파일이 위치한 디렉토리

# j2_cmake.cmake
include(j2_cmake OPTIONAL RESULT_VARIABLE _J2_CMAKE_FOUND)
if(NOT _J2_CMAKE_FOUND)
 message(FATAL_ERROR "[j2_hello] 필수 모듈 j2_cmake.cmake 를 찾을 수 없습니다. CMAKE_MODULE_PATH 설정을 확인하세요.")
endif()
#################################################

add_executable(j2_hello
  main.cpp
)

target_link_libraries(j2_hello PRIVATE
  j2_library::j2_library # j2_library 라이브러리 링크
)

 
