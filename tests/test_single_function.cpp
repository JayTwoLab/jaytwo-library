#include <gtest/gtest.h>
#include "j2_library/j2_library.hpp"

// Google Test framework
// TEST(Test Suite Name, Test Name)

// --- Single Function ---

TEST(SingleFunction, Add) {
    EXPECT_EQ(j2::core::add(2, 3), 5);
}

