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

TEST(ColorTest, AllPredefinedColorsAccessible)
{
    // 烟雾测试：所有预定义颜色都应可访问且 alpha 不为 0（否则就是构造异常）。
    const cutl::color_argb* preset[] = {
        &cutl::color_argb::white,    &cutl::color_argb::black,    &cutl::color_argb::aqua,
        &cutl::color_argb::blue,     &cutl::color_argb::fuschia,  &cutl::color_argb::gray,
        &cutl::color_argb::green,    &cutl::color_argb::lime,     &cutl::color_argb::maroon,
        &cutl::color_argb::navy,     &cutl::color_argb::olive,    &cutl::color_argb::purple,
        &cutl::color_argb::red,      &cutl::color_argb::silver,   &cutl::color_argb::teal,
        &cutl::color_argb::yellow,
    };
    for (auto* c : preset)
    {
        EXPECT_NE(c->a(), 0x00);
    }

    // 个别颜色的具体 RGB 验证
    EXPECT_EQ(cutl::color_argb::aqua.r(), 0x00);
    EXPECT_EQ(cutl::color_argb::aqua.g(), 0xFF);
    EXPECT_EQ(cutl::color_argb::aqua.b(), 0xFF);

    EXPECT_EQ(cutl::color_argb::blue.r(), 0x00);
    EXPECT_EQ(cutl::color_argb::blue.g(), 0x00);
    EXPECT_EQ(cutl::color_argb::blue.b(), 0xFF);

    EXPECT_EQ(cutl::color_argb::yellow.r(), 0xFF);
    EXPECT_EQ(cutl::color_argb::yellow.g(), 0xFF);
    EXPECT_EQ(cutl::color_argb::yellow.b(), 0x00);
}
