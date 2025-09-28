#########################################
# 전역 범위(Global scope) 에서 중복 include를 막습니다.
include_guard(GLOBAL)

###################################
# AddressSanitizer(ASan) / UndefinedBehaviorSanitizer(UBSan) 활성화 함수
###################################

function(j2_enable_sanitizers target_name)
# j2_enable_sanitizers(target_name) 함수는 지정한 타깃(target_name)에
# 런타임 오류 검출 도구(Sanitizer) 옵션을 적용하는 함수입니다.

  if (MSVC)
    message(WARNING "MSVC에서는 AddressSanitizer 설정이 제한적일 수 있습니다.") 
    # MSVC는 Clang-cl 기반이 아니면 ASan 지원이 불완전함
  else()
    target_compile_options(${target_name} PRIVATE
        -fsanitize=address,undefined    # AddressSanitizer(메모리 에러) + UBSan(정의되지 않은 동작) 활성화
        -fno-omit-frame-pointer         # 함수 프레임 포인터를 생략하지 않음 (스택 추적 시 필요)
    )
    target_link_options(${target_name} PRIVATE
        -fsanitize=address,undefined    # 링커에도 Sanitizer 옵션 전달
    )
  endif()
endfunction()

##################################
# GCC vs Clang 차이
#  Clang은 -fsanitize=undefined에서 더 많은 체크(예: unsigned integer overflow)를 지원합니다.
#  GCC는 일부 체크가 누락되어 있을 수 있습니다.
##################################
# GCC에서의 지원 여부
#  -fsanitize=address (ASan)
#   GCC 4.8 이상에서 지원. 정상 동작합니다.
#
# -fsanitize=undefined (UBSan)
#  GCC 4.9 이상에서 지원. 정상 동작합니다.
#  (undefined는 여러 개의 UBSan 체크 모듈을 한 번에 켜주는 집합 옵션입니다.)
#
# -fno-omit-frame-pointer
#  GCC, Clang 모두 지원. 스택 트레이스를 정확히 하기 위해 필요합니다.
#
# 링커 옵션의 -fsanitize=address,undefined
# GCC도 libasan, libubsan을 자동 링크합니다.
##################################
# 멀티스레드 프로그램
#  ASan을 멀티스레드 환경에서 사용할 경우, TLS(Thread Local Storage) 지원이 필요합니다.
# 최신 GCC/Clang에서는 기본 지원합니다
###################################
# 최적화와 함께 사용할 때
#  -O2 이상 최적화 옵션과 함께 쓰면 성능에 영향이 크며, false positive가 줄긴 하지만 실행 속도가 크게 느려집니다.
#  보통은 -O1 또는 -O0 + -g 디버그 빌드에서 켜는 것이 일반적입니다.
###################################


