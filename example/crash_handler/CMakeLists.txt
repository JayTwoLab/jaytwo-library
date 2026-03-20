cmake_minimum_required(VERSION 3.10)
project(j2_crash_handler_example)

# C++ 17 표준 설정
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 예제 실행 파일 생성
add_executable(j2_crash_handler_example main.cpp)

# j2_library 타겟을 참조 (프로젝트 구조에 따라 다를 수 있음)
# 보통 상위 프로젝트에서 add_subdirectory로 포함했다면 아래와 같이 연결합니다.
target_link_libraries(j2_crash_handler_example PRIVATE j2_library)

# Windows 환경에서 콜 스택 추출을 위해 필수적인 시스템 라이브러리 링크
if(WIN32)
    target_link_libraries(j2_crash_handler_example PRIVATE dbghelp)
    
    # MSVC에서 심볼 정보를 생성하도록 설정 (Release 모드에서도 콜 스택을 보려면 필요)
    target_compile_options(j2_crash_handler_example PRIVATE /Zi)
    set_target_properties(j2_crash_handler_example PROPERTIES LINK_FLAGS "/DEBUG")
endif()

# 인클루드 경로 설정
target_include_directories(j2_crash_handler_example PRIVATE ${CMAKE_SOURCE_DIR}/../../include)
