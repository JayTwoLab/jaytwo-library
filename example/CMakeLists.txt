cmake_minimum_required(VERSION 3.26)

set(J2_LIB_EXAMPLE_VERSION "0.1.0")
project(j2_lib_example VERSION ${J2_LIB_EXAMPLE_VERSION} LANGUAGES CXX)

add_subdirectory(hello-template) # 기본 템플릿 예제
add_subdirectory(hello-log-manager) # 로그 매니저 예제
add_subdirectory(hello-file-finder) # 파일 파인더 예제
add_subdirectory(network)






