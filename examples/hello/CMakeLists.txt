add_executable(j2_hello main.cpp)
target_link_libraries(j2_hello PRIVATE j2_library::j2_library)

# 선택: 경고 플래그 적용
if (COMMAND j2_enable_warnings)
  j2_enable_warnings(j2_hello)
endif()
