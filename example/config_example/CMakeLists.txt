cmake_minimum_required(VERSION 3.10)
project(j2_config_example)
set(EXE_NAME j2_config_example)

# C++17 표준 설정 (std::string_view 및 filesystem 등을 위해 권장)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 소스 파일 목록 정의
set(SOURCE_FILES  main.cpp)  

# 실행 파일 생성
add_executable(${EXE_NAME} ${SOURCE_FILES} ${HEADER_FILES})

# j2_library 타겟을 참조 (프로젝트 구조에 따라 다를 수 있음)
# 보통 상위 프로젝트에서 add_subdirectory로 포함했다면 아래와 같이 연결합니다.
# target_link_libraries(${EXE_NAME} PRIVATE j2_library)
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

# OS별 추가 설정 (필요 시)
if(WIN32)
    # Windows 전용 컴파일 옵션이 필요하다면 여기에 작성
else()
    # Linux/macOS 전용 컴파일 옵션이 필요하다면 여기에 작성
    # 예: target_link_libraries(j2_config_example PRIVATE pthread)
endif()

########################################################
# j2_library 라이브러리 관련하여 추가할 부분 
# {{

target_link_libraries(${EXE_NAME} PRIVATE j2_library) # j2_library를 링크

target_include_directories(${EXE_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}") # 인클루드 경로
# target_link_directories(${EXE_NAME} PRIVATE ...) # 링크 디렉토리 (필요 시)

# }} 
########################################################


# 빌드 폴더로 설정 파일을 자동 복사하는 명령
configure_file(
    "${CMAKE_CURRENT_SOURCE_DIR}/app_config.conf"
    "${CMAKE_CURRENT_BINARY_DIR}/app_config.conf"
    COPYONLY
)

