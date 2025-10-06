#include <gtest/gtest.h>
#include "j2_library/j2_library.hpp"

// Google Test framework
// TEST(Test Suite Name, Test Name)

// --- Single Function ---

TEST(SingleFunction, Add) {
    EXPECT_EQ(j2::add(2, 3), 5);
}

TEST(SingleFunction, Trim) {
    EXPECT_EQ(j2::trim("  hello  "), "hello");
}

TEST(SingleFunction, ToUpper) {
    EXPECT_EQ(j2::to_upper("Abc123!"), "ABC123!");
}

TEST(SingleFunction, Split) {
    auto v = j2::split("a,b,,c", ',');
    ASSERT_EQ(v.size(), 4u);
    EXPECT_EQ(v[0], "a");
    EXPECT_EQ(v[1], "b");
    EXPECT_EQ(v[2], "");
    EXPECT_EQ(v[3], "c");
}

