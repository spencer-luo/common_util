#pragma once

#include <cstdint>
#include <string>

namespace cutl
{

// 多项式滚动哈希: Polynomial rolling_hash
uint32_t hash_polynomial_rolling(const std::string& str);

// DJB2哈希算法
uint32_t hash_djb2(const std::string& str);

// FNV-1 32位版本
uint32_t hash_fnv1_32(const std::string& str);

// FNV-1a 32位版本（推荐使用）
uint32_t hash_fnv1a_32(const std::string& str);

// FNV-1a 64位版本
uint64_t hash_fnv1a_64(const std::string& str);

// Jenkins one_at_a_time 哈希（简单但高质量）
uint32_t hash_one_at_a_time(const std::string& str);

// Jenkins lookup3 哈希（更复杂的Jenkins哈希）
uint32_t hash_lookup3(const void* key, size_t length, uint32_t initval = 0);

// MurmurHash3 32位版本
uint32_t hash_murmur3_32(const void* key, size_t len, uint32_t seed = 0);

uint64_t hash_murmur3_64(const void* key, size_t len, uint64_t seed);

uint64_t hash_murmur3_64(const std::string& str, uint64_t seed = 0);

// Thomas Wang的整数哈希函数
uint32_t hash_thomas_wang(uint32_t key);

// 乘法哈希（适用于哈希表）
uint32_t hash_multiplication(uint32_t key, uint32_t table_size);

// 除法哈希
uint32_t hash_division(uint32_t key, uint32_t table_size);

} // namespace cutl