#include <gtest/gtest.h>
#include "j2_library/j2_library.hpp"

// Google Test framework
// TEST(Test Suite Name, Test Name)

// --- Version ---

TEST(Version, Present) {
    ASSERT_TRUE(std::string(j2::VersionString).size() > 0);
}


