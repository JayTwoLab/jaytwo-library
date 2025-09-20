add_executable(j2_hello main.cpp)
target_link_libraries(j2_hello PRIVATE j2_library::j2_library)

if (J2_LIBRARY_ENABLE_WARNINGS)
  j2_enable_warnings(j2_hello)
endif()
