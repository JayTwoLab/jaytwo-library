include(FetchContent)

FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

add_executable(j2_tests test_basic.cpp)

target_link_libraries(j2_tests
  PRIVATE
    j2_library::j2_library
    GTest::gtest_main
)

if (J2_LIBRARY_ENABLE_WARNINGS)
  j2_enable_warnings(j2_tests)
endif()

include(GoogleTest)
gtest_discover_tests(j2_tests)
