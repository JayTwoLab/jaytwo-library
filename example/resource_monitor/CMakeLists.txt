cmake_minimum_required(VERSION 3.10)
project(j2_resource_monitor_example)
set(EXE_NAME j2_resource_monitor_example)

# C++17 표준 설정
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 실행 파일 생성
add_executable(${EXE_NAME} main.cpp)

########################################################
# j2_library 라이브러리 관련하여 추가할 부분  
# {{

# j2_library 루트 경로 설정 (전역 변수)
set(J2_LIBRARY_ROOT_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../j2_library" CACHE PATH "Path to j2_library root")

# j2_library 라이브러리 루트 디렉토리 설정
#  - j2_library 라이브러리를 현재 프로젝트의 아웃풋과 같은 소스 트리에서 빌드하도록 설정
#  - 별도의 빌드 디렉토리를 사용하여 충돌 방지
#  - j2_library 라이브러리가 이미 특정 경로에 설치된 경우에는 해당 경로를 지정하여 사용할 수도 있음
add_subdirectory("${J2_LIBRARY_ROOT_PATH}" "${CMAKE_CURRENT_BINARY_DIR}/j2_library_build")

# j2_library CMake 모듈 포함
list(PREPEND CMAKE_MODULE_PATH "${J2_LIBRARY_ROOT_PATH}/cmake")
include(j2_cmake) # j2_cmake.cmake 모듈 포함 (필요한 모듈만 선택적으로 포함도 가능)

# }} 
########################################################

# OS별 필수 라이브러리 설정
if(WIN32)
    # Windows: 메모리 및 프로세스 정보 추출을 위한 psapi 링크
    target_link_libraries(${EXE_NAME} PRIVATE psapi)
else()
    # Linux: 기본 시스템 라이브러리 사용 (별도 추가 링크 불필요)
endif()
 
########################################################
# j2_library 라이브러리 관련하여 추가할 부분 
# {{

target_link_libraries(${EXE_NAME} PRIVATE j2_library) # j2_library를 링크

target_include_directories(${EXE_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}") # 인클루드 경로
# target_link_directories(${EXE_NAME} PRIVATE ...) # 링크 디렉토리 (필요 시)

# }} 
########################################################
