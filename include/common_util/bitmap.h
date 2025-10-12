/**
 * @copyright Copyright (c) 2025, Spencer.Luo. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *       http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software distributed under the
 * License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either
 * express or implied. See the License for the specific language governing permissions and
 * limitations.
 *
 * @file bitmap.h
 * @brief bitmap class, include bitmap、dynamic_bitmap、roaring_bitmap.
 * @author Spencer
 * @date 2025-10-12
 */

#pragma once

#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

namespace cutl
{

/**
 * @brief Interface class for bitmap operations
 *
 * This abstract class defines the common interface for all bitmap implementations,
 * including basic bit manipulation and conversion operations.
 */
class ibitmap
{
public:
    /**
     * @brief Destroy the ibitmap object
     *
     * Virtual destructor to ensure proper cleanup of derived classes
     */
    virtual ~ibitmap() = default;

public:
    // Basic operations
    /**
     * @brief Set the bit at specified position to 1
     *
     * @param position The index of the bit to set
     */
    virtual void set(size_t position) = 0;

    /**
     * @brief Get the value of the bit at specified position
     *
     * @param position The index of the bit to retrieve
     * @return true if the bit is set (1), false otherwise (0)
     */
    virtual bool get(size_t position) const = 0;

    /**
     * @brief Set the bit at specified position to 0
     *
     * @param position The index of the bit to reset
     */
    virtual void reset(size_t position) = 0;

    /**
     * @brief Reset all bits to 0
     */
    virtual void reset() = 0;

    /**
     * @brief Get the number of bits set to 1
     *
     * @return size_t The count of set bits
     */
    virtual size_t count() const = 0;

    /**
     * @brief Get the total number of bits that the bitmap can hold
     *
     * @return size_t The capacity of the bitmap in bits
     */
    virtual size_t size() const = 0;

    /**
     * @brief Convert the bitmap to a string representation
     *
     * @return std::string String representation of the bitmap
     */
    virtual std::string to_string() const = 0;

    /**
     * @brief Initialize the bitmap from a string representation
     *
     * @param text The string to parse and load into the bitmap
     */
    virtual void from_string(const std::string& text) = 0;

    /**
     * @brief Get a list of positions where bits are set to 1
     *
     * @return std::vector<size_t> Vector containing indices of set bits
     */
    virtual std::vector<size_t> valuelist() const = 0;

    // Operator overloads
    /**
     * @brief Overload of [] operator to get bit value
     *
     * @param position The index of the bit to retrieve
     * @return true if the bit is set (1), false otherwise (0)
     */
    virtual bool operator[](size_t position) const = 0;

    // Comparison operations
    /**
     * @brief Check if two bitmaps are equal
     *
     * @param other The bitmap to compare with
     * @return true if bitmaps are equal, false otherwise
     */
    virtual bool equals(const ibitmap& other) const = 0;
};

/**
 * @brief A fixed-size bitmap implementation
 *
 * This class provides a basic bitmap with a fixed size, implementing all
 * methods defined in the ibitmap interface. It stores bits in a vector of bytes.
 */
class bitmap : public ibitmap
{
protected:
    std::vector<uint8_t> bits_; ///< Underlying storage for bits
    size_t size_;               ///< Total number of bits the bitmap can hold

public:
    /**
     * @brief Construct a new bitmap object with specified size
     *
     * @param size The number of bits the bitmap should hold
     */
    bitmap(size_t size);

    /**
     * @brief Destroy the bitmap object
     */
    ~bitmap() = default;

public:
    /**
     * @brief Set the bit at specified position to 1
     *
     * @param position The index of the bit to set
     * @throw std::out_of_range If position is beyond bitmap size
     */
    void set(size_t position) override;

    /**
     * @brief Get the value of the bit at specified position
     *
     * @param position The index of the bit to retrieve
     * @return true if the bit is set (1), false otherwise (0)
     * @throw std::out_of_range If position is beyond bitmap size
     */
    bool get(size_t position) const override;

    /**
     * @brief Overload of [] operator to get bit value
     *
     * @param position The index of the bit to retrieve
     * @return true if the bit is set (1), false otherwise (0)
     */
    bool operator[](size_t position) const override { return get(position); }

    /**
     * @brief Set the bit at specified position to 0
     *
     * @param position The index of the bit to reset
     * @throw std::out_of_range If position is beyond bitmap size
     */
    void reset(size_t position) override;

    /**
     * @brief Reset all bits to 0
     */
    void reset() override;

    /**
     * @brief Get the number of bits set to 1
     *
     * @return size_t The count of set bits
     */
    size_t count() const override;

    /**
     * @brief Get the total number of bits that the bitmap can hold
     *
     * @return size_t The capacity of the bitmap in bits
     */
    size_t size() const override { return size_; }

    /**
     * @brief Convert the bitmap to a compressed hex string representation
     *
     * @return std::string Compressed hex string of the bitmap
     */
    std::string to_string() const;

    /**
     * @brief Initialize the bitmap from a hex string representation
     *
     * @param text The hex string to parse and load
     * @throw std::runtime_error If the input string contains invalid hex characters
     */
    void from_string(const std::string& text);

    /**
     * @brief Get a list of positions where bits are set to 1
     *
     * @return std::vector<size_t> Vector containing indices of set bits
     */
    std::vector<size_t> valuelist() const;

    /**
     * @brief Check if two bitmaps are equal
     *
     * @param other The bitmap to compare with
     * @return true if bitmaps are equal, false otherwise
     */
    bool equals(const ibitmap& other) const override;

    /**
     * @brief Check if two bitmaps are equal
     *
     * @param other The bitmap to compare with
     * @return true if bitmaps are equal, false otherwise
     */
    bool operator==(const bitmap& other) const { return equals(other); }

    /**
     * @brief Check if two bitmaps are not equal
     *
     * @param other The bitmap to compare with
     * @return true if bitmaps are not equal, false otherwise
     */
    bool operator!=(const bitmap& other) const { return !equals(other); }

    /**
     * @brief Perform bitwise AND with another bitmap
     *
     * @param other The bitmap to AND with
     * @return bitmap Result of the AND operation
     */
    bitmap operator&(const bitmap& other) const;

    /**
     * @brief Perform bitwise OR with another bitmap
     *
     * @param other The bitmap to OR with
     * @return bitmap Result of the OR operation
     */
    bitmap operator|(const bitmap& other) const;

    /**
     * @brief Perform bitwise NOT operation
     *
     * @return bitmap Result of the NOT operation
     */
    bitmap operator~() const;

    /**
     * @brief Perform bitwise XOR with another bitmap
     *
     * @param other The bitmap to XOR with
     * @return bitmap Result of the XOR operation
     * @throw std::invalid_argument If bitmaps have different sizes
     */
    bitmap operator^(const bitmap& other) const;

    /**
     * @brief Perform bitwise AND with another bitmap and assign result
     *
     * @param other The bitmap to AND with
     * @return bitmap& Reference to this bitmap after operation
     * @throw std::invalid_argument If bitmaps have different sizes
     */
    bitmap& operator&=(const bitmap& other);

    /**
     * @brief Perform bitwise OR with another bitmap and assign result
     *
     * @param other The bitmap to OR with
     * @return bitmap& Reference to this bitmap after operation
     * @throw std::invalid_argument If bitmaps have different sizes
     */
    bitmap& operator|=(const bitmap& other);

    /**
     * @brief Perform bitwise XOR with another bitmap and assign result
     *
     * @param other The bitmap to XOR with
     * @return bitmap& Reference to this bitmap after operation
     * @throw std::invalid_argument If bitmaps have different sizes
     */
    bitmap& operator^=(const bitmap& other);

private:
    /**
     * @brief Convert the bitmap to a hexadecimal string
     *
     * @param compress 0 for uncompressed, 1 for compressed (truncates trailing zeros)
     * @return std::string Hexadecimal string representation
     */
    std::string to_hex(int compress = 1) const;
};

/**
 * @brief A dynamically resizable bitmap implementation
 *
 * This class extends the basic bitmap with automatic resizing functionality,
 * allowing it to grow as needed when setting bits beyond the current capacity.
 */
class dynamic_bitmap : public bitmap
{
public:
    /**
     * @brief Construct a new dynamic_bitmap object
     *
     * @param init_size Initial number of bits (default: 64)
     */
    dynamic_bitmap(size_t init_size = 64);

    /**
     * @brief Destroy the dynamic_bitmap object
     */
    ~dynamic_bitmap() = default;

public:
    /**
     * @brief Set the bit at specified position to 1, resizing if necessary
     *
     * @param position The index of the bit to set
     */
    void set(size_t position) override;

private:
    /**
     * @brief Ensure the bitmap can hold at least the specified number of bits
     *
     * @param minSize The minimum number of bits required
     */
    void ensureCapacity(size_t minSize);
};

/**
 * @brief A roaring bitmap implementation for efficient sparse bit storage
 *
 * This class uses a block-based approach to store bits, which is more memory-efficient
 * for sparse bitmaps. It divides the bitmap into blocks of fixed size and only
 * allocates storage for blocks that contain set bits.
 */
class roaring_bitmap : public ibitmap
{
private:
    size_t block_size_; ///< Size of each block in bits
    std::unordered_map<size_t, bitmap>
      container_; ///< Storage for blocks (key: block index, value: bitmap block)

public:
    /**
     * @brief Construct a new roaring_bitmap object
     *
     * @param blockSize The size of each block in bits
     */
    roaring_bitmap(size_t blockSize);

    /**
     * @brief Destroy the roaring_bitmap object
     */
    ~roaring_bitmap() = default;

public:
    /**
     * @brief Get the block size
     *
     * @return size_t The size of each block in bits
     */
    size_t block_size() const { return block_size_; }

    /**
     * @brief Set the bit at specified position to 1
     *
     * Creates the necessary block if it doesn't exist
     * @param position The index of the bit to set
     */
    void set(size_t position) override;

    /**
     * @brief Get the value of the bit at specified position
     *
     * @param position The index of the bit to retrieve
     * @return true if the bit is set (1), false otherwise (0)
     */
    bool get(size_t position) const override;

    /**
     * @brief Overload of [] operator to get bit value
     *
     * @param position The index of the bit to retrieve
     * @return true if the bit is set (1), false otherwise (0)
     */
    bool operator[](size_t position) const override { return get(position); }

    /**
     * @brief Set the bit at specified position to 0
     *
     * @param position The index of the bit to reset
     * @throw std::out_of_range If the position's block doesn't exist
     */
    void reset(size_t position) override;

    /**
     * @brief Reset all bits to 0
     */
    void reset() override;

    /**
     * @brief Get the number of bits set to 1
     *
     * @return size_t The count of set bits
     */
    size_t count() const override;

    /**
     * @brief Get the total number of bits across all blocks
     *
     * @return size_t The total capacity of the bitmap in bits
     */
    size_t size() const override;

    /**
     * @brief Convert the roaring bitmap to a string representation
     *
     * Format: {"block_index":"hex_data", "block_index":"hex_data", ...}
     * @return std::string String representation of the roaring bitmap
     */
    std::string to_string() const;

    /**
     * @brief Initialize the roaring bitmap from a string representation
     *
     * @param text The string to parse and load
     * @throw std::runtime_error If the input string has invalid format
     */
    void from_string(const std::string& text);

    /**
     * @brief Get a list of positions where bits are set to 1
     *
     * @return std::vector<size_t> Vector containing indices of set bits
     */
    std::vector<size_t> valuelist() const;

    /**
     * @brief Check if two roaring bitmaps are equal
     *
     * @param other The roaring bitmap to compare with
     * @return true if bitmaps are equal, false otherwise
     */
    bool equals(const ibitmap& other) const override;

    /**
     * @brief Check if two roaring bitmaps are equal
     *
     * @param other The roaring bitmap to compare with
     * @return true if bitmaps are equal, false otherwise
     */
    bool operator==(const roaring_bitmap& other) const { return equals(other); }

    /**
     * @brief Check if two roaring bitmaps are not equal
     *
     * @param other The roaring bitmap to compare with
     * @return true if bitmaps are not equal, false otherwise
     */
    bool operator!=(const roaring_bitmap& other) const { return !equals(other); }

    /**
     * @brief Perform bitwise AND with another roaring bitmap
     *
     * @param other The roaring bitmap to AND with
     * @return roaring_bitmap Result of the AND operation
     * @throw std::invalid_argument If block sizes differ
     */
    roaring_bitmap operator&(const roaring_bitmap& other) const;

    /**
     * @brief Perform bitwise OR with another roaring bitmap
     *
     * @param other The roaring bitmap to OR with
     * @return roaring_bitmap Result of the OR operation
     * @throw std::invalid_argument If block sizes differ
     */
    roaring_bitmap operator|(const roaring_bitmap& other) const;

    /**
     * @brief Perform bitwise NOT operation
     *
     * @return roaring_bitmap Result of the NOT operation
     */
    roaring_bitmap operator~() const;

    /**
     * @brief Perform bitwise XOR with another roaring bitmap
     *
     * @param other The roaring bitmap to XOR with
     * @return roaring_bitmap Result of the XOR operation
     * @throw std::invalid_argument If block sizes differ or blocks are missing
     */
    roaring_bitmap operator^(const roaring_bitmap& other) const;

    /**
     * @brief Perform bitwise AND with another roaring bitmap and assign result
     *
     * @param other The roaring bitmap to AND with
     * @return roaring_bitmap& Reference to this bitmap after operation
     * @throw std::invalid_argument If block sizes differ or blocks are missing
     */
    roaring_bitmap& operator&=(const roaring_bitmap& other);

    /**
     * @brief Perform bitwise OR with another roaring bitmap and assign result
     *
     * @param other The roaring bitmap to OR with
     * @return roaring_bitmap& Reference to this bitmap after operation
     * @throw std::invalid_argument If block sizes differ or blocks are missing
     */
    roaring_bitmap& operator|=(const roaring_bitmap& other);

    /**
     * @brief Perform bitwise XOR with another roaring bitmap and assign result
     *
     * @param other The roaring bitmap to XOR with
     * @return roaring_bitmap& Reference to this bitmap after operation
     * @throw std::invalid_argument If block sizes differ or blocks are missing
     */
    roaring_bitmap& operator^=(const roaring_bitmap& other);
};

} // namespace cutl