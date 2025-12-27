cmake_minimum_required(VERSION 3.26)

set(PROJECT_NAME "use_j2_installed_project") # project name
project(${PROJECT_NAME} LANGUAGES CXX)

set(EXE_NAME "use_j2_installed") # Executable name

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

#
# j2_library is installed by referring to install-lib.cmd. 
# Windows example:
#  cmake -S . -B build -G "Ninja" -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX="C:/opt/j2_library"
#  cmake --build build
#  cmake --install build
# 
set(J2_LIBRARY_ROOT_PATH "C:/opt/j2_library") # set j2_library root path
list(PREPEND CMAKE_MODULE_PATH "${J2_LIBRARY_ROOT_PATH}/lib/cmake/j2_library")
include(j2_cmake) # Load j2_cmake module
include(j2_find_package) # Load j2_find_package module 
include(j2_link_lib) # Load j2_link_lib module

add_executable(${EXE_NAME} src/main.cpp) # Add executable target

target_include_directories(${EXE_NAME} PRIVATE "${J2_LIBRARY_ROOT_PATH}/include")
target_link_directories(${EXE_NAME} PRIVATE "${J2_LIBRARY_ROOT_PATH}/lib")
j2_link_lib(${EXE_NAME}) # Link j2_library to the executable
target_link_libraries(${EXE_NAME} PRIVATE j2_library)

# "Notice: If the program requires dynamic libraries (.dll) at runtime, instruct users to add the bin directory to their PATH."
if (WIN32)
  message(STATUS "If run fails due to missing DLLs, add C:/opt/j2_library/bin to PATH or copy DLLs next to the exe.")
endif()


