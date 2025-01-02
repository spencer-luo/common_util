#pragma once

#include <cstdint>

namespace cutl
{

class color_argb
{
public:
    color_argb();
    color_argb(uint32_t nColorValue);
    color_argb(uint8_t nRed, uint8_t nGreen, uint8_t nBlue);
    color_argb(uint8_t nAlpha, uint8_t nRed, uint8_t nGreen, uint8_t nBlue);
    virtual ~color_argb();

public:
    uint32_t value() const;
    uint8_t a() const;
    uint8_t r() const;
    uint8_t g() const;
    uint8_t b() const;
    void set_value(uint32_t uValue);
    bool operator==(const color_argb& color);
    bool operator!=(const color_argb& color);

public:
    static color_argb white;   // 白色
    static color_argb black;   // 黑色
    static color_argb aqua;    // 水(浅)绿
    static color_argb blue;    // 蓝色
    static color_argb fuschia; // 紫(粉)红
    static color_argb gray;    // 灰色
    static color_argb green;   // 绿色
    static color_argb lime;    // 酸橙
    static color_argb maroon;  // 栗色
    static color_argb navy;    // 海军色
    static color_argb olive;   // 橄榄色
    static color_argb purple;  // 紫色
    static color_argb red;     // 红色
    static color_argb silver;  // 银色
    static color_argb teal;    // 水鸭色
    static color_argb yellow;  // 黄色
private:
    uint32_t m_uValue;
};

} // namespace cutl
