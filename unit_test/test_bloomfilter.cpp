/**
 * @file test_bloomfilter.cpp
 * @brief Unit tests for cutl::bloom_filter and helper functions.
 */

#include "common_util/bloomfilter.h"
#include <gtest/gtest.h>
#include <string>
#include <vector>

TEST(BloomFilterTest, AddedElementsContained)
{
    cutl::bloom_filter bloom(2048, 4);
    std::vector<std::string> items = {"apple", "banana", "orange", "grape", "common_util"};
    for (const auto& it : items)
    {
        bloom.add(it);
    }
    for (const auto& it : items)
    {
        EXPECT_TRUE(bloom.contains(it)) << "missing " << it;
    }
}

TEST(BloomFilterTest, ClearMakesAllContainsFalse)
{
    cutl::bloom_filter bloom(1024, 3);
    bloom.add("a");
    bloom.add("b");
    EXPECT_TRUE(bloom.contains("a"));
    bloom.clear();
    EXPECT_FALSE(bloom.contains("a"));
    EXPECT_FALSE(bloom.contains("b"));
}

TEST(BloomFilterTest, AutoCalculatedConstructor)
{
    cutl::bloom_filter bloom(100, cutl::bloom_error_rate::percent_01);
    bloom.add("hello");
    EXPECT_TRUE(bloom.contains("hello"));
}

TEST(BloomFilterHelperTest, OptimalParametersConsistent)
{
    size_t n = 1000;
    double p = 0.01;
    auto m = cutl::bloom_optimal_m(n, p);
    auto k = cutl::bloom_optimal_k(m, n);
    auto fp = cutl::bloom_false_positive_rate(m, n, k);

    EXPECT_GT(m, 0u);
    EXPECT_GT(k, 0u);
    // 实际误判率应当不大于目标的 5 倍（容忍参数取整造成的偏差）
    EXPECT_LT(fp, p * 5.0);

    size_t opt_m = 0, opt_k = 0;
    double opt_fp = 0.0;
    cutl::bloom_optimal_parameters(n, p, opt_m, opt_k, opt_fp);
    EXPECT_EQ(opt_m, m);
    EXPECT_EQ(opt_k, k);
}
