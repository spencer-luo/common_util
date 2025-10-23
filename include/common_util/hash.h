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
 * @file hash.h
 * @brief Commonly used hash functions, such as DJB2/FNV-1/lookup3
 * @author Spencer
 * @date 2025-10-15
 */
#pragma once

#include <cstdint>
#include <string>

namespace cutl
{

/**
 * @brief Polynomial rolling hash function
 *
 * A simple hash function that processes each character in sequence,
 * using a rolling polynomial calculation to generate a hash value.
 *
 * @param str The input string to hash
 * @return uint32_t The resulting hash value
 */
uint32_t hash_polynomial_rolling(const std::string& str);

/**
 * @brief DJB2 hash algorithm
 *
 * A popular non-cryptographic hash function created by Daniel J. Bernstein.
 * Known for its simplicity and good distribution properties.
 *
 * @param str The input string to hash
 * @return uint32_t The resulting hash value
 */
uint32_t hash_djb2(const std::string& str);

/**
 * @brief FNV-1 hash function (32-bit version)
 *
 * Fowler-Noll-Vo hash function, version 1. A non-cryptographic hash function
 * with good avalanche properties and distribution.
 *
 * @param str The input string to hash
 * @return uint32_t The resulting hash value
 */
uint32_t hash_fnv1_32(const std::string& str);

/**
 * @brief FNV-1a hash function (32-bit version, recommended)
 *
 * Improved version of FNV-1 with better avalanche properties by changing
 * the order of XOR and multiplication operations.
 *
 * @param str The input string to hash
 * @return uint32_t The resulting hash value
 */
uint32_t hash_fnv1a_32(const std::string& str);

/**
 * @brief FNV-1a hash function (64-bit version)
 *
 * 64-bit implementation of the improved FNV-1a hash function, providing
 * a larger hash space for reduced collision probability.
 *
 * @param str The input string to hash
 * @return uint64_t The resulting hash value
 */
uint64_t hash_fnv1a_64(const std::string& str);

/**
 * @brief Jenkins one_at_a_time hash function
 *
 * A simple but high-quality hash function designed by Bob Jenkins.
 * Processes each byte individually and produces good distribution.
 *
 * @param str The input string to hash
 * @return uint32_t The resulting hash value
 */
uint32_t hash_one_at_a_time(const std::string& str);

/**
 * @brief Jenkins lookup3 hash function
 *
 * A more complex hash function by Bob Jenkins, offering high performance
 * and excellent avalanche properties for general-purpose hashing.
 *
 * @param key Pointer to the data to hash
 * @param length Length of the data in bytes
 * @param initval Initial value for the hash calculation
 * @return uint32_t The resulting hash value
 */
uint32_t hash_lookup3(const void* key, size_t length, uint32_t initval = 0);

/**
 * @brief MurmurHash3 hash function (32-bit version)
 *
 * A fast non-cryptographic hash function created by Austin Appleby.
 * Known for its excellent performance and distribution characteristics.
 *
 * @param key Pointer to the data to hash
 * @param len Length of the data in bytes
 * @param seed Initial seed value for hash calculation
 * @return uint32_t The resulting hash value
 */
uint32_t hash_murmur3_32(const void* key, size_t len, uint32_t seed = 0);

/**
 * @brief MurmurHash3 hash function (64-bit version)
 *
 * 64-bit implementation of MurmurHash3, providing a larger hash space
 * for applications requiring reduced collision probability.
 *
 * @param key Pointer to the data to hash
 * @param len Length of the data in bytes
 * @param seed Initial seed value for hash calculation
 * @return uint64_t The resulting hash value
 */
uint64_t hash_murmur3_64(const void* key, size_t len, uint64_t seed);

/**
 * @brief MurmurHash3 hash function (64-bit version for strings)
 *
 * Convenience overload of the 64-bit MurmurHash3 function for string inputs.
 *
 * @param str The input string to hash
 * @param seed Initial seed value for hash calculation
 * @return uint64_t The resulting hash value
 */
uint64_t hash_murmur3_64(const std::string& str, uint64_t seed = 0);

/**
 * @brief Thomas Wang's integer hash function
 *
 * A hash function designed specifically for 32-bit integers, providing
 * good avalanche properties and minimal collisions.
 *
 * @param key The 32-bit integer to hash
 * @return uint32_t The resulting hash value
 */
uint32_t hash_thomas_wang(uint32_t key);

/**
 * @brief Multiplication hash function
 *
 * A hash function suitable for hash tables that uses multiplication by a
 * large prime number to distribute keys across the table.
 *
 * @param key The integer key to hash
 * @param table_size The size of the hash table
 * @return uint32_t The resulting hash value (table index)
 */
uint32_t hash_multiplication(uint32_t key, uint32_t table_size);

/**
 * @brief Division hash function
 *
 * A simple hash function for hash tables that uses modulo operation to
 * map keys to table indices.
 *
 * @param key The integer key to hash
 * @param table_size The size of the hash table
 * @return uint32_t The resulting hash value (table index)
 */
uint32_t hash_division(uint32_t key, uint32_t table_size);

} // namespace cutl