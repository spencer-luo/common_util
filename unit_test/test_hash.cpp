/**
 * @file test_hash.cpp
 * @brief Unit tests for cutl hash functions.
 */

#include "common_util/hash.h"
#include <gtest/gtest.h>
#include <string>
#include <unordered_set>

TEST(HashTest, SameInputProducesSameOutput)
{
    std::string s = "Hello, common_util!";
    EXPECT_EQ(cutl::hash_polynomial_rolling(s), cutl::hash_polynomial_rolling(s));
    EXPECT_EQ(cutl::hash_djb2(s), cutl::hash_djb2(s));
    EXPECT_EQ(cutl::hash_fnv1_32(s), cutl::hash_fnv1_32(s));
    EXPECT_EQ(cutl::hash_fnv1a_32(s), cutl::hash_fnv1a_32(s));
    EXPECT_EQ(cutl::hash_fnv1a_64(s), cutl::hash_fnv1a_64(s));
    EXPECT_EQ(cutl::hash_one_at_a_time(s), cutl::hash_one_at_a_time(s));
    EXPECT_EQ(cutl::hash_lookup3(s.c_str(), s.size()), cutl::hash_lookup3(s.c_str(), s.size()));
    EXPECT_EQ(cutl::hash_murmur3_32(s.c_str(), s.size()),
              cutl::hash_murmur3_32(s.c_str(), s.size()));
    EXPECT_EQ(cutl::hash_murmur3_64(s, 0), cutl::hash_murmur3_64(s, 0));
}

TEST(HashTest, DifferentInputProducesDifferentOutput)
{
    std::string a = "abc";
    std::string b = "abd";
    EXPECT_NE(cutl::hash_djb2(a), cutl::hash_djb2(b));
    EXPECT_NE(cutl::hash_fnv1a_32(a), cutl::hash_fnv1a_32(b));
    EXPECT_NE(cutl::hash_fnv1a_64(a), cutl::hash_fnv1a_64(b));
    EXPECT_NE(cutl::hash_murmur3_32(a.c_str(), a.size()),
              cutl::hash_murmur3_32(b.c_str(), b.size()));
    EXPECT_NE(cutl::hash_murmur3_64(a, 0), cutl::hash_murmur3_64(b, 0));
}

TEST(HashTest, IntegerHashes)
{
    EXPECT_EQ(cutl::hash_thomas_wang(123u), cutl::hash_thomas_wang(123u));
    EXPECT_NE(cutl::hash_thomas_wang(0u), cutl::hash_thomas_wang(1u));

    // 哈希表索引的输出范围应在 [0, table_size)
    constexpr uint32_t kTable = 100;
    for (uint32_t i = 0; i < 50; ++i)
    {
        auto h_mul = cutl::hash_multiplication(i, kTable);
        auto h_div = cutl::hash_division(i, kTable);
        EXPECT_LT(h_mul, kTable);
        EXPECT_LT(h_div, kTable);
    }
}

TEST(HashTest, ReasonableSpread)
{
    // 1000 个不同字符串，murmur3_64 应至少产生 800 个不同哈希值
    std::unordered_set<uint64_t> hashes;
    for (int i = 0; i < 1000; ++i)
    {
        hashes.insert(cutl::hash_murmur3_64("element_" + std::to_string(i), 0));
    }
    EXPECT_GE(hashes.size(), 800u);
}
