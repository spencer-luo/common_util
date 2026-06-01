/**
 * @file test_color.cpp
 * @brief Unit tests for cutl::color_argb.
 */

#include "common_util/color.h"
#include <gtest/gtest.h>

TEST(ColorTest, DefaultConstructIsBlackOpaque)
{
    cutl::color_argb c;
    EXPECT_EQ(c.a(), 0xff);
    EXPECT_EQ(c.r(), 0x00);
    EXPECT_EQ(c.g(), 0x00);
    EXPECT_EQ(c.b(), 0x00);
}

TEST(ColorTest, ConstructFromUint32)
{
    cutl::color_argb c(0x80FF1020u);
    EXPECT_EQ(c.value(), 0x80FF1020u);
    EXPECT_EQ(c.a(), 0x80);
    EXPECT_EQ(c.r(), 0xFF);
    EXPECT_EQ(c.g(), 0x10);
    EXPECT_EQ(c.b(), 0x20);
}

TEST(ColorTest, ConstructFromRGB)
{
    cutl::color_argb c(0x12, 0x34, 0x56);
    EXPECT_EQ(c.a(), 0xFF); // 默认 alpha
    EXPECT_EQ(c.r(), 0x12);
    EXPECT_EQ(c.g(), 0x34);
    EXPECT_EQ(c.b(), 0x56);
}

TEST(ColorTest, ConstructFromARGB)
{
    cutl::color_argb c(0x55, 0x12, 0x34, 0x56);
    EXPECT_EQ(c.a(), 0x55);
    EXPECT_EQ(c.r(), 0x12);
    EXPECT_EQ(c.g(), 0x34);
    EXPECT_EQ(c.b(), 0x56);
}

TEST(ColorTest, EqualityOperators)
{
    cutl::color_argb red1(0xFF, 0x00, 0x00);
    cutl::color_argb red2(0xFF, 0x00, 0x00);
    cutl::color_argb green(0x00, 0xFF, 0x00);
    EXPECT_TRUE(red1 == red2);
    EXPECT_TRUE(red1 != green);
}

TEST(ColorTest, SetValue)
{
    cutl::color_argb c;
    c.set_value(0x12345678u);
    EXPECT_EQ(c.value(), 0x12345678u);
}

TEST(ColorTest, PredefinedColors)
{
    EXPECT_EQ(cutl::color_argb::white.r(), 0xFF);
    EXPECT_EQ(cutl::color_argb::white.g(), 0xFF);
    EXPECT_EQ(cutl::color_argb::white.b(), 0xFF);

    EXPECT_EQ(cutl::color_argb::black.r(), 0x00);
    EXPECT_EQ(cutl::color_argb::black.g(), 0x00);
    EXPECT_EQ(cutl::color_argb::black.b(), 0x00);

    EXPECT_EQ(cutl::color_argb::red.r(), 0xFF);
    EXPECT_EQ(cutl::color_argb::red.g(), 0x00);
    EXPECT_EQ(cutl::color_argb::red.b(), 0x00);
}
