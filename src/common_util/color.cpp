#include "color.h"

namespace cutl
{

color_argb color_argb::red(0xff, 0x00, 0x00);
color_argb color_argb::white(0xff, 0xff, 0xff);
color_argb color_argb::black(0x00, 0x00, 0x00);
color_argb color_argb::aqua(0x00, 0xff, 0xff);
color_argb color_argb::blue(0x00, 0x00, 0xff);
color_argb color_argb::fuschia(0xff, 0x00, 0xff);
color_argb color_argb::gray(0x80, 0x80, 0x80);
color_argb color_argb::green(0x00, 0x80, 0x00);
color_argb color_argb::lime(0x00, 0xff, 0x00);
color_argb color_argb::maroon(0x80, 0x00, 0x00);
color_argb color_argb::navy(0x00, 0x00, 0x80);
color_argb color_argb::olive(0x80, 0x80, 0x00);
color_argb color_argb::purple(0x80, 0x00, 0x80);
color_argb color_argb::silver(0xc0, 0xc0, 0xc0);
color_argb color_argb::teal(0x00, 0x80, 0x80);
color_argb color_argb::yellow(0xff, 0xff, 0x00);

color_argb::color_argb()
  : m_uValue(0xff000000)
{
}

color_argb::color_argb(uint32_t nColorValue)
{
    m_uValue = nColorValue;
}

color_argb::color_argb(uint8_t nRed, uint8_t nGreen, uint8_t nBlue)
{
    m_uValue = 0xff000000 | ((0x000000ff & nRed) << 16) | ((0x000000ff & nGreen) << 8) |
               (0x000000ff & nBlue);
}

color_argb::color_argb(uint8_t nAlpha, uint8_t nRed, uint8_t nGreen, uint8_t nBlue)
{
    m_uValue = ((0x000000ff & nAlpha) << 24) | ((0x000000ff & nRed) << 16) |
               ((0x000000ff & nGreen) << 8) | (0x000000ff & nBlue);
}

color_argb::~color_argb() {}

uint32_t color_argb::value() const
{
    return m_uValue;
}

uint8_t color_argb::a() const
{
    return (m_uValue & 0xff000000) >> 24;
}

uint8_t color_argb::r() const
{
    return (m_uValue & 0x00ff0000) >> 16;
}

uint8_t color_argb::g() const
{
    return (m_uValue & 0x0000ff00) >> 8;
}

uint8_t color_argb::b() const
{
    return m_uValue & 0x000000ff;
}

void color_argb::set_value(uint32_t uValue)
{
    m_uValue = uValue;
}

bool color_argb::operator==(const color_argb& color)
{
    return m_uValue == color.value();
}

bool color_argb::operator!=(const color_argb& color)
{
    return !(*this == color);
}

} // namespace cutl
