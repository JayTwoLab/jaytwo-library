# hello/CMakeLists.txt
cmake_minimum_required(VERSION 3.26)

project(j2_hello
  VERSION 0.1.0
  LANGUAGES CXX
)

###############################################
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../j2_library/cmake")
#  CMAKE_CURRENT_SOURCE_DIR → 현재 처리 중인 CMakeLists.txt 파일이 위치한 디렉토리

include(j2_cmake OPTIONAL) # j2_cmake.cmake
#################################################

add_executable(j2_hello
  main.cpp
)

target_link_libraries(j2_hello PRIVATE
  j2_library::j2_library # j2_library 라이브러리 링크
)

 
