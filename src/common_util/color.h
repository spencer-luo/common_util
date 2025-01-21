/**
 * @copyright Copyright (c) 2025, Spencer.Luo. All Rights Reserved.
 * 
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 * 
 *       http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions and limitations.
 * 
 * @file color.h
 * @brief The wraper for argb color space.
 * @author Spencer
 * @date 2025-01-21
 */

#pragma once

#include <cstdint>

namespace cutl
{

/**
 * @brief argb clolor space
 * 
 */
class color_argb
{
public:
    /**
     * @brief Construct a new color argb object, default color is black.
     * 
     */
    color_argb();

    /**
     * @brief Construct a new color argb object
     * 
     * @param value the value of argb color
     */
    color_argb(uint32_t value);

    /**
     * @brief Construct a new color argb object
     * 
     * @param r red value
     * @param g green value
     * @param b blue value
     */
    color_argb(uint8_t r, uint8_t g, uint8_t b);
    /**
     * @brief Construct a new color argb object
     * 
     * @param a alpha value
     * @param r red value
     * @param g green value
     * @param b blue value
     */
    color_argb(uint8_t a, uint8_t r, uint8_t g, uint8_t b);

    /**
     * @brief Destroy the color argb object
     * 
     */
    virtual ~color_argb();

public:
    /**
     * @brief Get the value of argb color
     * 
     * @return uint32_t 
     */
    uint32_t value() const;

    /**
     * @brief Get alpha value
     * 
     * @return uint8_t 
     */
    uint8_t a() const;
    /**
     * @brief Get red value
     * 
     * @return uint8_t 
     */
    uint8_t r() const;
    /**
     * @brief Get green value
     * 
     * @return uint8_t 
     */
    uint8_t g() const;
    /**
     * @brief Get blue value
     * 
     * @return uint8_t 
     */
    uint8_t b() const;
    /**
     * @brief Set the value object
     * 
     * @param uValue 
     */
    void set_value(uint32_t uValue);
    /**
     * @brief Is equal to another color_argb object
     * 
     * @param color 
     * @return true 
     * @return false 
     */
    bool operator==(const color_argb& color);
    /**
     * @brief Is not equal to another color_argb object
     * 
     * @param color 
     * @return true 
     * @return false 
     */
    bool operator!=(const color_argb& color);

public:
    /**
     * @brief Const color_argb object, white color
     * 
     */
    static color_argb white;
    /**
     * @brief Const color_argb object, black color
     * 
     */
    static color_argb black;
    /**
     * @brief Const color_argb object, aqua color
     * 
     */
    static color_argb aqua;
    /**
     * @brief Const color_argb object, blue color
     * 
     */
    static color_argb blue;
    /**
     * @brief Const color_argb object, fuchsia color
     * 
     */
    static color_argb fuschia;
    /**
     * @brief Const color_argb object, gray color
     * 
     */
    static color_argb gray;
    /**
     * @brief Const color_argb object, green color
     * 
     */
    static color_argb green;
    /**
     * @brief Const color_argb object, lime color
     * 
     */
    static color_argb lime;
    /**
     * @brief Const color_argb object, maroon color
     * 
     */
    static color_argb maroon;
    /**
     * @brief Const color_argb object, navy color
     * 
     */
    static color_argb navy;
    /**
     * @brief Const color_argb object, olive color
     * 
     */
    static color_argb olive;
    /**
     * @brief Const color_argb object, orange color
     * 
     */
    static color_argb purple;
    /**
     * @brief Const color_argb object, red color
     * 
     */
    static color_argb red;
    /**
     * @brief Const color_argb object, silver color
     * 
     */
    static color_argb silver;
    /**
     * @brief Const color_argb object, teal color
     * 
     */
    static color_argb teal;
    /**
     * @brief Const color_argb object, yellow color
     * 
     */
    static color_argb yellow;

private:
    uint32_t m_uValue;
};

} // namespace cutl
