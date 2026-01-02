cmake_minimum_required(VERSION 3.26)

set(PROJECT_NAME "use_j2_installed_project") # project name
project(${PROJECT_NAME} LANGUAGES CXX)

set(EXE_NAME "use_j2_installed") # Executable name

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

#
# j2_library is installed by referring to install-lib.cmd.
# 
# Example: Building and Installation j2_library in Windows 
#  cmake -S . -B build -G "Ninja" -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX="C:/opt/j2_library"
#  cmake --build build
#  cmake --install build
#
# Dynamic library build example:
# cmake -S . -B build -G "Ninja" -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX="C:/opt/j2_library" -DBUILD_SHARED_LIBS=ON
# 

# Windows 11
set(J2_LIBRARY_ROOT_PATH "C:/opt/j2_library")
set(J2_LIBRARY_ROOT_LIB_PATH "${J2_LIBRARY_ROOT_PATH}/lib")
set(J2_THIRD_PARTY_ROOT_PATH "${J2_LIBRARY_ROOT_PATH}/share/j2_library/third-party")

# Linux x64 (Rocky8, etc)
# set(J2_LIBRARY_ROOT_PATH "/home/jaytwo/workspace/j2")
# set(J2_LIBRARY_ROOT_LIB_PATH "${J2_LIBRARY_ROOT_PATH}/lib64")


list(PREPEND CMAKE_MODULE_PATH "${J2_LIBRARY_ROOT_LIB_PATH}/cmake/j2_library") # Add j2_library cmake module path
include(j2_cmake) # Load j2_cmake module
include(j2_find_package) # Load j2_find_package module 
include(j2_link_lib) # Load j2_link_lib module

add_executable(${EXE_NAME} src/main.cpp) # Add executable target

target_include_directories(${EXE_NAME} PRIVATE "${J2_LIBRARY_ROOT_PATH}/include") # Include directory for j2_library
target_link_directories(${EXE_NAME} PRIVATE "${J2_LIBRARY_ROOT_LIB_PATH}") # Link directory for j2_library
j2_link_lib(${EXE_NAME}) # Link library using j2_link_lib function
target_link_libraries(${EXE_NAME} PRIVATE j2_library) # Link j2_library

# Notice: If the program requires dynamic libraries (.dll) at runtime,
# instruct users to add the bin directory to their PATH.
if (WIN32)
  message(STATUS "If run fails due to missing DLLs, "
  "add ${J2_LIBRARY_ROOT_PATH}/bin to PATH or copy DLLs next to the exe.")
endif()


