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
 * @file bloomfilter.h
 * @brief bloom_filter algorithm.
 * @author Spencer
 * @date 2025-10-16
 */

#pragma once

#include "bitmap.h"
#include <functional>
#include <memory>
#include <random>
#include <string>
#include <vector>

namespace cutl
{

/**
 * @brief False positive rate enumeration for bloom filter
 *
 * This enum defines possible false positive rates for the bloom filter
 * as percentage values.
 */
enum bloom_error_rate
{
    percent_01 = 1,  // <= 1% false positive rate
    percent_10 = 10, // <= 10% false positive rate
    percent_20 = 20, // <= 20% false positive rate
    percent_30 = 30, // <= 30% false positive rate
    percent_40 = 40, // <= 40% false positive rate
    percent_50 = 50, // <= 50% false positive rate
};

/**
 * @brief Bloom filter implementation
 *
 * A bloom filter is a space-efficient probabilistic data structure that is used
 * to test whether an element is a member of a set. It may return false positives
 * but never false negatives.
 */
class bloom_filter
{
public:
    /**
     * @brief Construct a new bloom filter object with specified sizes
     *
     * @param size Total number of bits in the bitmap
     * @param hash_size Number of hash functions to use
     */
    bloom_filter(size_t size, size_t hash_size);

    /**
     * @brief Construct a new bloom filter object with automatic parameter calculation
     *
     * @param expected_size Estimated number of elements to be inserted
     * @param error_rate Desired false positive rate (default: 1%)
     */
    bloom_filter(size_t expected_size, bloom_error_rate error_rate = bloom_error_rate::percent_01);

    /**
     * @brief Destroy the bloom filter object
     */
    ~bloom_filter() = default;

public:
    /**
     * @brief Add an element to the bloom filter
     *
     * @param value The string element to add
     */
    void add(const std::string& value);

    /**
     * @brief Check if an element may be in the bloom filter
     *
     * Note: This may return true for elements not actually inserted (false positive)
     * but will never return false for elements that were inserted.
     *
     * @param value The string element to check
     * @return true If the element is probably in the set
     * @return false If the element is definitely not in the set
     */
    bool contains(const std::string& value) const;

    /**
     * @brief Clear all elements from the bloom filter
     *
     * Resets all bits in the underlying bitmap to 0.
     */
    void clear() { bitmap_->reset(); }

    // /**
    //  * Get the number of set bits in the bitmap (test function, not recommended for production)
    //  */
    // size_t test_getSetBitCount() const;

    // // Estimate the number of elements (test function, not recommended for production)
    // size_t test_estimateCount() const;

    // // Get the relative error (test function, not recommended for production)
    // double test_getRelativeError();

private:
    /**
     * @brief Double hashing function to generate k hash values
     *
     * Implements a double hashing technique to generate multiple hash values
     * using two base hash functions.
     *
     * @param str The string to hash
     * @return std::pair<size_t, size_t> Two base hash values used to generate k hashes
     */
    std::pair<size_t, size_t> hash(const std::string& str) const;

private:
    size_t size_;                    ///< Total number of bits in the bitmap
    std::shared_ptr<bitmap> bitmap_; ///< Underlying bitmap storage
    size_t hash_size_;               ///< Number of hash functions to use
};

/**
 * @brief Calculate the optimal number of hash functions
 *
 * @param m Size of the bit array (in bits)
 * @param n Expected number of inserted elements
 * @return size_t Optimal number of hash functions
 */
size_t bloom_optimal_k(size_t m, size_t n);

/**
 * @brief Calculate the optimal size of the bit array
 *
 * @param n Expected number of inserted elements
 * @param p Desired false positive rate (0 < p < 1)
 * @return size_t Optimal bit array size in bits
 */
size_t bloom_optimal_m(size_t n, double p);

/**
 * @brief Calculate the actual false positive rate
 *
 * @param m Size of the bit array (in bits)
 * @param n Expected number of inserted elements
 * @param k Number of hash functions
 * @return double Actual false positive rate
 */
double bloom_false_positive_rate(size_t m, size_t n, size_t k);

/**
 * @brief Comprehensive calculation of all optimal parameters
 *
 * @param n Expected number of elements
 * @param target_fp_rate Target false positive rate
 * @param optimal_m [out] Calculated optimal bit array size
 * @param optimal_k [out] Calculated optimal number of hash functions
 * @param actual_fp_rate [out] Actual false positive rate with these parameters
 */
void bloom_optimal_parameters(size_t n,
                              double target_fp_rate,
                              size_t& optimal_m,
                              size_t& optimal_k,
                              double& actual_fp_rate);

} // namespace cutl