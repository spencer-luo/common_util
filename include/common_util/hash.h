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
 * @brief Non-cryptographic hashes, integer / hash-table hashes, and checksums.
 * @author Spencer
 * @date 2025-10-15
 *
 * Declaration order: non-cryptographic hashes, then integer hashes (a special
 * case of non-cryptographic hashing whose input is an integer key), then
 * checksums. This header does not include cryptographic hashes (MD5, SHA, etc.).
 *
 * Byte hashes take const uint8_t* plus a length so callers treat the input as
 * a sequence of octets. std::string overloads are convenience wrappers and
 * omit seed / initval (they always use the default).
 *
 * Throughput below is this library's implementation on x86_64, g++ 13.3 -O2,
 * Intel Core Ultra 7 255H (two runs averaged, then rounded). Byte hashes /
 * checksums: MB/s on fixed 16B and 1KiB inputs. Integer hashes: million
 * calls per second (Mops/s). Numbers vary with compiler, CPU, and cache;
 * use them for relative comparison only.
 *
 * | Function | Category | Performance | Characteristics | Typical use |
 * |----------|----------|-------------|-----------------|-------------|
 * | hash_polynomial_rolling | Non-crypto | 16B 3100 MB/s, 1KiB 1510 MB/s | `h=31*h+b`, same as Java `String.hashCode` (ASCII); very simple, more collisions | Short strings, teaching |
 * | hash_djb2 | Non-crypto | 16B 3090 MB/s, 1KiB 1510 MB/s | Bernstein, `hash*33+b`; very simple, average distribution | General string hashing |
 * | hash_fnv1_32 | Non-crypto | 16B 3040 MB/s, 1KiB 1150 MB/s | FNV-1 32-bit: multiply then XOR; good on short keys | Short strings, simple fingerprints |
 * | hash_fnv1_64 | Non-crypto | 16B 3060 MB/s, 1KiB 1110 MB/s | FNV-1 64-bit: multiply then XOR; larger space, fewer collisions | Short strings, simple fingerprints |
 * | hash_fnv1a_32 | Non-crypto | 16B 2890 MB/s, 1KiB 1140 MB/s | FNV-1a 32-bit: XOR then multiply; better avalanche than FNV-1 | Hash tables, Bloom filters |
 * | hash_fnv1a_64 | Non-crypto | 16B 3040 MB/s, 1KiB 1130 MB/s | FNV-1a 64-bit: XOR then multiply; fewer 64-bit collisions | Hash tables, Bloom filters, HyperLogLog |
 * | hash_one_at_a_time | Non-crypto | 16B 1600 MB/s, 1KiB 890 MB/s | Jenkins per-byte mix; better quality than DJB2; slower due to per-byte shifts | General string hashing |
 * | hash_lookup3 | Non-crypto | 16B 4220 MB/s, 1KiB 3650 MB/s | Jenkins hashlittle; mixes 12-byte blocks; good avalanche | Hash tables needing stronger mixing |
 * | hash_murmur2_32 | Non-crypto | 16B 6070 MB/s, 1KiB 4680 MB/s | Original MurmurHash2, 4-byte blocks; known weaknesses | Compatibility with old data/protocols |
 * | hash_murmur2_64 | Non-crypto | 16B 8510 MB/s, 1KiB 9710 MB/s | MurmurHash64A, 8-byte blocks; among the fastest on long inputs | 64-bit hash tables, longer inputs |
 * | hash_murmur3_32 | Non-crypto | 16B 5070 MB/s, 1KiB 4040 MB/s | MurmurHash3 x86_32; a de-facto modern non-crypto hash | Hash tables, Bloom filters |
 * | hash_murmur3_64 | Non-crypto | 16B 4820 MB/s, 1KiB 9770 MB/s | Low 64 bits of MurmurHash3_x64_128; close to murmur2_64 on long inputs | General 64-bit hashing |
 * | hash_thomas_wang | Integer hash | 1340 Mops/s | 32-bit integer avalanche mix (hash32shift); input is not a byte sequence | Mixing integer keys before table insert |
 * | hash_multiplication | Integer hash | 2170 Mops/s | Knuth multiplicative hash; output is a slot in `[0, table_size)` | Hash-table index |
 * | hash_division | Integer hash | 760 Mops/s | `key % table_size`; table size should be prime; division is relatively slow | Hash-table index |
 * | hash_crc16 | Checksum | 16B 150 MB/s, 1KiB 140 MB/s | CRC-16/CCITT-FALSE (bit-by-bit in this implementation, hence slowest) | Serial / embedded error detection |
 * | hash_crc32 | Checksum | 16B 1440 MB/s, 1KiB 640 MB/s | CRC-32/ISO-HDLC (ZIP/PNG/Ethernet); byte-wise table lookup | File / frame integrity |
 * | hash_crc32c | Checksum | 16B 1420 MB/s, 1KiB 640 MB/s | CRC-32C Castagnoli; stronger detection than CRC-32; also table-based here | iSCSI, ext4 |
 * | hash_adler32 | Checksum | 16B 3740 MB/s, 1KiB 3900 MB/s | zlib Adler-32; faster than CRC, weaker error detection | Compression-stream checksums |
 */
#pragma once

#include <cstddef>
#include <cstdint>
#include <string>

namespace cutl
{

// ---------------------------------------------------------------------------
// Non-cryptographic hashes (byte sequence -> hash; hash tables / Bloom filters)
// ---------------------------------------------------------------------------

/**
 * @brief Polynomial rolling hash function
 *
 * A simple hash function that processes each byte in sequence,
 * using a rolling polynomial calculation (`h = 31 * h + byte`).
 *
 * @param data Pointer to the bytes to hash; may be nullptr if len is 0
 * @param len Length of the data in bytes
 * @return uint32_t The resulting hash value
 */
uint32_t hash_polynomial_rolling(const uint8_t* data, size_t len);

/** @brief Convenience overload. Hashes the string's underlying bytes. */
inline uint32_t hash_polynomial_rolling(const std::string& str)
{
    return hash_polynomial_rolling(reinterpret_cast<const uint8_t*>(str.data()), str.size());
}

/**
 * @brief DJB2 hash algorithm
 *
 * A popular non-cryptographic hash function created by Daniel J. Bernstein.
 * Known for its simplicity and good distribution properties.
 *
 * @param data Pointer to the bytes to hash; may be nullptr if len is 0
 * @param len Length of the data in bytes
 * @return uint32_t The resulting hash value
 */
uint32_t hash_djb2(const uint8_t* data, size_t len);

/** @brief Convenience overload. Hashes the string's underlying bytes. */
inline uint32_t hash_djb2(const std::string& str)
{
    return hash_djb2(reinterpret_cast<const uint8_t*>(str.data()), str.size());
}

/**
 * @brief FNV-1 hash function (32-bit version)
 *
 * Fowler-Noll-Vo hash function, version 1. A non-cryptographic hash function
 * with good avalanche properties and distribution.
 *
 * @param data Pointer to the bytes to hash; may be nullptr if len is 0
 * @param len Length of the data in bytes
 * @return uint32_t The resulting hash value
 */
uint32_t hash_fnv1_32(const uint8_t* data, size_t len);

/** @brief Convenience overload. Hashes the string's underlying bytes. */
inline uint32_t hash_fnv1_32(const std::string& str)
{
    return hash_fnv1_32(reinterpret_cast<const uint8_t*>(str.data()), str.size());
}

/**
 * @brief FNV-1 hash function (64-bit version)
 *
 * 64-bit Fowler-Noll-Vo hash, version 1 (multiply then XOR).
 *
 * @param data Pointer to the bytes to hash; may be nullptr if len is 0
 * @param len Length of the data in bytes
 * @return uint64_t The resulting hash value
 */
uint64_t hash_fnv1_64(const uint8_t* data, size_t len);

/** @brief Convenience overload. Hashes the string's underlying bytes. */
inline uint64_t hash_fnv1_64(const std::string& str)
{
    return hash_fnv1_64(reinterpret_cast<const uint8_t*>(str.data()), str.size());
}

/**
 * @brief FNV-1a hash function (32-bit version, recommended)
 *
 * Improved version of FNV-1 with better avalanche properties by changing
 * the order of XOR and multiplication operations.
 *
 * @param data Pointer to the bytes to hash; may be nullptr if len is 0
 * @param len Length of the data in bytes
 * @return uint32_t The resulting hash value
 */
uint32_t hash_fnv1a_32(const uint8_t* data, size_t len);

/** @brief Convenience overload. Hashes the string's underlying bytes. */
inline uint32_t hash_fnv1a_32(const std::string& str)
{
    return hash_fnv1a_32(reinterpret_cast<const uint8_t*>(str.data()), str.size());
}

/**
 * @brief FNV-1a hash function (64-bit version)
 *
 * 64-bit implementation of the improved FNV-1a hash function, providing
 * a larger hash space for reduced collision probability.
 *
 * @param data Pointer to the bytes to hash; may be nullptr if len is 0
 * @param len Length of the data in bytes
 * @return uint64_t The resulting hash value
 */
uint64_t hash_fnv1a_64(const uint8_t* data, size_t len);

/** @brief Convenience overload. Hashes the string's underlying bytes. */
inline uint64_t hash_fnv1a_64(const std::string& str)
{
    return hash_fnv1a_64(reinterpret_cast<const uint8_t*>(str.data()), str.size());
}

/**
 * @brief Jenkins one_at_a_time hash function
 *
 * A simple but high-quality hash function designed by Bob Jenkins.
 * Processes each byte individually and produces good distribution.
 *
 * @param data Pointer to the bytes to hash; may be nullptr if len is 0
 * @param len Length of the data in bytes
 * @return uint32_t The resulting hash value
 */
uint32_t hash_one_at_a_time(const uint8_t* data, size_t len);

/** @brief Convenience overload. Hashes the string's underlying bytes. */
inline uint32_t hash_one_at_a_time(const std::string& str)
{
    return hash_one_at_a_time(reinterpret_cast<const uint8_t*>(str.data()), str.size());
}

/**
 * @brief Jenkins lookup3 hash function
 *
 * A more complex hash function by Bob Jenkins, offering high performance
 * and excellent avalanche properties for general-purpose hashing.
 *
 * @param data Pointer to the bytes to hash; may be nullptr if len is 0
 * @param len Length of the data in bytes
 * @param initval Initial value for the hash calculation
 * @return uint32_t The resulting hash value
 */
uint32_t hash_lookup3(const uint8_t* data, size_t len, uint32_t initval = 0);

/** @brief Convenience overload. Hashes the string's underlying bytes with initval 0. */
inline uint32_t hash_lookup3(const std::string& str)
{
    return hash_lookup3(reinterpret_cast<const uint8_t*>(str.data()), str.size());
}

/**
 * @brief MurmurHash2 32-bit hash
 *
 * Original 32-bit MurmurHash2 by Austin Appleby. Pair of hash_murmur2_64
 * (MurmurHash64A).
 *
 * @param data Pointer to the bytes to hash; may be nullptr if len is 0
 * @param len Length of the data in bytes
 * @param seed Initial seed value for hash calculation
 * @return uint32_t The resulting hash value
 */
uint32_t hash_murmur2_32(const uint8_t* data, size_t len, uint32_t seed = 0);

/** @brief Convenience overload. Hashes the string's underlying bytes with seed 0. */
inline uint32_t hash_murmur2_32(const std::string& str)
{
    return hash_murmur2_32(reinterpret_cast<const uint8_t*>(str.data()), str.size());
}

/**
 * @brief MurmurHash2 64-bit hash (MurmurHash64A)
 *
 * 64-bit implementation of MurmurHash2 for 64-bit platforms. This is not
 * MurmurHash3; MurmurHash3's 64-bit-oriented variant produces 128 bits.
 *
 * @param data Pointer to the bytes to hash; may be nullptr if len is 0
 * @param len Length of the data in bytes
 * @param seed Initial seed value for hash calculation
 * @return uint64_t The resulting hash value
 */
uint64_t hash_murmur2_64(const uint8_t* data, size_t len, uint64_t seed = 0);

/** @brief Convenience overload. Hashes the string's underlying bytes with seed 0. */
inline uint64_t hash_murmur2_64(const std::string& str)
{
    return hash_murmur2_64(reinterpret_cast<const uint8_t*>(str.data()), str.size());
}

/**
 * @brief MurmurHash3 hash function (x86 32-bit version)
 *
 * A fast non-cryptographic hash function created by Austin Appleby.
 * Known for its excellent performance and distribution characteristics.
 *
 * @param data Pointer to the bytes to hash; may be nullptr if len is 0
 * @param len Length of the data in bytes
 * @param seed Initial seed value for hash calculation
 * @return uint32_t The resulting hash value
 */
uint32_t hash_murmur3_32(const uint8_t* data, size_t len, uint32_t seed = 0);

/** @brief Convenience overload. Hashes the string's underlying bytes with seed 0. */
inline uint32_t hash_murmur3_32(const std::string& str)
{
    return hash_murmur3_32(reinterpret_cast<const uint8_t*>(str.data()), str.size());
}

/**
 * @brief MurmurHash3 64-bit hash
 *
 * MurmurHash3 has no official 64-bit output; the x64 variant produces 128 bits.
 * This returns the first 64 bits of MurmurHash3_x64_128, which is the usual
 * way to use MurmurHash3 as a 64-bit hash.
 *
 * @param data Pointer to the bytes to hash; may be nullptr if len is 0
 * @param len Length of the data in bytes
 * @param seed Initial seed value for hash calculation
 * @return uint64_t The resulting hash value
 */
uint64_t hash_murmur3_64(const uint8_t* data, size_t len, uint32_t seed = 0);

/** @brief Convenience overload. Hashes the string's underlying bytes with seed 0. */
inline uint64_t hash_murmur3_64(const std::string& str)
{
    return hash_murmur3_64(reinterpret_cast<const uint8_t*>(str.data()), str.size());
}

// ---------------------------------------------------------------------------
// Integer hashes (integer key -> mixed value or table slot; not a byte hash)
// ---------------------------------------------------------------------------

/**
 * @brief Thomas Wang's integer hash function
 *
 * Non-cryptographic mix for a 32-bit integer key (hash32shift). Spreads bits
 * so nearby integers map to distant hash values. Used before inserting an
 * integer into a hash table; not a checksum and not for hashing byte sequences.
 *
 * @param key The 32-bit integer to hash
 * @return uint32_t The resulting hash value
 */
uint32_t hash_thomas_wang(uint32_t key);

/**
 * @brief Knuth multiplication hash function
 *
 * Non-cryptographic hash-table mapping: h(k) = floor(m * frac(k * A)), where
 * A is the golden-ratio conjugate (√5-1)/2. Turns an integer key into a slot
 * index in [0, table_size). Not a checksum.
 *
 * @param key The integer key to hash
 * @param table_size The size of the hash table; 0 returns 0
 * @return uint32_t The resulting hash value (table index)
 */
uint32_t hash_multiplication(uint32_t key, uint32_t table_size);

/**
 * @brief Division hash function
 *
 * Non-cryptographic hash-table mapping: h(k) = k mod m. table_size should
 * preferably be prime. Turns an integer key into a slot index; not a checksum.
 *
 * @param key The integer key to hash
 * @param table_size The size of the hash table; 0 returns 0
 * @return uint32_t The resulting hash value (table index)
 */
uint32_t hash_division(uint32_t key, uint32_t table_size);

// ---------------------------------------------------------------------------
// Checksums (byte sequence -> error-detecting code; not a hash-table hash)
// ---------------------------------------------------------------------------

/**
 * @brief CRC-16/CCITT-FALSE checksum
 *
 * Polynomial 0x1021, init 0xFFFF, not reflected, xorout 0x0000.
 * Common in embedded and telecom protocols. Check value for "123456789" is 0x29B1.
 *
 * @param data Pointer to the bytes to hash; may be nullptr if len is 0
 * @param len Length of the data in bytes
 * @return uint16_t The resulting checksum
 */
uint16_t hash_crc16(const uint8_t* data, size_t len);

/** @brief Convenience overload. Hashes the string's underlying bytes. */
inline uint16_t hash_crc16(const std::string& str)
{
    return hash_crc16(reinterpret_cast<const uint8_t*>(str.data()), str.size());
}

/**
 * @brief CRC-32/ISO-HDLC checksum (IEEE 802.3, ZIP, PNG, Ethernet)
 *
 * Polynomial 0x04C11DB7 (reflected 0xEDB88320), init/xorout 0xFFFFFFFF.
 * Check value for "123456789" is 0xCBF43926.
 *
 * @param data Pointer to the bytes to hash; may be nullptr if len is 0
 * @param len Length of the data in bytes
 * @return uint32_t The resulting checksum
 */
uint32_t hash_crc32(const uint8_t* data, size_t len);

/** @brief Convenience overload. Hashes the string's underlying bytes. */
inline uint32_t hash_crc32(const std::string& str)
{
    return hash_crc32(reinterpret_cast<const uint8_t*>(str.data()), str.size());
}

/**
 * @brief CRC-32C checksum (Castagnoli)
 *
 * Polynomial 0x1EDC6F41 (reflected 0x82F63B78). Used by iSCSI, ext4, and SSE4.2.
 * Check value for "123456789" is 0xE3069283.
 *
 * @param data Pointer to the bytes to hash; may be nullptr if len is 0
 * @param len Length of the data in bytes
 * @return uint32_t The resulting checksum
 */
uint32_t hash_crc32c(const uint8_t* data, size_t len);

/** @brief Convenience overload. Hashes the string's underlying bytes. */
inline uint32_t hash_crc32c(const std::string& str)
{
    return hash_crc32c(reinterpret_cast<const uint8_t*>(str.data()), str.size());
}

/**
 * @brief Adler-32 checksum (RFC 1950 / zlib)
 *
 * Two 16-bit sums modulo 65521. Faster than CRC-32, weaker error detection.
 * Check value for "123456789" is 0x091E01DE; empty input is 1.
 *
 * @param data Pointer to the bytes to hash; may be nullptr if len is 0
 * @param len Length of the data in bytes
 * @return uint32_t The resulting checksum
 */
uint32_t hash_adler32(const uint8_t* data, size_t len);

/** @brief Convenience overload. Hashes the string's underlying bytes. */
inline uint32_t hash_adler32(const std::string& str)
{
    return hash_adler32(reinterpret_cast<const uint8_t*>(str.data()), str.size());
}

} // namespace cutl
