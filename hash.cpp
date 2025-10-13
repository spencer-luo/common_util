#include <cstdint>
#include <iostream>
#include <string>

// DJB2哈希算法
uint32_t DJB2(const std::string& str)
{
    // 5381: 经过大量测试选择的质数
    uint32_t djb2 = 5381;
    for (char c : str)
    {
        djb2 = ((djb2 << 5) + djb2) + c; // DJB2 * 33 + c
    }
    return djb2;
}

static const uint32_t FNV_OFFSET_BASIS = 2166136261u;
static const uint32_t FNV_PRIME = 16777619u;

// FNV-1 32位版本
uint32_t fnv1_32(const std::string& str)
{
    uint32_t hash = FNV_OFFSET_BASIS;
    for (char c : str)
    {
        hash = (hash * FNV_PRIME) ^ c;
    }
    return hash;
}

// FNV-1a 32位版本（推荐使用）
uint32_t fnv1a_32(const std::string& str)
{
    uint32_t hash = FNV_OFFSET_BASIS;
    for (char c : str)
    {
        hash = (hash ^ c) * FNV_PRIME;
    }
    return hash;
}

static const uint64_t FNV_OFFSET_BASIS_64 = 14695981039346656037u;
static const uint64_t FNV_PRIME_64 = 1099511628211u;

// FNV-1a 64位版本
uint64_t fnv1a_64(const std::string& str)
{
    uint64_t hash = FNV_OFFSET_BASIS_64;
    for (char c : str)
    {
        hash = (hash ^ static_cast<uint8_t>(c)) * FNV_PRIME_64;
    }
    return hash;
}

// Jenkins one_at_a_time 哈希（简单但高质量）
uint32_t one_at_a_time(const std::string& str)
{
    uint32_t hash = 0;

    for (char c : str)
    {
        hash += static_cast<uint8_t>(c);
        hash += (hash << 10);
        hash ^= (hash >> 6);
    }

    hash += (hash << 3);
    hash ^= (hash >> 11);
    hash += (hash << 15);

    return hash;
}

// Jenkins lookup3 哈希（更复杂的Jenkins哈希）
uint32_t lookup3(const void* key, size_t length, uint32_t initval = 0)
{
    uint32_t a, b, c;
    const uint8_t* k = static_cast<const uint8_t*>(key);

    // 设置初始值
    a = b = c = 0xdeadbeef + static_cast<uint32_t>(length) + initval;

    // 主要哈希循环
    while (length > 12)
    {
        a += k[0] + (static_cast<uint32_t>(k[1]) << 8) + (static_cast<uint32_t>(k[2]) << 16) +
             (static_cast<uint32_t>(k[3]) << 24);
        b += k[4] + (static_cast<uint32_t>(k[5]) << 8) + (static_cast<uint32_t>(k[6]) << 16) +
             (static_cast<uint32_t>(k[7]) << 24);
        c += k[8] + (static_cast<uint32_t>(k[9]) << 8) + (static_cast<uint32_t>(k[10]) << 16) +
             (static_cast<uint32_t>(k[11]) << 24);

        // 混合函数
        a -= c;
        a ^= ((c << 4) | (c >> 28));
        c += b;
        b -= a;
        b ^= ((a << 6) | (a >> 26));
        a += c;
        c -= b;
        c ^= ((b << 8) | (b >> 24));
        b += a;
        a -= c;
        a ^= ((c << 16) | (c >> 16));
        c += b;
        b -= a;
        b ^= ((a << 19) | (a >> 13));
        a += c;
        c -= b;
        c ^= ((b << 4) | (b >> 28));
        b += a;

        k += 12;
        length -= 12;
    }

    // 处理最后1-12个字节
    switch (length)
    {
        case 12:
            c += (static_cast<uint32_t>(k[11]) << 24);
            [[fallthrough]];
        case 11:
            c += (static_cast<uint32_t>(k[10]) << 16);
            [[fallthrough]];
        case 10:
            c += (static_cast<uint32_t>(k[9]) << 8);
            [[fallthrough]];
        case 9:
            c += k[8];
            [[fallthrough]];
        case 8:
            b += (static_cast<uint32_t>(k[7]) << 24);
            [[fallthrough]];
        case 7:
            b += (static_cast<uint32_t>(k[6]) << 16);
            [[fallthrough]];
        case 6:
            b += (static_cast<uint32_t>(k[5]) << 8);
            [[fallthrough]];
        case 5:
            b += k[4];
            [[fallthrough]];
        case 4:
            a += (static_cast<uint32_t>(k[3]) << 24);
            [[fallthrough]];
        case 3:
            a += (static_cast<uint32_t>(k[2]) << 16);
            [[fallthrough]];
        case 2:
            a += (static_cast<uint32_t>(k[1]) << 8);
            [[fallthrough]];
        case 1:
            a += k[0];
            break;
        case 0:
            return c;
    }

    // 最终混合
    c ^= b;
    c -= ((b << 14) | (b >> 18));
    a ^= c;
    a -= ((c << 11) | (c >> 21));
    b ^= a;
    b -= ((a << 25) | (a >> 7));
    c ^= b;
    c -= ((b << 16) | (b >> 16));
    a ^= c;
    a -= ((c << 4) | (c >> 28));
    b ^= a;
    b -= ((a << 14) | (a >> 18));
    c ^= b;
    c -= ((b << 24) | (b >> 8));

    return c;
}

// MurmurHash3 32位版本
uint32_t murmur3_32(const void* key, size_t len, uint32_t seed = 0)
{
    const uint8_t* data = static_cast<const uint8_t*>(key);
    const int nblocks = len / 4;

    uint32_t h1 = seed;

    const uint32_t c1 = 0xcc9e2d51;
    const uint32_t c2 = 0x1b873593;

    // 主体处理
    const uint32_t* blocks = reinterpret_cast<const uint32_t*>(data + nblocks * 4);
    for (int i = -nblocks; i; i++)
    {
        uint32_t k1 = blocks[i];

        k1 *= c1;
        k1 = (k1 << 15) | (k1 >> 17);
        k1 *= c2;

        h1 ^= k1;
        h1 = (h1 << 13) | (h1 >> 19);
        h1 = h1 * 5 + 0xe6546b64;
    }

    // 尾部处理
    const uint8_t* tail = data + nblocks * 4;
    uint32_t k1 = 0;

    switch (len & 3)
    {
        case 3:
            k1 ^= tail[2] << 16;
            [[fallthrough]];
        case 2:
            k1 ^= tail[1] << 8;
            [[fallthrough]];
        case 1:
            k1 ^= tail[0];
            k1 *= c1;
            k1 = (k1 << 15) | (k1 >> 17);
            k1 *= c2;
            h1 ^= k1;
    }

    // 最终混合
    h1 ^= len;
    h1 ^= h1 >> 16;
    h1 *= 0x85ebca6b;
    h1 ^= h1 >> 13;
    h1 *= 0xc2b2ae35;
    h1 ^= h1 >> 16;

    return h1;
}

void TestDJB2()
{
    std::string str1("Hello World");
    std::string str2("我爱中国");
    std::cout << str1 << " --> " << DJB2(str1) << std::endl;
    std::cout << str2 << " --> " << DJB2(str2) << std::endl;
}

void TestFNV1()
{
    std::string str1("Hello World");
    std::string str2("我爱中国");

    std::cout << "fnv1_32:" << std::endl;
    std::cout << str1 << " --> " << fnv1_32(str1) << std::endl;
    std::cout << str2 << " --> " << fnv1_32(str2) << std::endl;

    std::cout << "fnv1a_32:" << std::endl;
    std::cout << str1 << " --> " << fnv1a_32(str1) << std::endl;
    std::cout << str2 << " --> " << fnv1a_32(str2) << std::endl;

    std::cout << "fnv1a_64:" << std::endl;
    std::cout << str1 << " --> " << fnv1a_64(str1) << std::endl;
    std::cout << str2 << " --> " << fnv1a_64(str2) << std::endl;
}

void TestJenkins()
{
    std::string str1("Hello World");
    std::string str2("我爱中国");

    std::cout << "one_at_a_time:" << std::endl;
    std::cout << str1 << " --> " << one_at_a_time(str1) << std::endl;
    std::cout << str2 << " --> " << one_at_a_time(str2) << std::endl;

    std::cout << "lookup3:" << std::endl;
    std::cout << str1 << " --> " << lookup3(str1.c_str(), str1.length()) << std::endl;
    std::cout << str2 << " --> " << lookup3(str2.c_str(), str2.length()) << std::endl;
}

void TestMurmurHash()
{
    std::string str1("Hello World");
    std::string str2("我爱中国");

    std::cout << "murmur3_32:" << std::endl;
    std::cout << str1 << " --> " << murmur3_32(str1.c_str(), str1.length()) << std::endl;
    std::cout << str2 << " --> " << murmur3_32(str2.c_str(), str2.length()) << std::endl;
}

int main()
{
    TestDJB2();
    TestFNV1();
    TestJenkins();
    TestMurmurHash();
    return 0;
}