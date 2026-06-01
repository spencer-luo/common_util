/**
 * @file test_algoutil.cpp
 * @brief Unit tests for cutl::clamp / cutl::in_range.
 */

#include "common_util/algoutil.h"
#include <gtest/gtest.h>

TEST(AlgoUtilTest, Clamp)
{
    EXPECT_EQ(cutl::clamp(7, 0, 10), 7);
    EXPECT_EQ(cutl::clamp(-2, 0, 10), 0);
    EXPECT_EQ(cutl::clamp(15, 0, 10), 10);
    EXPECT_EQ(cutl::clamp(0, 0, 10), 0);
    EXPECT_EQ(cutl::clamp(10, 0, 10), 10);

    EXPECT_DOUBLE_EQ(cutl::clamp(3.14, 0.0, 1.0), 1.0);
    EXPECT_DOUBLE_EQ(cutl::clamp(0.5, 0.0, 1.0), 0.5);
}

TEST(AlgoUtilTest, InRange)
{
    // [min, max) 半开区间
    EXPECT_TRUE(cutl::in_range(0, 0, 10));
    EXPECT_TRUE(cutl::in_range(5, 0, 10));
    EXPECT_FALSE(cutl::in_range(10, 0, 10));
    EXPECT_FALSE(cutl::in_range(-1, 0, 10));
    EXPECT_FALSE(cutl::in_range(11, 0, 10));
}
