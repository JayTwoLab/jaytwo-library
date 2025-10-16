#include <gtest/gtest.h>
#include "j2_library/j2_library.hpp"

namespace jc = j2::core;

TEST(to_string, to_string)
{
    float  f = 10.12f; // set f for 'float' value 10.12
    double d = 10.12;

    EXPECT_EQ(jc::to_string<float>(f, 2),  "10.12"); // set precision to 2
    EXPECT_EQ(jc::to_string<double>(d, 2), "10.12"); // set precision to 2 

    EXPECT_EQ(jc::to_string<float>(f, 10),  "10.1199998856"); // set precision to 10
    EXPECT_EQ(jc::to_string<double>(d, 20), "10.11999999999999921840"); // set precision to 20 
}


TEST(to_string, is_equal)
{
    EXPECT_TRUE(jc::is_equal<double>(10.12, 10.12, 2)); // 10.12  == 10.12
    EXPECT_TRUE(jc::is_equal<double>(10.12, 10.1,  1)); // 10.1   == 10.1
    EXPECT_TRUE(jc::is_equal<double>(10.12, 10.12, 3)); // 10.120 == 10.120

    EXPECT_TRUE(jc::is_equal<double>( 10.12, 10.121,  2)); // 10.12 == 10.12 
    EXPECT_FALSE(jc::is_equal<double>(10.12, 10.121,  3)); // 10.120 != 10.121
    EXPECT_FALSE(jc::is_equal<double>(10.12, 10.121, 20)); // 10.11999999999999921840 != 10.12100000000000044054

    EXPECT_FALSE(jc::is_equal<double>(10.12, 10.1201,    20)); // 10.11999999999999921840 != 10.12010000000000076170
    EXPECT_FALSE(jc::is_equal<double>(10.12, 10.12001,   20)); // 10.11999999999999921840 != 10.12001000000000061618
    EXPECT_FALSE(jc::is_equal<double>(10.12, 10.120001,  20)); // 10.11999999999999921840 != 10.12000100000000024636
    EXPECT_FALSE(jc::is_equal<double>(10.12, 10.1200001, 20)); // 10.11999999999999921840 != 10.12000010000000038701

    EXPECT_TRUE(jc::is_equal<double>(10.12, 10.12, 10));  // 10.1200000000 == 10.1200000000
}

