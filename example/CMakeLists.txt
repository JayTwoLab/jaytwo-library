cmake_minimum_required(VERSION 3.26)

set(J2_LIB_EXAMPLE_SUPER_VERSION "0.1.0")
project(j2_lib_example VERSION ${J2_LIB_EXAMPLE_SUPER_VERSION} LANGUAGES CXX)

add_subdirectory(hello-log-manager)
add_subdirectory(hello-file-finder)




