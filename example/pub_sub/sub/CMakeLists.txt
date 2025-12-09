cmake_minimum_required(VERSION 3.26)

project(j2_subscriber_project LANGUAGES CXX)

set(EXE_NAME "j2_subscriber")

# C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# j2_library 
set(J2_LIBRARY_ROOT_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../../j2_library")
add_subdirectory("${J2_LIBRARY_ROOT_PATH}" "${CMAKE_CURRENT_BINARY_DIR}/j2_library_build")
list(PREPEND CMAKE_MODULE_PATH "${J2_LIBRARY_ROOT_PATH}/cmake")
include(j2_cmake)
include(j2_network_platform_pre_module)

# Sources and target
file(GLOB SRC_FILES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
add_executable(${EXE_NAME} ${SRC_FILES})

# Link and include
target_link_libraries(${EXE_NAME} PRIVATE j2_library)
target_include_directories(${EXE_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}")

# Platform-specific network linking
include(j2_network_platform_post_module)
j2_target_network_platform_link(${EXE_NAME})




