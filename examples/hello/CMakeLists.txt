# examples/hello/CMakeLists.txt
cmake_minimum_required(VERSION 3.26)

project(j2_hello VERSION 0.1.0 LANGUAGES CXX) # 프로젝트 선언

# j2_library/cmake 폴더를 모듈 검색 경로에 추가
list(PREPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/../../j2_library/cmake")
include(j2_cmake) # j2_cmake 모듈 포함 (필요한 모듈만 선택적으로 포함도 가능)

# j2_library 라이브러리를 같은 소스 트리에서 빌드
add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/../../j2_library" "${CMAKE_CURRENT_BINARY_DIR}/j2_library_build")

add_executable(j2_hello main.cpp) # 실행 프로그램 설정

target_compile_features(j2_hello PRIVATE cxx_std_17) # C++17
target_include_directories(j2_hello PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}" ) # 인클루드 경로 설정
target_link_libraries(j2_hello PRIVATE j2_library) # j2_library 라이브러리 링크



 
