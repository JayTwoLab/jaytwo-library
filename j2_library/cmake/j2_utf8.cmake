#########################################
# 전역 범위(Global scope) 에서 중복 include를 막습니다.
include_guard(GLOBAL)

#####################################################
# 소스 코드 UTF-8 인코딩 설정

# MSVC 전용: 소스 코드 인코딩을 UTF-8로 지정
if (MSVC)
    add_compile_options(/utf-8)
endif()

# GCC/Clang 전용: 입력/실행 문자셋을 UTF-8로 지정
if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    add_compile_options(-finput-charset=UTF-8 -fexec-charset=UTF-8)
endif()



