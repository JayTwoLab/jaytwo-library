# hello-template/CMakeLists.txt
cmake_minimum_required(VERSION 3.26)
project(j2_hello_template LANGUAGES CXX)

# j2_library 를 같은 소스 트리에서 빌드
add_subdirectory(${CMAKE_SOURCE_DIR}/../j2_library ${CMAKE_BINARY_DIR}/j2_library_build)

file(GLOB SRC_FILES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/*.c")
add_executable(j2_hello_template ${SRC_FILES})

target_compile_features(j2_hello_template PRIVATE cxx_std_17)
target_link_libraries(j2_hello_template PRIVATE j2_library)



