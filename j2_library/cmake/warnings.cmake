#########################################
# 전역 범위(Global scope) 에서 중복 include를 막습니다.
include_guard(GLOBAL)

###################################
# 컴파일러 경고 플래그 설정
###################################
function(j2_enable_warnings target_name)
  if (MSVC) # Visual Studio용 설정 
        target_compile_options(${target_name} PRIVATE
            /W4                # 높은 수준의 경고 (MSVC 권장 설정)
            /permissive-       # 표준 C++ 규격에 더 엄격하게 맞춤
        )
  elseif(APPLE AND (CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")) # Mac용 Clang
      target_compile_options(${target_name} PRIVATE
        -Wall                 # 대부분의 일반적인 경고 활성화
        -Wextra               # 추가적인 경고 활성화
        -Wpedantic            # 표준 C++ 규격 위반에 대한 경고
        -Wconversion          # 타입 변환 시 데이터 손실 가능성 경고
        -Wsign-conversion     # 부호 있는/없는 정수 변환 경고
        -Wshadow              # 동일 이름 변수 가리기(shadowing) 경고
        -Wnon-virtual-dtor    # 가상 함수가 있는 클래스의 소멸자가 virtual이 아닐 경우 경고
        -Wold-style-cast      # C 스타일 캐스트 사용 경고
        -Woverloaded-virtual  # 가상 함수가 오버로드로 가려질 때 경고
        -Wunreachable-code    # 도달 불가능한 코드에 대한 경고
        -Wnull-dereference    # 널 포인터 역참조 가능성 경고
        -Wdouble-promotion    # float이 double로 자동 승격되는 경우 경고
        -Wformat=2            # printf/scanf 류 포맷 문자열 검사 강화
      )
  else() # 그외 설정 (gcc 가정)
    target_compile_options(${target_name} PRIVATE
        -Wall                 # 대부분의 일반적인 경고 활성화
        -Wextra               # 추가적인 경고 활성화
        -Wpedantic            # 표준 C++ 규격 위반에 대한 경고
        -Wconversion          # 타입 변환 시 데이터 손실 가능성 경고
        -Wsign-conversion     # 부호 있는/없는 정수 변환 경고
        -Wshadow              # 동일 이름 변수 가리기(shadowing) 경고
        -Wnon-virtual-dtor    # 가상 함수가 있는 클래스의 소멸자가 virtual이 아닐 경우 경고
        -Wold-style-cast      # C 스타일 캐스트 사용 경고
        -Woverloaded-virtual  # 가상 함수가 오버로드로 가려질 때 경고
        -Wduplicated-cond     # 중복된 조건식에 대한 경고 (GCC 전용)
        -Wduplicated-branches # if-else 등의 분기문이 동일한 코드일 때 경고 (GCC 전용)
        -Wlogical-op          # 논리 연산자 사용 오류 가능성 경고 (GCC 전용)
        -Wnull-dereference    # 널 포인터 역참조 가능성 경고
    )
  endif()
endfunction()

