#include <gtest/gtest.h>
#include "j2_library/j2_library.hpp"
#include "j2_library/version.hpp"

TEST(Core, Add) { EXPECT_EQ(j2::add(2, 3), 5); }

TEST(Core, Trim) { EXPECT_EQ(j2::trim("  hello  "), "hello"); }

TEST(Core, ToUpper) { EXPECT_EQ(j2::to_upper("Abc123!"), "ABC123!"); }

TEST(Core, Split)
{
    auto v = j2::split("a,b,,c", ',');
    ASSERT_EQ(v.size(), 4u);
    EXPECT_EQ(v[0], "a");
    EXPECT_EQ(v[1], "b");
    EXPECT_EQ(v[2], "");
    EXPECT_EQ(v[3], "c");
}

TEST(Version, Present) { ASSERT_TRUE(std::string(j2::VersionString).size() > 0); }
