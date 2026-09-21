#include "common.hpp"
#include "common_util/hash.h"

void TestPolynomialRolling()
{
    PrintSubTitle("polynomial rolling hash");

    std::string str1("Hello World!");
    std::string str2("我爱中国！");
    std::cout << str1 << " --> " << cutl::hash_polynomial_rolling(str1) << std::endl;
    std::cout << str2 << " --> " << cutl::hash_polynomial_rolling(str2) << std::endl;
}

void TestDJB2Hash()
{
    PrintSubTitle("DJB2 hash");

    std::string str1("Hello World!");
    std::string str2("我爱中国！");
    std::cout << str1 << " --> " << cutl::hash_djb2(str1) << std::endl;
    std::cout << str2 << " --> " << cutl::hash_djb2(str2) << std::endl;
}

void TestFNV1Hash()
{
    PrintSubTitle("FNV1 hash");

    std::string str1("Hello World!");
    std::string str2("我爱中国！");

    std::cout << "fnv1_32:" << std::endl;
    std::cout << str1 << " --> " << cutl::hash_fnv1_32(str1) << std::endl;
    std::cout << str2 << " --> " << cutl::hash_fnv1_32(str2) << std::endl;

    std::cout << "fnv1_64:" << std::endl;
    std::cout << str1 << " --> " << cutl::hash_fnv1_64(str1) << std::endl;
    std::cout << str2 << " --> " << cutl::hash_fnv1_64(str2) << std::endl;

    std::cout << "fnv1a_32:" << std::endl;
    std::cout << str1 << " --> " << cutl::hash_fnv1a_32(str1) << std::endl;
    std::cout << str2 << " --> " << cutl::hash_fnv1a_32(str2) << std::endl;

    std::cout << "fnv1a_64:" << std::endl;
    std::cout << str1 << " --> " << cutl::hash_fnv1a_64(str1) << std::endl;
    std::cout << str2 << " --> " << cutl::hash_fnv1a_64(str2) << std::endl;
}

void TestJenkinsHash()
{
    PrintSubTitle("Jenkins hash");

    std::string str1("Hello World!");
    std::string str2("我爱中国！");

    std::cout << "one_at_a_time:" << std::endl;
    std::cout << str1 << " --> " << cutl::hash_one_at_a_time(str1) << std::endl;
    std::cout << str2 << " --> " << cutl::hash_one_at_a_time(str2) << std::endl;

    std::cout << "lookup3:" << std::endl;
    std::cout << str1 << " --> " << cutl::hash_lookup3(str1) << std::endl;
    std::cout << str2 << " --> " << cutl::hash_lookup3(str2) << std::endl;
}

void TestMurmurHash()
{
    PrintSubTitle("murmur hash");

    std::string str1("Hello World!");
    std::string str2("我爱中国！");

    std::cout << "murmur3_32:" << std::endl;
    std::cout << str1 << " --> " << cutl::hash_murmur3_32(str1) << std::endl;
    std::cout << str2 << " --> " << cutl::hash_murmur3_32(str2) << std::endl;

    std::cout << "murmur3_64:" << std::endl;
    std::cout << str1 << " --> " << cutl::hash_murmur3_64(str1) << std::endl;
    std::cout << str2 << " --> " << cutl::hash_murmur3_64(str2) << std::endl;

    std::cout << "murmur2_32:" << std::endl;
    std::cout << str1 << " --> " << cutl::hash_murmur2_32(str1) << std::endl;
    std::cout << str2 << " --> " << cutl::hash_murmur2_32(str2) << std::endl;

    std::cout << "murmur2_64:" << std::endl;
    std::cout << str1 << " --> " << cutl::hash_murmur2_64(str1) << std::endl;
    std::cout << str2 << " --> " << cutl::hash_murmur2_64(str2) << std::endl;
}

void TestIntHash()
{
    PrintSubTitle("int hash");

    uint32_t a = 6;
    uint32_t b = 127;

    std::cout << "thomas_wang:" << std::endl;
    std::cout << a << " --> " << cutl::hash_thomas_wang(a) << std::endl;
    std::cout << b << " --> " << cutl::hash_thomas_wang(b) << std::endl;

    std::cout << "multiplicative_hash:" << std::endl;
    std::cout << a << " --> " << cutl::hash_multiplication(a, 20) << std::endl;
    std::cout << b << " --> " << cutl::hash_multiplication(b, 20) << std::endl;

    std::cout << "division_hash:" << std::endl;
    std::cout << a << " --> " << cutl::hash_division(a, 20) << std::endl;
    std::cout << b << " --> " << cutl::hash_division(b, 20) << std::endl;
}

void TestChecksumHash()
{
    PrintSubTitle("checksum hash");

    std::string str1("Hello World!");
    std::string str2("123456789");

    std::cout << "crc16:" << std::endl;
    std::cout << str1 << " --> " << cutl::hash_crc16(str1) << std::endl;
    std::cout << str2 << " --> " << cutl::hash_crc16(str2) << std::endl;

    std::cout << "crc32:" << std::endl;
    std::cout << str1 << " --> " << cutl::hash_crc32(str1) << std::endl;
    std::cout << str2 << " --> " << cutl::hash_crc32(str2) << std::endl;

    std::cout << "crc32c:" << std::endl;
    std::cout << str1 << " --> " << cutl::hash_crc32c(str1) << std::endl;
    std::cout << str2 << " --> " << cutl::hash_crc32c(str2) << std::endl;

    std::cout << "adler32:" << std::endl;
    std::cout << str1 << " --> " << cutl::hash_adler32(str1) << std::endl;
    std::cout << str2 << " --> " << cutl::hash_adler32(str2) << std::endl;
}

void TestStdHash()
{
    PrintSubTitle("std::hash()");

    std::string str1("Hello World!");
    std::string str2("我爱中国！");

    std::cout << "std::hash():" << std::endl;
    std::hash<std::string> hasher;
    std::cout << str1 << " --> " << hasher(str1.c_str()) << std::endl;
    std::cout << str2 << " --> " << hasher(str2.c_str()) << std::endl;
}

void TestHash()
{
    PrintTitle("TestHash");

    TestPolynomialRolling();
    TestDJB2Hash();
    TestFNV1Hash();
    TestJenkinsHash();
    TestMurmurHash();
    TestIntHash();
    TestChecksumHash();
    TestStdHash();
}
