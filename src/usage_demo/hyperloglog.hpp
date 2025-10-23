#include "common.hpp"
#include "common_util/hyperloglog.h"
#include "common_util/timecount.h"

// Test function
static void TestBasicUsage()
{
    PrintSubTitle("basic usage");

    cutl::HyperLogLog hll(15);
    hll.add("abc");
    hll.add("def");
    // hll.add("ghijk");
    hll.add("123");
    std::cout << "count1: " << hll.count() << std::endl;
    hll.add("abc");
    std::cout << "count2: " << hll.count() << std::endl;
}

static void TestMerge()
{
    PrintSubTitle("TestMerge");

    cutl::HyperLogLog hll_01(14);
    cutl::HyperLogLog hll_02(14);

    for (int i = 0; i < 1000; i++)
    {
        hll_01.add("element-" + std::to_string(i) + "-" + std::to_string(rand()));
        hll_02.add("element-" + std::to_string(i) + "-" + std::to_string(rand()));
    }

    std::cout << "hll_01 actual value: 1000, estimated value: " << hll_01.count() << std::endl;
    std::cout << "hll_02 actual value: 1000, estimated value: " << hll_02.count() << std::endl;
    hll_01.merge(hll_02);
    std::cout << "hll_01 estimated value after merging: " << hll_01.count() << std::endl;
}

// Generate 64-bit random hash (for testing)
uint64_t generateRandomHash()
{
    static std::random_device rd;
    static std::mt19937_64 gen(rd());
    static std::uniform_int_distribution<uint64_t> dis(1, 10000000);
    return dis(gen);
}

// Test function
static void TestPerformance()
{
    PrintSubTitle("TestPerformance");

    cutl::timecount counter("TestPerformance");

    cutl::HyperLogLog hll(14);
    const uint64_t actualCount = 10000; // 10 million

    std::cout << "Test data volume: " << actualCount << std::endl;

    for (uint64_t i = 0; i < actualCount; i++)
    {
        // Add using 64-bit hash value directly
        std::string str =
          std::string("element_") + std::to_string(i) + "_" + std::to_string(generateRandomHash());
        hll.add(std::move(str));
    }

    uint64_t estimated = hll.count();
    double error = std::abs(static_cast<double>(estimated) - actualCount) / actualCount;

    std::cout << "Actual cardinality: " << actualCount << std::endl;
    std::cout << "Estimated cardinality: " << estimated << std::endl;
    std::cout << "Actual error: " << std::fixed << std::setprecision(4) << error * 100 << "%"
              << std::endl;
    std::cout << "Theoretical error: " << std::fixed << std::setprecision(4)
              << hll.get_theoretical_error() * 100 << "%" << std::endl;
}

static void TestCollisionResistance()
{
    PrintSubTitle("TestCollisionResistance");

    // Test a large number of similar strings: 1 million
    cutl::HyperLogLog hll(12);
    for (int i = 0; i < 1000000; i++)
    {
        hll.add("user_" + std::to_string(i) + "_session_" + std::to_string(rand()));
    }

    std::cout << "Estimation for 1 million similar strings: " << hll.count() << std::endl;
}

void TestHyperLogLog()
{
    PrintTitle("TestHyperLogLog");

    try
    {
        TestBasicUsage();
        TestMerge();
        TestPerformance();
        TestCollisionResistance();
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
    }
}