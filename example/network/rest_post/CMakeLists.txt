cmake_minimum_required(VERSION 3.26)

project(j2_rest_post_client_project LANGUAGES CXX) # 프로젝트 선언

set(EXE_NAME "j2_rest_post_client") # 실행 파일 이름 설정

########################################################

set(CMAKE_CXX_STANDARD 17) # C++17
set(CMAKE_CXX_STANDARD_REQUIRED ON) # 지정한 표준을 반드시 사용
set(CMAKE_CXX_EXTENSIONS OFF) # OFF: 컴파일러 확장 기능을 쓰지 않고 순수한 표준 모드만 사용

########################################################
# {{

# j2_library 루트 경로 설정 (전역 변수)
set(J2_LIBRARY_ROOT_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../../j2_library" CACHE PATH "Path to j2_library root")

# j2_library 라이브러리 루트 디렉토리 설정
#  - j2_library 라이브러리를 현재 프로젝트의 아웃풋과 같은 소스 트리에서 빌드하도록 설정
#  - 별도의 빌드 디렉토리를 사용하여 충돌 방지
#  - j2_library 라이브러리가 이미 특정 경로에 설치된 경우에는 해당 경로를 지정하여 사용할 수도 있음
add_subdirectory("${J2_LIBRARY_ROOT_PATH}" "${CMAKE_CURRENT_BINARY_DIR}/j2_library_build")

list(PREPEND CMAKE_MODULE_PATH "${J2_LIBRARY_ROOT_PATH}/cmake")
include(j2_cmake) # j2_cmake.cmake 모듈 포함 (필요한 모듈만 선택적으로 포함도 가능)

include(j2_network_platform_pre_module) # 플랫폼별 네트워크 라이브러리 설정 (사전 설정)

# }} 
########################################################

# 소스 파일 설정
file(GLOB SRC_FILES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
add_executable(${EXE_NAME} ${SRC_FILES}) 

########################################################
# {{

# j2_library 링크 및 인클루드 설정

target_link_libraries(${EXE_NAME} PRIVATE j2_library) # j2_library를 링크

target_include_directories(${EXE_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}") # 인클루드 경로
# target_link_directories(${EXE_NAME} PRIVATE ...) # 링크 디렉토리 (필요 시)

# }}
########################################################

########################################################
# {{

# 플랫폼별 네트워크 라이브러리 설정 (사후 설정)
include(j2_network_platform_post_module)
j2_target_network_platform_link(${EXE_NAME})

# }}
########################################################

