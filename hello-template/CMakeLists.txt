# hello-template/CMakeLists.txt
cmake_minimum_required(VERSION 3.26)
project(j2_hello_template LANGUAGES CXX)

# j2_library 라이브러리를 같은 소스 트리에서 빌드
add_subdirectory(${CMAKE_SOURCE_DIR}/../j2_library ${CMAKE_BINARY_DIR}/j2_library_build)
##
## NOTE: 빌드 동작 방식
##
## (1) 처음 빌드 시
##  `j2_library`가 한 번 전체 컴파일되고, 이후 `hello-template` 실행 파일이 링크됩니다.
##
## (2) `j2_library`의 소스 코드가 변경되지 않은 경우
##  → `j2_library`는 재컴파일되지 않습니다.
##  빌드 시스템(Ninja/VS/MSBuild)이 타임스탬프와 의존성을 체크해서 "변경 없음"이면 건너뜁니다.
##  따라서 `hello-template`만 다시 빌드/링크됩니다.
##
## (3) `j2_library` 소스가 수정된 경우
##  → 변경된 소스 파일만 재컴파일하고, 필요 시 라이브러리를 다시 만들며, 그 후 `hello-template`도 다시 링크됩니다.
##
## (4) 헤더 파일 수정 시
##  → 그 헤더를 사용하는 소스들이 다시 빌드됩니다.
## 예를 들어 `math.hpp`를 고치면 `math.cpp`와 `main.cpp` 둘 다 재컴파일될 수 있습니다.
##

file(GLOB SRC_FILES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp" "${CMAKE_CURRENT_SOURCE_DIR}/*.c")
add_executable(j2_hello_template ${SRC_FILES}) # j2_hello_template 는 ${PROJECT_NAME}을 사용하지 말고 직접 명시할 것

target_compile_features(j2_hello_template PRIVATE cxx_std_17) # C++17 필요
target_link_libraries(j2_hello_template PRIVATE j2_library) # j2_library를 링크



