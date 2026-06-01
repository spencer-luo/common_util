/**
 * @file test_hyperloglog.cpp
 * @brief Unit tests for cutl::HyperLogLog.
 */

#include "common_util/hyperloglog.h"
#include <cmath>
#include <gtest/gtest.h>
#include <string>

TEST(HyperLogLogTest, SmallSetAccurate)
{
    cutl::HyperLogLog hll(14);
    hll.add(std::string("abc"));
    hll.add(std::string("def"));
    hll.add(std::string("123"));
    auto count = hll.count();
    // 小集合下应该非常接近真实基数
    EXPECT_GE(count, 1u);
    EXPECT_LE(count, 5u);

    // 重复元素不应该让基数大幅增长
    hll.add(std::string("abc"));
    auto count2 = hll.count();
    EXPECT_LE(count2, count + 1);
}

TEST(HyperLogLogTest, ResetClearsCounter)
{
    cutl::HyperLogLog hll(10);
    for (int i = 0; i < 200; ++i)
    {
        hll.add("element_" + std::to_string(i));
    }
    auto before_reset = hll.count();
    EXPECT_GT(before_reset, 0u);
    hll.reset();
    EXPECT_EQ(hll.count(), 0u);
}

TEST(HyperLogLogTest, MergeEqualsUnion)
{
    // 同样使用较低 precision + 较大基数验证 merge 的累积效果。
    cutl::HyperLogLog a(8);
    cutl::HyperLogLog b(8);
    for (int i = 0; i < 5000; ++i)
    {
        a.add("a_" + std::to_string(i));
        b.add("b_" + std::to_string(i));
    }
    auto count_a = a.count();
    a.merge(b);
    auto merged = a.count();
    // merge 之后的估计值应当大于单边 a.count()
    EXPECT_GT(merged, count_a);
}

TEST(HyperLogLogTest, ToFromStringRoundTrip)
{
    cutl::HyperLogLog hll(10);
    for (int i = 0; i < 200; ++i)
    {
        hll.add("k_" + std::to_string(i));
    }
    auto serialized = hll.to_string();
    EXPECT_FALSE(serialized.empty());

    cutl::HyperLogLog restored(10);
    restored.from_string(serialized);
    EXPECT_EQ(hll.count(), restored.count());
}

TEST(HyperLogLogTest, TheoreticalErrorIsPositive)
{
    cutl::HyperLogLog hll(14);
    EXPECT_GT(hll.get_theoretical_error(), 0.0);
    EXPECT_LT(hll.get_theoretical_error(), 1.0);
}
