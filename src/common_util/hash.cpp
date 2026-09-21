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
 * @file hash.cpp
 * @brief Non-cryptographic hashes, integer / hash-table hashes, and checksums.
 * @author Spencer
 * @date 2025-10-15
 *
 * 实现顺序：非加密哈希 → 整数哈希（也是非加密哈希的一种，输入为整数键）→ 校验和。
 * 本文件不含密码学哈希（MD5 / SHA 等）。
 *
 * 性能为本库实现在 x86_64、g++ 13.3 -O2、Intel Core Ultra 7 255H 上的实测
 * （跑两次取平均后四舍五入）。字节哈希 / 校验和：固定输入 16B 与 1KiB 的吞吐（MB/s）；
 * 整数哈希：每秒百万次调用（Mops/s）。数值随编译器、CPU、缓存而变，仅供横向比较。
 *
 * | 函数 | 分类 | 性能 | 特点 | 应用场景 |
 * |------|------|------|------|----------|
 * | hash_polynomial_rolling | 非加密哈希 | 16B 3100 MB/s，1KiB 1510 MB/s | `h=31*h+b`，等同 Java `String.hashCode`（ASCII）；实现极简，碰撞偏多 | 短字符串、教学 |
 * | hash_djb2 | 非加密哈希 | 16B 3090 MB/s，1KiB 1510 MB/s | Bernstein，`hash*33+b`；实现极简，分布一般 | 一般字符串哈希 |
 * | hash_fnv1_32 | 非加密哈希 | 16B 3040 MB/s，1KiB 1150 MB/s | FNV-1 32 位：先乘后 XOR；短键表现好 | 短字符串、简单指纹 |
 * | hash_fnv1_64 | 非加密哈希 | 16B 3060 MB/s，1KiB 1110 MB/s | FNV-1 64 位：先乘后 XOR；空间更大、碰撞更少 | 短字符串、简单指纹 |
 * | hash_fnv1a_32 | 非加密哈希 | 16B 2890 MB/s，1KiB 1140 MB/s | FNV-1a 32 位：先 XOR 后乘，雪崩优于 FNV-1 | 哈希表、Bloom Filter |
 * | hash_fnv1a_64 | 非加密哈希 | 16B 3040 MB/s，1KiB 1130 MB/s | FNV-1a 64 位：先 XOR 后乘；64 位碰撞更少 | 哈希表、Bloom Filter、HyperLogLog |
 * | hash_one_at_a_time | 非加密哈希 | 16B 1600 MB/s，1KiB 890 MB/s | Jenkins 逐字节混合，质量好于 DJB2；逐字节移位偏慢 | 通用字符串哈希 |
 * | hash_lookup3 | 非加密哈希 | 16B 4220 MB/s，1KiB 3650 MB/s | Jenkins hashlittle，12 字节一块混合，雪崩好 | 哈希表、对分布要求较高的场景 |
 * | hash_murmur2_32 | 非加密哈希 | 16B 6070 MB/s，1KiB 4680 MB/s | 原版 MurmurHash2，按 4 字节块处理；有已知弱点 | 兼容旧数据/协议 |
 * | hash_murmur2_64 | 非加密哈希 | 16B 8510 MB/s，1KiB 9710 MB/s | MurmurHash64A，按 8 字节块处理；长输入最快之一 | 64 位哈希表、较长输入 |
 * | hash_murmur3_32 | 非加密哈希 | 16B 5070 MB/s，1KiB 4040 MB/s | MurmurHash3 x86_32，现代非加密哈希事实标准之一 | 哈希表、Bloom Filter |
 * | hash_murmur3_64 | 非加密哈希 | 16B 4820 MB/s，1KiB 9770 MB/s | MurmurHash3_x64_128 的低 64 位；长输入接近 murmur2_64 | 64 位通用哈希 |
 * | hash_thomas_wang | 整数哈希 | 1340 Mops/s | 32 位整数雪崩混合（hash32shift），输入不是字节序列 | 整数键打散后再入表 |
 * | hash_multiplication | 整数哈希 | 2170 Mops/s | Knuth 乘法，输出 `[0, table_size)` 槽位 | 哈希表下标 |
 * | hash_division | 整数哈希 | 760 Mops/s | `key % table_size`，表长宜为质数；除法相对较慢 | 哈希表下标 |
 * | hash_crc16 | 校验和 | 16B 150 MB/s，1KiB 140 MB/s | CRC-16/CCITT-FALSE（本实现按位计算，故最慢） | 串口、嵌入式检错 |
 * | hash_crc32 | 校验和 | 16B 1440 MB/s，1KiB 640 MB/s | CRC-32/ISO-HDLC（ZIP/PNG/以太网），字节查表 | 文件/帧完整性 |
 * | hash_crc32c | 校验和 | 16B 1420 MB/s，1KiB 640 MB/s | CRC-32C Castagnoli，检错强于 CRC-32；本实现同样查表 | iSCSI、ext4 |
 * | hash_adler32 | 校验和 | 16B 3740 MB/s，1KiB 3900 MB/s | zlib Adler-32，比 CRC 快、检错更弱 | 压缩流校验 |
 */

#include "hash.h"

namespace cutl
{
namespace
{

uint32_t rotl32(uint32_t x, int r)
{
    return (x << r) | (x >> (32 - r));
}

uint64_t rotl64(uint64_t x, int r)
{
    return (x << r) | (x >> (64 - r));
}

uint32_t load32_le(const uint8_t* p)
{
    return static_cast<uint32_t>(p[0]) | (static_cast<uint32_t>(p[1]) << 8) |
           (static_cast<uint32_t>(p[2]) << 16) | (static_cast<uint32_t>(p[3]) << 24);
}

uint64_t load64_le(const uint8_t* p)
{
    return static_cast<uint64_t>(p[0]) | (static_cast<uint64_t>(p[1]) << 8) |
           (static_cast<uint64_t>(p[2]) << 16) | (static_cast<uint64_t>(p[3]) << 24) |
           (static_cast<uint64_t>(p[4]) << 32) | (static_cast<uint64_t>(p[5]) << 40) |
           (static_cast<uint64_t>(p[6]) << 48) | (static_cast<uint64_t>(p[7]) << 56);
}

void lookup3_mix(uint32_t& a, uint32_t& b, uint32_t& c)
{
    a -= c;
    a ^= rotl32(c, 4);
    c += b;
    b -= a;
    b ^= rotl32(a, 6);
    a += c;
    c -= b;
    c ^= rotl32(b, 8);
    b += a;
    a -= c;
    a ^= rotl32(c, 16);
    c += b;
    b -= a;
    b ^= rotl32(a, 19);
    a += c;
    c -= b;
    c ^= rotl32(b, 4);
    b += a;
}

void lookup3_final(uint32_t& a, uint32_t& b, uint32_t& c)
{
    c ^= b;
    c -= rotl32(b, 14);
    a ^= c;
    a -= rotl32(c, 11);
    b ^= a;
    b -= rotl32(a, 25);
    c ^= b;
    c -= rotl32(b, 16);
    a ^= c;
    a -= rotl32(c, 4);
    b ^= a;
    b -= rotl32(a, 14);
    c ^= b;
    c -= rotl32(b, 24);
}

constexpr uint32_t FNV_OFFSET_BASIS = 2166136261u;
constexpr uint32_t FNV_PRIME = 16777619u;
constexpr uint64_t FNV_OFFSET_BASIS_64 = 14695981039346656037ull;
constexpr uint64_t FNV_PRIME_64 = 1099511628211ull;

struct crc32_table
{
    uint32_t v[256];

    explicit crc32_table(uint32_t poly)
    {
        for (uint32_t i = 0; i < 256; ++i)
        {
            uint32_t c = i;
            for (int j = 0; j < 8; ++j)
            {
                c = (c & 1) ? (poly ^ (c >> 1)) : (c >> 1);
            }
            v[i] = c;
        }
    }
};

uint32_t crc32_generic(const uint8_t* data, size_t len, uint32_t poly)
{
    static const crc32_table ieee_table(0xEDB88320u);
    static const crc32_table castagnoli_table(0x82F63B78u);
    const uint32_t* table = (poly == 0xEDB88320u) ? ieee_table.v : castagnoli_table.v;

    uint32_t crc = 0xFFFFFFFFu;
    for (size_t i = 0; i < len; ++i)
    {
        crc = table[(crc ^ data[i]) & 0xFFu] ^ (crc >> 8);
    }
    return crc ^ 0xFFFFFFFFu;
}

} // namespace

// ---------------------------------------------------------------------------
// 非加密哈希（字节序列）
// ---------------------------------------------------------------------------

uint32_t hash_polynomial_rolling(const uint8_t* data, size_t len)
{
    uint32_t h = 0;
    for (size_t i = 0; i < len; ++i)
    {
        h = 31 * h + data[i];
    }
    return h;
}

uint32_t hash_djb2(const uint8_t* data, size_t len)
{
    // 5381: 经过大量测试选择的质数
    uint32_t djb2 = 5381;
    for (size_t i = 0; i < len; ++i)
    {
        djb2 = ((djb2 << 5) + djb2) + data[i]; // DJB2 * 33 + c
    }
    return djb2;
}

uint32_t hash_fnv1_32(const uint8_t* data, size_t len)
{
    uint32_t hash = FNV_OFFSET_BASIS;
    for (size_t i = 0; i < len; ++i)
    {
        hash = (hash * FNV_PRIME) ^ data[i];
    }
    return hash;
}

uint64_t hash_fnv1_64(const uint8_t* data, size_t len)
{
    uint64_t hash = FNV_OFFSET_BASIS_64;
    for (size_t i = 0; i < len; ++i)
    {
        hash = (hash * FNV_PRIME_64) ^ data[i];
    }
    return hash;
}

uint32_t hash_fnv1a_32(const uint8_t* data, size_t len)
{
    uint32_t hash = FNV_OFFSET_BASIS;
    for (size_t i = 0; i < len; ++i)
    {
        hash = (hash ^ data[i]) * FNV_PRIME;
    }
    return hash;
}

uint64_t hash_fnv1a_64(const uint8_t* data, size_t len)
{
    uint64_t hash = FNV_OFFSET_BASIS_64;
    for (size_t i = 0; i < len; ++i)
    {
        hash = (hash ^ data[i]) * FNV_PRIME_64;
    }
    return hash;
}

uint32_t hash_one_at_a_time(const uint8_t* data, size_t len)
{
    uint32_t hash = 0;

    for (size_t i = 0; i < len; ++i)
    {
        hash += data[i];
        hash += (hash << 10);
        hash ^= (hash >> 6);
    }

    hash += (hash << 3);
    hash ^= (hash >> 11);
    hash += (hash << 15);

    return hash;
}

uint32_t hash_lookup3(const uint8_t* data, size_t len, uint32_t initval)
{
    uint32_t a, b, c;
    const uint8_t* k = data;
    size_t length = len;

    a = b = c = 0xdeadbeef + static_cast<uint32_t>(length) + initval;

    while (length > 12)
    {
        a += load32_le(k);
        b += load32_le(k + 4);
        c += load32_le(k + 8);
        lookup3_mix(a, b, c);
        k += 12;
        length -= 12;
    }

    switch (length)
    {
        case 12:
            c += (static_cast<uint32_t>(k[11]) << 24);
            // fall through
        case 11:
            c += (static_cast<uint32_t>(k[10]) << 16);
            // fall through
        case 10:
            c += (static_cast<uint32_t>(k[9]) << 8);
            // fall through
        case 9:
            c += k[8];
            // fall through
        case 8:
            b += (static_cast<uint32_t>(k[7]) << 24);
            // fall through
        case 7:
            b += (static_cast<uint32_t>(k[6]) << 16);
            // fall through
        case 6:
            b += (static_cast<uint32_t>(k[5]) << 8);
            // fall through
        case 5:
            b += k[4];
            // fall through
        case 4:
            a += (static_cast<uint32_t>(k[3]) << 24);
            // fall through
        case 3:
            a += (static_cast<uint32_t>(k[2]) << 16);
            // fall through
        case 2:
            a += (static_cast<uint32_t>(k[1]) << 8);
            // fall through
        case 1:
            a += k[0];
            break;
        case 0:
            return c;
    }

    lookup3_final(a, b, c);
    return c;
}

uint32_t hash_murmur2_32(const uint8_t* data, size_t len, uint32_t seed)
{
    const uint32_t m = 0x5bd1e995;
    const int r = 24;

    uint32_t h = seed ^ static_cast<uint32_t>(len);
    const size_t nblocks = len / 4;

    for (size_t i = 0; i < nblocks; ++i)
    {
        uint32_t k = load32_le(data + i * 4);
        k *= m;
        k ^= k >> r;
        k *= m;
        h *= m;
        h ^= k;
    }

    const uint8_t* tail = data + nblocks * 4;
    switch (len & 3)
    {
        case 3:
            h ^= static_cast<uint32_t>(tail[2]) << 16;
            // fall through
        case 2:
            h ^= static_cast<uint32_t>(tail[1]) << 8;
            // fall through
        case 1:
            h ^= tail[0];
            h *= m;
    }

    h ^= h >> 13;
    h *= m;
    h ^= h >> 15;
    return h;
}

uint64_t hash_murmur2_64(const uint8_t* data, size_t len, uint64_t seed)
{
    const uint64_t m = 0xc6a4a7935bd1e995ULL;
    const int r = 47;

    uint64_t h = seed ^ (len * m);
    const size_t nblocks = len / 8;

    for (size_t i = 0; i < nblocks; ++i)
    {
        uint64_t k = load64_le(data + i * 8);

        k *= m;
        k ^= k >> r;
        k *= m;

        h ^= k;
        h *= m;
    }

    const uint8_t* tail = data + nblocks * 8;
    switch (len & 7)
    {
        case 7:
            h ^= static_cast<uint64_t>(tail[6]) << 48;
            // fall through
        case 6:
            h ^= static_cast<uint64_t>(tail[5]) << 40;
            // fall through
        case 5:
            h ^= static_cast<uint64_t>(tail[4]) << 32;
            // fall through
        case 4:
            h ^= static_cast<uint64_t>(tail[3]) << 24;
            // fall through
        case 3:
            h ^= static_cast<uint64_t>(tail[2]) << 16;
            // fall through
        case 2:
            h ^= static_cast<uint64_t>(tail[1]) << 8;
            // fall through
        case 1:
            h ^= static_cast<uint64_t>(tail[0]);
            h *= m;
    }

    h ^= h >> r;
    h *= m;
    h ^= h >> r;

    return h;
}

uint32_t hash_murmur3_32(const uint8_t* data, size_t len, uint32_t seed)
{
    uint32_t h1 = seed;
    const uint32_t c1 = 0xcc9e2d51;
    const uint32_t c2 = 0x1b873593;
    const size_t nblocks = len / 4;

    for (size_t i = 0; i < nblocks; ++i)
    {
        uint32_t k1 = load32_le(data + i * 4);

        k1 *= c1;
        k1 = rotl32(k1, 15);
        k1 *= c2;

        h1 ^= k1;
        h1 = rotl32(h1, 13);
        h1 = h1 * 5 + 0xe6546b64;
    }

    const uint8_t* tail = data + nblocks * 4;
    uint32_t k1 = 0;

    switch (len & 3)
    {
        case 3:
            k1 ^= static_cast<uint32_t>(tail[2]) << 16;
            // fall through
        case 2:
            k1 ^= static_cast<uint32_t>(tail[1]) << 8;
            // fall through
        case 1:
            k1 ^= tail[0];
            k1 *= c1;
            k1 = rotl32(k1, 15);
            k1 *= c2;
            h1 ^= k1;
    }

    h1 ^= static_cast<uint32_t>(len);
    h1 ^= h1 >> 16;
    h1 *= 0x85ebca6b;
    h1 ^= h1 >> 13;
    h1 *= 0xc2b2ae35;
    h1 ^= h1 >> 16;

    return h1;
}

uint64_t hash_murmur3_64(const uint8_t* data, size_t len, uint32_t seed)
{
    const uint64_t c1 = 0x87c37b91114253d5ULL;
    const uint64_t c2 = 0x4cf5ad432745937fULL;

    uint64_t h1 = seed;
    uint64_t h2 = seed;
    const size_t nblocks = len / 16;

    for (size_t i = 0; i < nblocks; ++i)
    {
        uint64_t k1 = load64_le(data + i * 16);
        uint64_t k2 = load64_le(data + i * 16 + 8);

        k1 *= c1;
        k1 = rotl64(k1, 31);
        k1 *= c2;
        h1 ^= k1;
        h1 = rotl64(h1, 27);
        h1 += h2;
        h1 = h1 * 5 + 0x52dce729;

        k2 *= c2;
        k2 = rotl64(k2, 33);
        k2 *= c1;
        h2 ^= k2;
        h2 = rotl64(h2, 31);
        h2 += h1;
        h2 = h2 * 5 + 0x38496740;
    }

    const uint8_t* tail = data + nblocks * 16;
    uint64_t k1 = 0;
    uint64_t k2 = 0;

    switch (len & 15)
    {
        case 15:
            k2 ^= static_cast<uint64_t>(tail[14]) << 48;
            // fall through
        case 14:
            k2 ^= static_cast<uint64_t>(tail[13]) << 40;
            // fall through
        case 13:
            k2 ^= static_cast<uint64_t>(tail[12]) << 32;
            // fall through
        case 12:
            k2 ^= static_cast<uint64_t>(tail[11]) << 24;
            // fall through
        case 11:
            k2 ^= static_cast<uint64_t>(tail[10]) << 16;
            // fall through
        case 10:
            k2 ^= static_cast<uint64_t>(tail[9]) << 8;
            // fall through
        case 9:
            k2 ^= static_cast<uint64_t>(tail[8]);
            k2 *= c2;
            k2 = rotl64(k2, 33);
            k2 *= c1;
            h2 ^= k2;
            // fall through
        case 8:
            k1 ^= static_cast<uint64_t>(tail[7]) << 56;
            // fall through
        case 7:
            k1 ^= static_cast<uint64_t>(tail[6]) << 48;
            // fall through
        case 6:
            k1 ^= static_cast<uint64_t>(tail[5]) << 40;
            // fall through
        case 5:
            k1 ^= static_cast<uint64_t>(tail[4]) << 32;
            // fall through
        case 4:
            k1 ^= static_cast<uint64_t>(tail[3]) << 24;
            // fall through
        case 3:
            k1 ^= static_cast<uint64_t>(tail[2]) << 16;
            // fall through
        case 2:
            k1 ^= static_cast<uint64_t>(tail[1]) << 8;
            // fall through
        case 1:
            k1 ^= static_cast<uint64_t>(tail[0]);
            k1 *= c1;
            k1 = rotl64(k1, 31);
            k1 *= c2;
            h1 ^= k1;
    }

    h1 ^= static_cast<uint64_t>(len);
    h2 ^= static_cast<uint64_t>(len);
    h1 += h2;
    h2 += h1;

    h1 ^= h1 >> 33;
    h1 *= 0xff51afd7ed558ccdULL;
    h1 ^= h1 >> 33;
    h1 *= 0xc4ceb9fe1a85ec53ULL;
    h1 ^= h1 >> 33;

    h2 ^= h2 >> 33;
    h2 *= 0xff51afd7ed558ccdULL;
    h2 ^= h2 >> 33;
    h2 *= 0xc4ceb9fe1a85ec53ULL;
    h2 ^= h2 >> 33;

    h1 += h2;
    h2 += h1;
    return h1;
}

// ---------------------------------------------------------------------------
// 整数哈希（哈希表映射）
// ---------------------------------------------------------------------------

uint32_t hash_thomas_wang(uint32_t key)
{
    key = ~key + (key << 15);
    key = key ^ (key >> 12);
    key = key + (key << 2);
    key = key ^ (key >> 4);
    key = key * 2057;
    key = key ^ (key >> 16);
    return key;
}

uint32_t hash_multiplication(uint32_t key, uint32_t table_size)
{
    if (table_size == 0)
    {
        return 0;
    }

    // Knuth: h(k) = floor(m * frac(k * A)), A = (√5-1)/2
    // 2^32 * A ≈ 2654435769
    const uint32_t fraction = key * 2654435769u;
    return static_cast<uint32_t>((static_cast<uint64_t>(fraction) * table_size) >> 32);
}

uint32_t hash_division(uint32_t key, uint32_t table_size)
{
    if (table_size == 0)
    {
        return 0;
    }
    return key % table_size;
}

// ---------------------------------------------------------------------------
// 校验和
// ---------------------------------------------------------------------------

uint16_t hash_crc16(const uint8_t* data, size_t len)
{
    uint16_t crc = 0xFFFF;
    for (size_t i = 0; i < len; ++i)
    {
        crc ^= static_cast<uint16_t>(data[i]) << 8;
        for (int j = 0; j < 8; ++j)
        {
            crc = (crc & 0x8000) ? static_cast<uint16_t>((crc << 1) ^ 0x1021) :
                                   static_cast<uint16_t>(crc << 1);
        }
    }
    return crc;
}

uint32_t hash_crc32(const uint8_t* data, size_t len)
{
    return crc32_generic(data, len, 0xEDB88320u);
}

uint32_t hash_crc32c(const uint8_t* data, size_t len)
{
    return crc32_generic(data, len, 0x82F63B78u);
}

uint32_t hash_adler32(const uint8_t* data, size_t len)
{
    const uint32_t base = 65521;
    const size_t nmax = 5552;
    uint32_t a = 1;
    uint32_t b = 0;

    while (len > 0)
    {
        size_t n = (len < nmax) ? len : nmax;
        len -= n;
        while (n-- > 0)
        {
            a += *data++;
            b += a;
        }
        a %= base;
        b %= base;
    }
    return (b << 16) | a;
}

} // namespace cutl
