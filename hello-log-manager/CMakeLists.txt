# hello-log-manager/CMakeLists.txt
cmake_minimum_required(VERSION 3.26)

project(j2_log_manager_template LANGUAGES CXX) # 프로젝트 선언

set(CMAKE_CXX_STANDARD 17) # C++17
set(CMAKE_CXX_STANDARD_REQUIRED ON) # 지정한 표준을 반드시 사용
set(CMAKE_CXX_EXTENSIONS OFF) # OFF: 컴파일러 확장 기능을 쓰지 않고 순수한 표준 모드만 사용

# j2_library 루트 경로 설정 (전역 변수)
set(J2_LIBRARY_ROOT_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../j2_library" CACHE PATH "Path to j2_library root")

list(PREPEND CMAKE_MODULE_PATH "${J2_LIBRARY_ROOT_PATH}/cmake")
include(j2_cmake) # j2_cmake.cmake 모듈 포함 (필요한 모듈만 선택적으로 포함도 가능)

# j2_library 라이브러리를 같은 소스 트리에서 빌드
add_subdirectory("${J2_LIBRARY_ROOT_PATH}" "${CMAKE_CURRENT_BINARY_DIR}/j2_library_build")

file(GLOB SRC_FILES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/*.c")
add_executable(j2_log_manager_template ${SRC_FILES}) 

target_link_libraries(j2_log_manager_template PRIVATE j2_library) # j2_library를 링크

target_include_directories(j2_log_manager_template PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}") # 인클루드 경로
# target_link_directories(j2_log_manager_template PRIVATE ...) # 링크 디렉토리 (필요 시)


