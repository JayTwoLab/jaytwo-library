cmake_minimum_required(VERSION 3.10)
project(j2_crash_handler_example)
set(EXE_NAME "j2_crash_handler_example") # 실행 파일 이름 설정

# C++ 17 표준 설정
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 예제 실행 파일 생성
add_executable(${EXE_NAME} main.cpp)

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

# Windows 환경에서 콜 스택 추출을 위해 필수적인 시스템 라이브러리 링크
if(WIN32)
    # Windows API에서 콜 스택 추출을 위해 dbghelp 라이브러리를 링크
    target_link_libraries(${EXE_NAME} PRIVATE dbghelp)
    
    # MSVC에서 심볼 정보를 생성하도록 설정 (Release 모드에서도 콜 스택을 보려면 필요)
    target_compile_options(${EXE_NAME} PRIVATE /Zi)
    set_target_properties(${EXE_NAME} PROPERTIES LINK_FLAGS "/DEBUG")
endif()

# UNIX 계열 환경(Linux, Mac 등)에서는 디버깅 심볼과 최적화 설정을 조정하여 콜 스택 추출이 원활하도록 설정
if(UNIX)
    # gcc를 사용한다고 가정하고 디버깅 심볼과 최적화 설정
    target_compile_options(${EXE_NAME} PRIVATE -g -O0) # 최적화 끄기(-O0) 권장
    target_link_options(${EXE_NAME} PRIVATE -rdynamic) # 런타임에 심볼 정보를 포함하여 콜 스택 추출이 가능하도록 설정
endif()

# 인클루드 경로 설정
target_include_directories(${EXE_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/../../include)

########################################################
# j2_library 라이브러리 관련하여 추가할 부분 
# {{

target_link_libraries(${EXE_NAME} PRIVATE j2_library) # j2_library를 링크

target_include_directories(${EXE_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}") # 인클루드 경로
# target_link_directories(${EXE_NAME} PRIVATE ...) # 링크 디렉토리 (필요 시)

# }} 
########################################################
