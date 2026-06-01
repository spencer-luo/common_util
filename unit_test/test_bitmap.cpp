/**
 * @file test_bitmap.cpp
 * @brief Unit tests for cutl::bitmap, cutl::dynamic_bitmap, cutl::roaring_bitmap.
 */

#include "common_util/bitmap.h"
#include <algorithm>
#include <gtest/gtest.h>
#include <vector>

TEST(BitmapTest, SetGetCountReset)
{
    cutl::bitmap bm(100);
    bm.set(10);
    bm.set(20);
    bm.set(99);
    EXPECT_TRUE(bm.get(10));
    EXPECT_TRUE(bm[20]);
    EXPECT_FALSE(bm.get(50));
    EXPECT_EQ(bm.count(), 3u);

    bm.set(20); // 重复 set 不增加计数
    EXPECT_EQ(bm.count(), 3u);

    bm.reset(99);
    EXPECT_EQ(bm.count(), 2u);

    bm.reset();
    EXPECT_EQ(bm.count(), 0u);
}

TEST(BitmapTest, ToFromString)
{
    cutl::bitmap bm(100);
    bm.set(10);
    bm.set(50);
    auto s = bm.to_string();

    cutl::bitmap bm2(100);
    bm2.from_string(s);
    EXPECT_TRUE(bm2.equals(bm));
    EXPECT_TRUE(bm == bm2);
}

TEST(BitmapTest, ValuelistMatchesSets)
{
    cutl::bitmap bm(64);
    bm.set(0);
    bm.set(7);
    bm.set(63);
    auto list = bm.valuelist();
    std::sort(list.begin(), list.end());
    std::vector<size_t> expected = {0, 7, 63};
    EXPECT_EQ(list, expected);
}

TEST(BitmapTest, BitwiseOps)
{
    cutl::bitmap a(64);
    a.set(1);
    a.set(2);
    cutl::bitmap b(64);
    b.set(2);
    b.set(3);

    auto and_res = a & b;
    EXPECT_EQ(and_res.count(), 1u);
    EXPECT_TRUE(and_res.get(2));

    auto or_res = a | b;
    EXPECT_EQ(or_res.count(), 3u);

    auto xor_res = a ^ b;
    EXPECT_EQ(xor_res.count(), 2u);
    EXPECT_TRUE(xor_res.get(1));
    EXPECT_TRUE(xor_res.get(3));
    EXPECT_FALSE(xor_res.get(2));

    auto not_res = ~a;
    EXPECT_FALSE(not_res.get(1));
    EXPECT_TRUE(not_res.get(0));
}

TEST(BitmapTest, OutOfRangeThrows)
{
    cutl::bitmap bm(8);
    EXPECT_THROW(bm.set(100), std::out_of_range);
    EXPECT_THROW(bm.get(100), std::out_of_range);
}

TEST(DynamicBitmapTest, AutoResizes)
{
    cutl::dynamic_bitmap bm(8);
    EXPECT_EQ(bm.size(), 8u);
    bm.set(100); // 触发扩容
    EXPECT_TRUE(bm.get(100));
    EXPECT_GE(bm.size(), 101u);
}

TEST(RoaringBitmapTest, BasicOps)
{
    cutl::roaring_bitmap rb(64);
    rb.set(10);
    rb.set(80); // 第二个 block
    EXPECT_TRUE(rb.get(10));
    EXPECT_TRUE(rb.get(80));
    EXPECT_FALSE(rb.get(20));
    EXPECT_EQ(rb.count(), 2u);
    EXPECT_GE(rb.size(), 128u);

    auto s = rb.to_string();
    cutl::roaring_bitmap rb2(64);
    rb2.from_string(s);
    EXPECT_TRUE(rb2.equals(rb));
}

TEST(RoaringBitmapTest, BitwiseOps)
{
    cutl::roaring_bitmap a(64);
    a.set(10);
    a.set(80);
    cutl::roaring_bitmap b(64);
    b.set(80);
    b.set(150);

    auto and_res = a & b;
    EXPECT_EQ(and_res.count(), 1u);
    EXPECT_TRUE(and_res.get(80));

    auto or_res = a | b;
    EXPECT_EQ(or_res.count(), 3u);

    auto xor_res = a ^ b;
    EXPECT_EQ(xor_res.count(), 2u);
    EXPECT_TRUE(xor_res.get(10));
    EXPECT_TRUE(xor_res.get(150));
}
