/**
 * @file test_hash.cpp
 * @brief Unit tests for cutl hash functions.
 *
 * Algorithm tests use published vectors where they exist (FNV, MurmurHash3_x86_32,
 * Jenkins lookup3/hashlittle, Java String.hashCode for ASCII). Remaining expected
 * values are from independent implementations of the same published algorithms.
 */

#include "common_util/hash.h"
#include <gtest/gtest.h>
#include <string>
#include <unordered_set>

namespace
{

const uint8_t* u8(const char* s)
{
    return reinterpret_cast<const uint8_t*>(s);
}

} // namespace

TEST(HashTest, SameInputProducesSameOutput)
{
    std::string s = "Hello, common_util!";
    EXPECT_EQ(cutl::hash_polynomial_rolling(s), cutl::hash_polynomial_rolling(s));
    EXPECT_EQ(cutl::hash_djb2(s), cutl::hash_djb2(s));
    EXPECT_EQ(cutl::hash_fnv1_32(s), cutl::hash_fnv1_32(s));
    EXPECT_EQ(cutl::hash_fnv1_64(s), cutl::hash_fnv1_64(s));
    EXPECT_EQ(cutl::hash_fnv1a_32(s), cutl::hash_fnv1a_32(s));
    EXPECT_EQ(cutl::hash_fnv1a_64(s), cutl::hash_fnv1a_64(s));
    EXPECT_EQ(cutl::hash_one_at_a_time(s), cutl::hash_one_at_a_time(s));
    EXPECT_EQ(cutl::hash_lookup3(s), cutl::hash_lookup3(s));
    EXPECT_EQ(cutl::hash_murmur3_32(s), cutl::hash_murmur3_32(s));
    EXPECT_EQ(cutl::hash_murmur3_64(s), cutl::hash_murmur3_64(s));
    EXPECT_EQ(cutl::hash_murmur2_32(s), cutl::hash_murmur2_32(s));
    EXPECT_EQ(cutl::hash_murmur2_64(s), cutl::hash_murmur2_64(s));
    EXPECT_EQ(cutl::hash_crc16(s), cutl::hash_crc16(s));
    EXPECT_EQ(cutl::hash_crc32(s), cutl::hash_crc32(s));
    EXPECT_EQ(cutl::hash_crc32c(s), cutl::hash_crc32c(s));
    EXPECT_EQ(cutl::hash_adler32(s), cutl::hash_adler32(s));
}

TEST(HashTest, DifferentInputProducesDifferentOutput)
{
    std::string a = "abc";
    std::string b = "abd";
    EXPECT_NE(cutl::hash_djb2(a), cutl::hash_djb2(b));
    EXPECT_NE(cutl::hash_fnv1a_32(a), cutl::hash_fnv1a_32(b));
    EXPECT_NE(cutl::hash_fnv1a_64(a), cutl::hash_fnv1a_64(b));
    EXPECT_NE(cutl::hash_fnv1_64(a), cutl::hash_fnv1_64(b));
    EXPECT_NE(cutl::hash_murmur3_32(a), cutl::hash_murmur3_32(b));
    EXPECT_NE(cutl::hash_murmur3_64(a), cutl::hash_murmur3_64(b));
    EXPECT_NE(cutl::hash_murmur2_32(a), cutl::hash_murmur2_32(b));
    EXPECT_NE(cutl::hash_murmur2_64(a), cutl::hash_murmur2_64(b));
    EXPECT_NE(cutl::hash_crc32(a), cutl::hash_crc32(b));
}

TEST(HashTest, BytePointerMatchesStringOverload)
{
    std::string s = "Hello World!";
    const uint8_t* data = reinterpret_cast<const uint8_t*>(s.data());
    const size_t len = s.size();

    EXPECT_EQ(cutl::hash_polynomial_rolling(s), cutl::hash_polynomial_rolling(data, len));
    EXPECT_EQ(cutl::hash_djb2(s), cutl::hash_djb2(data, len));
    EXPECT_EQ(cutl::hash_fnv1_32(s), cutl::hash_fnv1_32(data, len));
    EXPECT_EQ(cutl::hash_fnv1_64(s), cutl::hash_fnv1_64(data, len));
    EXPECT_EQ(cutl::hash_fnv1a_32(s), cutl::hash_fnv1a_32(data, len));
    EXPECT_EQ(cutl::hash_fnv1a_64(s), cutl::hash_fnv1a_64(data, len));
    EXPECT_EQ(cutl::hash_one_at_a_time(s), cutl::hash_one_at_a_time(data, len));
    EXPECT_EQ(cutl::hash_lookup3(s), cutl::hash_lookup3(data, len));
    EXPECT_EQ(cutl::hash_murmur3_32(s), cutl::hash_murmur3_32(data, len));
    EXPECT_EQ(cutl::hash_murmur3_64(s), cutl::hash_murmur3_64(data, len));
    EXPECT_EQ(cutl::hash_murmur2_32(s), cutl::hash_murmur2_32(data, len));
    EXPECT_EQ(cutl::hash_murmur2_64(s), cutl::hash_murmur2_64(data, len));
    EXPECT_EQ(cutl::hash_crc16(s), cutl::hash_crc16(data, len));
    EXPECT_EQ(cutl::hash_crc32(s), cutl::hash_crc32(data, len));
    EXPECT_EQ(cutl::hash_crc32c(s), cutl::hash_crc32c(data, len));
    EXPECT_EQ(cutl::hash_adler32(s), cutl::hash_adler32(data, len));
}

TEST(HashTest, PolynomialRollingKnownVectors)
{
    // Same recurrence as Java String.hashCode for ASCII / Latin-1 bytes: h = 31 * h + b
    EXPECT_EQ(cutl::hash_polynomial_rolling(""), 0u);
    EXPECT_EQ(cutl::hash_polynomial_rolling("a"), 97u);
    EXPECT_EQ(cutl::hash_polynomial_rolling("Hello"), 69609650u);
    EXPECT_EQ(cutl::hash_polynomial_rolling("abc"), 0x00017862u);
    EXPECT_EQ(cutl::hash_polynomial_rolling("Hello World!"), 0xc63cb61du);

    const uint8_t wo[] = {0xE6, 0x88, 0x91}; // UTF-8 "我"
    EXPECT_EQ(cutl::hash_polynomial_rolling(wo, sizeof(wo)), 0x0003706fu);
}

TEST(HashTest, Djb2KnownVectors)
{
    // Bernstein djb2: hash = 5381; hash = hash * 33 + byte
    EXPECT_EQ(cutl::hash_djb2(""), 5381u);
    EXPECT_EQ(cutl::hash_djb2("a"), 0x0002b606u);
    EXPECT_EQ(cutl::hash_djb2("hello"), 0x0f923099u);
    EXPECT_EQ(cutl::hash_djb2("Hello"), 0x0d4f2079u);
    EXPECT_EQ(cutl::hash_djb2("foobar"), 0xfde460beu);
    EXPECT_EQ(cutl::hash_djb2("Hello World!"), 0x767a20c2u);
}

TEST(HashTest, Fnv1_32KnownVectors)
{
    // Fowler–Noll–Vo FNV-1 32-bit (offset 0x811c9dc5, prime 0x01000193)
    EXPECT_EQ(cutl::hash_fnv1_32(""), 0x811c9dc5u);
    EXPECT_EQ(cutl::hash_fnv1_32("a"), 0x050c5d7eu);
    EXPECT_EQ(cutl::hash_fnv1_32("foo"), 0x408f5e13u);
    EXPECT_EQ(cutl::hash_fnv1_32("foobar"), 0x31f0b262u);
    EXPECT_EQ(cutl::hash_fnv1_32("hello"), 0xb6fa7167u);
    EXPECT_EQ(cutl::hash_fnv1_32("Hello World!"), 0x12a9a41cu);
}

TEST(HashTest, Fnv1_64KnownVectors)
{
    // FNV-1 64-bit: multiply then XOR. Empty string is the 64-bit offset basis.
    EXPECT_EQ(cutl::hash_fnv1_64(""), 0xcbf29ce484222325ull);
    EXPECT_EQ(cutl::hash_fnv1_64("a"), 0xaf63bd4c8601b7beull);
    EXPECT_EQ(cutl::hash_fnv1_64("foo"), 0xd8cbc7186ba13533ull);
    EXPECT_EQ(cutl::hash_fnv1_64("foobar"), 0x340d8765a4dda9c2ull);
    EXPECT_EQ(cutl::hash_fnv1_64("hello"), 0x7b495389bdbdd4c7ull);
}

TEST(HashTest, Fnv1a_32KnownVectors)
{
    // FNV-1a 32-bit published vectors (XOR then multiply)
    EXPECT_EQ(cutl::hash_fnv1a_32(""), 0x811c9dc5u);
    EXPECT_EQ(cutl::hash_fnv1a_32("a"), 0xe40c292cu);
    EXPECT_EQ(cutl::hash_fnv1a_32("foo"), 0xa9f37ed7u);
    EXPECT_EQ(cutl::hash_fnv1a_32("foobar"), 0xbf9cf968u);
    EXPECT_EQ(cutl::hash_fnv1a_32("hello"), 0x4f9f2cabu);
    EXPECT_EQ(cutl::hash_fnv1a_32("Hello World!"), 0xb1ea4872u);
}

TEST(HashTest, Fnv1a_64KnownVectors)
{
    // FNV-1a 64-bit (offset 0xcbf29ce484222325, prime 0x100000001b3)
    EXPECT_EQ(cutl::hash_fnv1a_64(""), 0xcbf29ce484222325ull);
    EXPECT_EQ(cutl::hash_fnv1a_64("a"), 0xaf63dc4c8601ec8cull);
    EXPECT_EQ(cutl::hash_fnv1a_64("foobar"), 0x85944171f73967e8ull);
    EXPECT_EQ(cutl::hash_fnv1a_64("hello"), 0xa430d84680aabd0bull);
    EXPECT_EQ(cutl::hash_fnv1a_64("Hello World!"), 0x8c0ec8d1fb9e6e32ull);
}

TEST(HashTest, OneAtATimeKnownVectors)
{
    // Jenkins one-at-a-time: per-byte add/shift/xor, then final mix
    EXPECT_EQ(cutl::hash_one_at_a_time(""), 0u);
    EXPECT_EQ(cutl::hash_one_at_a_time("a"), 0xca2e9442u);
    EXPECT_EQ(cutl::hash_one_at_a_time("hello"), 0xc8fd181bu);
    EXPECT_EQ(cutl::hash_one_at_a_time("foobar"), 0xf952fde7u);
    EXPECT_EQ(cutl::hash_one_at_a_time("Hello World!"), 0x2476cedbu);

    const uint8_t wo[] = {0xE6, 0x88, 0x91};
    EXPECT_EQ(cutl::hash_one_at_a_time(wo, sizeof(wo)), 0xc191720cu);
}

TEST(HashTest, Lookup3KnownVectors)
{
    // Bob Jenkins lookup3 / hashlittle (http://burtleburtle.net/bob/c/lookup3.c)
    EXPECT_EQ(cutl::hash_lookup3(static_cast<const uint8_t*>(nullptr), 0, 0), 0xdeadbeefu);

    const char* four_score = "Four score and seven years ago";
    EXPECT_EQ(cutl::hash_lookup3(u8(four_score), 30, 0), 0x17770551u);
    EXPECT_EQ(cutl::hash_lookup3(u8(four_score), 30, 1), 0xcd628161u);

    EXPECT_EQ(cutl::hash_lookup3("a"), 0x58d68708u);
    EXPECT_EQ(cutl::hash_lookup3("hello"), 0x34cbbc6eu);
    EXPECT_EQ(cutl::hash_lookup3("foobar"), 0xaeb72b0cu);
    EXPECT_EQ(cutl::hash_lookup3("Hello World!"), 0xd5cf3256u);

    // 12-byte block boundary and one leftover byte
    EXPECT_EQ(cutl::hash_lookup3("abcdefghijkl"), 0x4012f87bu);
    EXPECT_EQ(cutl::hash_lookup3("abcdefghijklm"), 0x928128f9u);
}

TEST(HashTest, Murmur3_32KnownVectors)
{
    // MurmurHash3_x86_32 (Austin Appleby / SMHasher)
    EXPECT_EQ(cutl::hash_murmur3_32(static_cast<const uint8_t*>(nullptr), 0, 0), 0u);
    EXPECT_EQ(cutl::hash_murmur3_32(static_cast<const uint8_t*>(nullptr), 0, 1), 0x514e28b7u);
    EXPECT_EQ(cutl::hash_murmur3_32(static_cast<const uint8_t*>(nullptr), 0, 0xffffffffu),
              0x81f16f39u);

    const uint8_t ff4[] = {0xff, 0xff, 0xff, 0xff};
    EXPECT_EQ(cutl::hash_murmur3_32(ff4, sizeof(ff4), 0), 0x76293b50u);

    const uint8_t endian[] = {0x21, 0x43, 0x65, 0x87};
    EXPECT_EQ(cutl::hash_murmur3_32(endian, sizeof(endian), 0), 0xf55b516bu);
    EXPECT_EQ(cutl::hash_murmur3_32(endian, 3, 0), 0x7e4a8634u);
    EXPECT_EQ(cutl::hash_murmur3_32(endian, 2, 0), 0xa0f7b07au);
    EXPECT_EQ(cutl::hash_murmur3_32(endian, 1, 0), 0x72661cf4u);

    const uint8_t zeros4[] = {0x00, 0x00, 0x00, 0x00};
    EXPECT_EQ(cutl::hash_murmur3_32(zeros4, 4, 0), 0x2362f9deu);
    EXPECT_EQ(cutl::hash_murmur3_32(zeros4, 3, 0), 0x85f0b427u);
    EXPECT_EQ(cutl::hash_murmur3_32(zeros4, 2, 0), 0x30f4c306u);
    EXPECT_EQ(cutl::hash_murmur3_32(zeros4, 1, 0), 0x514e28b7u);

    EXPECT_EQ(cutl::hash_murmur3_32("hello"), 0x248bfa47u);
    EXPECT_EQ(cutl::hash_murmur3_32("abc"), 0xb3dd93fau);
    EXPECT_EQ(cutl::hash_murmur3_32("test"), 0xba6bd213u);
    EXPECT_EQ(cutl::hash_murmur3_32("Hello, world!"), 0xc0363e43u);

    const uint32_t seed = 0x9747b28c;
    EXPECT_EQ(cutl::hash_murmur3_32(u8("a"), 1, seed), 0x7fa09ea6u);
    EXPECT_EQ(cutl::hash_murmur3_32(u8("ab"), 2, seed), 0x74875592u);
    EXPECT_EQ(cutl::hash_murmur3_32(u8("abc"), 3, seed), 0xc84a62ddu);
    EXPECT_EQ(cutl::hash_murmur3_32(u8("abcd"), 4, seed), 0xf0478627u);
    EXPECT_EQ(cutl::hash_murmur3_32(u8("Hello, world!"), 13, seed), 0x24884cbau);
    EXPECT_EQ(cutl::hash_murmur3_32(u8("The quick brown fox jumps over the lazy dog"), 43, seed),
              0x2fa826cdu);
}

TEST(HashTest, Murmur2_64KnownVectors)
{
    // MurmurHash64A (MurmurHash2, 64-bit platforms)
    EXPECT_EQ(cutl::hash_murmur2_64(static_cast<const uint8_t*>(nullptr), 0, 0), 0ull);
    EXPECT_EQ(cutl::hash_murmur2_64("a"), 0x071717d2d36b6b11ull);
    EXPECT_EQ(cutl::hash_murmur2_64("hello"), 0x1e68d17c457bf117ull);
    EXPECT_EQ(cutl::hash_murmur2_64("abc"), 0x9cc9c33498a95efbull);
    EXPECT_EQ(cutl::hash_murmur2_64("foobar"), 0xd49f461720d7a196ull);
    EXPECT_EQ(cutl::hash_murmur2_64("Hello World!"), 0x899e7fbbf113dfa4ull);

    // 8-byte block boundary and one leftover byte
    EXPECT_EQ(cutl::hash_murmur2_64("12345678"), 0x758f67d162b2d202ull);
    EXPECT_EQ(cutl::hash_murmur2_64("123456789"), 0x4977490251674330ull);

    EXPECT_EQ(cutl::hash_murmur2_64(u8("hello"), 5, 42), 0xd417125ccb971887ull);
}

TEST(HashTest, Murmur2_32KnownVectors)
{
    // MurmurHash2 32-bit
    EXPECT_EQ(cutl::hash_murmur2_32(static_cast<const uint8_t*>(nullptr), 0, 0), 0u);
    EXPECT_EQ(cutl::hash_murmur2_32("a"), 0x92685f5eu);
    EXPECT_EQ(cutl::hash_murmur2_32("hello"), 0xe56129cbu);
    EXPECT_EQ(cutl::hash_murmur2_32("abc"), 0x13577c9bu);
    EXPECT_EQ(cutl::hash_murmur2_32("foobar"), 0x6715a92eu);
    EXPECT_EQ(cutl::hash_murmur2_32("Hello World!"), 0x69b24a14u);
    EXPECT_EQ(cutl::hash_murmur2_32(u8("hello"), 5, 1), 0xa631918eu);
}

TEST(HashTest, Murmur3_64KnownVectors)
{
    // First 64 bits of MurmurHash3_x64_128
    EXPECT_EQ(cutl::hash_murmur3_64(static_cast<const uint8_t*>(nullptr), 0, 0), 0ull);
    EXPECT_EQ(cutl::hash_murmur3_64("a"), 0x85555565f6597889ull);
    EXPECT_EQ(cutl::hash_murmur3_64("hello"), 0xcbd8a7b341bd9b02ull);
    EXPECT_EQ(cutl::hash_murmur3_64("abc"), 0xb4963f3f3fad7867ull);
    EXPECT_EQ(cutl::hash_murmur3_64("foobar"), 0xbdd2ae7116c85a45ull);
    EXPECT_EQ(cutl::hash_murmur3_64("Hello World!"), 0x80025454af3196b2ull);
    EXPECT_EQ(cutl::hash_murmur3_64("123456789"), 0x3c84645edb66cca4ull);
    EXPECT_EQ(cutl::hash_murmur3_64(u8("hello"), 5, 42), 0xc4b8b3c960af6f08ull);
}

TEST(HashTest, ChecksumKnownVectors)
{
    // CRC catalogue check values for "123456789"
    EXPECT_EQ(cutl::hash_crc16("123456789"), 0x29b1);
    EXPECT_EQ(cutl::hash_crc32("123456789"), 0xcbf43926u);
    EXPECT_EQ(cutl::hash_crc32c("123456789"), 0xe3069283u);
    EXPECT_EQ(cutl::hash_adler32("123456789"), 0x091e01deu);

    EXPECT_EQ(cutl::hash_crc16(""), 0xffff);
    EXPECT_EQ(cutl::hash_crc32(""), 0u);
    EXPECT_EQ(cutl::hash_crc32c(""), 0u);
    EXPECT_EQ(cutl::hash_adler32(""), 1u);

    EXPECT_EQ(cutl::hash_crc32("a"), 0xe8b7be43u);
    EXPECT_EQ(cutl::hash_crc32c("a"), 0xc1d04330u);
    EXPECT_EQ(cutl::hash_crc16("a"), 0x9d77);
    EXPECT_EQ(cutl::hash_adler32("a"), 0x00620062u);

    EXPECT_EQ(cutl::hash_crc32("hello"), 0x3610a686u);
    EXPECT_EQ(cutl::hash_crc32c("hello"), 0x9a71bb4cu);
    EXPECT_EQ(cutl::hash_crc16("hello"), 0xd26e);
    EXPECT_EQ(cutl::hash_adler32("hello"), 0x062c0215u);

    EXPECT_EQ(cutl::hash_crc32("Hello World!"), 0x1c291ca3u);
    EXPECT_EQ(cutl::hash_crc32c("Hello World!"), 0xfe6cf1dcu);
    EXPECT_EQ(cutl::hash_crc16("Hello World!"), 0x882a);
    EXPECT_EQ(cutl::hash_adler32("Hello World!"), 0x1c49043eu);
}

TEST(HashTest, HighBitBytesAreTreatedAsUnsigned)
{
    // UTF-8 for "我" is E6 88 91; hashing must use 0xE6 rather than a sign-extended char
    const uint8_t raw[] = {0xE6, 0x88, 0x91};
    std::string s(reinterpret_cast<const char*>(raw), sizeof(raw));

    EXPECT_EQ(cutl::hash_fnv1a_32(s), 0xdcddc496u);
    EXPECT_EQ(cutl::hash_fnv1a_64(s), 0x337f5b1b7499cb76ull);
    EXPECT_EQ(cutl::hash_djb2(s), 0x0b8a9764u);
    EXPECT_EQ(cutl::hash_murmur3_32(s), 0x296df263u);
    EXPECT_EQ(cutl::hash_murmur2_64(s), 0xeb594bb2a550d163ull);
    EXPECT_EQ(cutl::hash_fnv1a_32(s), cutl::hash_fnv1a_32(raw, sizeof(raw)));
}

TEST(HashTest, ThomasWangKnownVectors)
{
    // hash32shift: ~k+(k<<15), xor>>12, +<<2, xor>>4, *2057, xor>>16
    EXPECT_EQ(cutl::hash_thomas_wang(0u), 0xcaa3caa3u);
    EXPECT_EQ(cutl::hash_thomas_wang(1u), 0x12d60bf6u);
    EXPECT_EQ(cutl::hash_thomas_wang(6u), 0x710506b9u);
    EXPECT_EQ(cutl::hash_thomas_wang(123u), 0x0cfd698bu);
    EXPECT_EQ(cutl::hash_thomas_wang(127u), 0x806aae0cu);
    EXPECT_EQ(cutl::hash_thomas_wang(0x80000000u), 0x6551e551u);
    EXPECT_EQ(cutl::hash_thomas_wang(0xffffffffu), 0xbd55fc18u);
}

TEST(HashTest, MultiplicationAndDivisionKnownVectors)
{
    EXPECT_EQ(cutl::hash_division(6u, 20u), 6u);
    EXPECT_EQ(cutl::hash_division(127u, 20u), 7u);
    EXPECT_EQ(cutl::hash_division(100u, 20u), 0u);
    EXPECT_EQ(cutl::hash_division(1u, 0u), 0u);

    // Knuth multiplicative: h = floor(m * frac(k * A)), A_int = 2654435769
    EXPECT_EQ(cutl::hash_multiplication(6u, 20u), 14u);
    EXPECT_EQ(cutl::hash_multiplication(127u, 20u), 9u);
    EXPECT_EQ(cutl::hash_multiplication(1u, 20u), 12u);
    EXPECT_EQ(cutl::hash_multiplication(100u, 20u), 16u);
    EXPECT_EQ(cutl::hash_multiplication(6u, 100u), 70u);
    EXPECT_EQ(cutl::hash_multiplication(127u, 100u), 49u);
    EXPECT_EQ(cutl::hash_multiplication(1u, 0u), 0u);

    constexpr uint32_t kTable = 100;
    for (uint32_t i = 0; i < 50; ++i)
    {
        EXPECT_LT(cutl::hash_multiplication(i, kTable), kTable);
        EXPECT_LT(cutl::hash_division(i, kTable), kTable);
    }
}

TEST(HashTest, ReasonableSpread)
{
    std::unordered_set<uint64_t> hashes;
    for (int i = 0; i < 1000; ++i)
    {
        hashes.insert(cutl::hash_murmur2_64("element_" + std::to_string(i)));
    }
    EXPECT_GE(hashes.size(), 800u);
}

TEST(HashTest, Murmur2_64BytePointerMatchesString)
{
    std::string s = "common_util_murmur2_void_ptr";
    auto from_str = cutl::hash_murmur2_64(s);
    auto from_ptr = cutl::hash_murmur2_64(reinterpret_cast<const uint8_t*>(s.data()), s.size());
    EXPECT_EQ(from_str, from_ptr);

    auto from_ptr_seed =
        cutl::hash_murmur2_64(reinterpret_cast<const uint8_t*>(s.data()), s.size(), 0xDEADBEEF);
    EXPECT_NE(from_ptr, from_ptr_seed);
}
