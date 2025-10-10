#include <gtest/gtest.h>
#include "j2_library/j2_library.hpp"

// Google Test framework
// TEST(Test Suite Name, Test Name)

// --- Single Function ---

namespace jc = j2::core;

TEST(SingleFunction, Add) {
    EXPECT_EQ(jc::add(2, 3), 5);
}

