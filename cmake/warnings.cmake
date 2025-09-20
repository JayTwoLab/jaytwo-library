# 컴파일러 경고 플래그 설정
function(j2_enable_warnings target_name)
  if (MSVC)
    target_compile_options(${target_name} PRIVATE /W4 /permissive-)
  else()
    target_compile_options(${target_name} PRIVATE
      -Wall -Wextra -Wpedantic
      -Wconversion -Wsign-conversion
      -Wshadow -Wnon-virtual-dtor
      -Wold-style-cast -Woverloaded-virtual
      -Wduplicated-cond -Wduplicated-branches
      -Wlogical-op -Wnull-dereference
    )
  endif()
endfunction()
