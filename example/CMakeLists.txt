cmake_minimum_required(VERSION 3.26)

set(J2_LIB_EXAMPLE_VERSION "0.1.0")
project(j2_lib_example VERSION ${J2_LIB_EXAMPLE_VERSION} LANGUAGES CXX)

add_subdirectory(concurrent_queue) # 동시성 큐 예제

add_subdirectory(file-finder) # 파일 파인더 예제

add_subdirectory(hello-world) # 기본 템플릿 예제

add_subdirectory(log-manager) # 로그 매니저 예제

add_subdirectory(network) # 네트워크 예제

add_subdirectory(pub_sub) # 퍼블리시-서브스크라이브 예제








