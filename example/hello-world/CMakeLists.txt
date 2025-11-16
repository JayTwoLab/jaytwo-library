# hello-template/CMakeLists.txt
cmake_minimum_required(VERSION 3.26)

project(j2_hello_world_example LANGUAGES CXX) # 프로젝트 선언

set(EXE_NAME "j2_hello_world") # 실행 파일 이름 설정

set(CMAKE_CXX_STANDARD 17) # C++17
set(CMAKE_CXX_STANDARD_REQUIRED ON) # 지정한 표준을 반드시 사용
set(CMAKE_CXX_EXTENSIONS OFF) # OFF: 컴파일러 확장 기능을 쓰지 않고 순수한 표준 모드만 사용

########################################################
# j2_library 라이브러리 관련하여 추가할 부분 
# {{

# j2_library 루트 경로 설정 (전역 변수)
# 루트 경로는 상대 경로 또는 절대 경로로 지정 가능
set(J2_LIBRARY_ROOT_PATH "${CMAKE_CURRENT_SOURCE_DIR}/../../j2_library" CACHE PATH "Path to j2_library root")

# j2_library CMake 모듈 포함
list(PREPEND CMAKE_MODULE_PATH "${J2_LIBRARY_ROOT_PATH}/cmake")
include(j2_cmake) # 디폴트 모듈(j2_cmake.cmake) 포함 (필요한 모듈만 선택적으로 포함도 가능)

# j2_library 라이브러리를 같은 소스 트리에서 빌드
add_subdirectory("${J2_LIBRARY_ROOT_PATH}" "${CMAKE_CURRENT_BINARY_DIR}/j2_library_build")

##
## NOTE: 빌드 동작 방식
##
## (1) 처음 빌드 시
##  `j2_library`가 한 번 전체 컴파일되고, 이후 `hello-template` 실행 파일이 링크됩니다.
##
## (2) `j2_library`의 소스 코드가 변경되지 않은 경우
##    → `j2_library`는 재컴파일되지 않습니다.
##  빌드 시스템(Ninja/VS/MSBuild)이 타임스탬프와 의존성을 체크해서 "변경 없음"이면 건너뜁니다.
##  따라서 `hello-template`만 다시 빌드/링크됩니다.
##
## (3) `j2_library` 소스가 수정된 경우
##    → 변경된 소스 파일만 재컴파일하고, 필요 시 라이브러리를 다시 만들며, 그 후 `hello-template`도 다시 링크됩니다.
##
## (4) 헤더 파일 수정 시
##    → 그 헤더를 사용하는 소스들이 다시 빌드됩니다.
## 예를 들어 `math.hpp`를 고치면 `math.cpp`와 `main.cpp` 둘 다 재컴파일될 수 있습니다.
##

# }} 
########################################################

# 소스 파일(*.cpp) 설정 및 실행 파일 생성 
file(GLOB SRC_FILES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp")
add_executable(${EXE_NAME} ${SRC_FILES}) 

########################################################
# j2_library 라이브러리 관련하여 추가할 부분 
# {{

target_link_libraries(${EXE_NAME} PRIVATE j2_library) # j2_library를 링크

target_include_directories(${EXE_NAME} PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}") # 인클루드 경로
# target_link_directories(${EXE_NAME} PRIVATE ...) # 링크 디렉토리 (필요 시)

# }} 
########################################################


