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
 * @file hyperloglog.h
 * @brief HyperLogLog algorithm implementation for approximate cardinality counting
 * @author Spencer
 * @date 2025-10-12
 */

#include "hash.h"
#include <cmath>
#include <cstdint>
#include <string>
#include <vector>

namespace cutl
{

/**
 * @brief HyperLogLog algorithm implementation
 *
 * This class provides an efficient implementation of the HyperLogLog algorithm,
 * which is used for approximating the number of distinct elements (cardinality)
 * in a multiset with high accuracy and low memory usage.
 */
class HyperLogLog
{
public:
    /**
     * @brief Construct a new HyperLogLog object
     *
     * @param precision The precision parameter (p), which determines the number of registers
     *                  and the accuracy. Typical values range from 4 to 16. Defaults to 14.
     */
    explicit HyperLogLog(int precision = 14);

    /**
     * @brief Destroy the HyperLogLog object
     */
    ~HyperLogLog() { registers_.clear(); }

public:
    /**
     * @brief Add a 64-bit integer element to the HyperLogLog structure
     *
     * @param value The 64-bit integer to add
     */
    void add(uint64_t value);

    /**
     * @brief Add a string element to the HyperLogLog structure
     *
     * @param value The string to add
     */
    void add(const std::string& value);

    /**
     * @brief Estimate the cardinality of the set
     *
     * @return uint64_t The approximate number of distinct elements
     */
    uint64_t count() const;

    /**
     * @brief Reset all registers to their initial state
     */
    void reset() { std::fill(registers_.begin(), registers_.end(), 0); }

    /**
     * @brief Merge another HyperLogLog structure into this one
     *
     * @param other The HyperLogLog instance to merge with
     */
    void merge(const HyperLogLog& other);

    /**
     * @brief Convert the HyperLogLog structure to a string representation
     *
     * @return std::string Compressed hex string representation
     */
    std::string to_string() const { return this->to_hex(1); }

    /**
     * @brief Initialize the HyperLogLog structure from a string representation
     *
     * @param text The string to parse and load
     */
    void from_string(const std::string& text);

    /**
     * @brief Get the theoretical error margin of the cardinality estimate
     *
     * @return double The theoretical error (approximately 1.04 / sqrt(m))
     */
    double get_theoretical_error() const { return 1.04 / std::sqrt(m_); }

private:
    /**
     * @brief Compute the alpha constant used in cardinality estimation
     *
     * The alpha constant depends on the number of registers (m).
     * @return double The computed alpha value
     */
    double compute_alpha() const;

    /**
     * @brief Count the number of leading zeros in a 64-bit integer
     *
     * @param bits The 64-bit integer to analyze
     * @return int The number of leading zeros
     */
    int countLeading_zeros(uint64_t bits) const;

    /**
     * @brief Convert the HyperLogLog structure to a hexadecimal string
     *
     * @param compress 0 for uncompressed, 1 for compressed (truncates trailing zeros)
     * @return std::string Hexadecimal string representation
     */
    std::string to_hex(int compress = 1) const;

private:
    int p_; ///< Precision parameter, determines the number of registers (m = 2^p)
    int m_; ///< Number of registers (m = 2^p)
    std::vector<uint8_t> registers_; ///< Array of registers storing maximum leading zeros
    double alpha_;                   ///< Alpha constant used in cardinality estimation formula
};

} // namespace cutl