#include "common.hpp"
#include "common_util/hyperloglog.h"

// 测试函数
void testUsage()
{
    std::cout << "------ 基础功能使用 ------" << std::endl;
    cutl::HyperLogLog hll(15);
    hll.add("abc");
    hll.add("def");
    // hll.add("ghijk");
    hll.add("123");
    std::cout << "count1: " << hll.count() << std::endl;
    hll.add("abc");
    std::cout << "count2: " << hll.count() << std::endl;
}

void testMerge()
{
    std::cout << "------ merge功能演示 ------" << std::endl;
    cutl::HyperLogLog hll_01(14);
    cutl::HyperLogLog hll_02(14);

    for (int i = 0; i < 1000; i++)
    {
        hll_01.add("element-" + std::to_string(i) + "-" + std::to_string(rand()));
        hll_02.add("element-" + std::to_string(i) + "-" + std::to_string(rand()));
    }

    std::cout << "hll_01 实际值: 1000, 估算值: " << hll_01.count() << std::endl;
    std::cout << "hll_02 实际值: 1000, 估算值: " << hll_02.count() << std::endl;
    hll_01.merge(hll_02);
    std::cout << "合并后 hll_01 估算值: " << hll_01.count() << std::endl;
}

// 生成64位随机哈希（用于测试）
uint64_t generateRandomHash()
{
    static std::random_device rd;
    static std::mt19937_64 gen(rd());
    static std::uniform_int_distribution<uint64_t> dis(1, 10000000);
    return dis(gen);
}

// 测试函数
void test64BitPerformance()
{
    std::cout << "====== 64位 cutl::HyperLogLog 性能测试 ======" << std::endl;

    cutl::HyperLogLog hll(14);
    const uint64_t actualCount = 10000; // 1000万

    std::cout << "测试数据量: " << actualCount << std::endl;
    std::cout << "哈希空间: 2^64 = " << ((1ULL << 63) * 2 - 1) << std::endl;

    auto start = std::chrono::high_resolution_clock::now();

    for (uint64_t i = 0; i < actualCount; i++)
    {
        // 使用64位哈希值直接添加
        std::string str =
          std::string("element_") + std::to_string(i) + "_" + std::to_string(generateRandomHash());
        hll.add(std::move(str));
    }

    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    uint64_t estimated = hll.count();
    double error = std::abs(static_cast<double>(estimated) - actualCount) / actualCount;

    std::cout << "实际基数: " << actualCount << std::endl;
    std::cout << "估计基数: " << estimated << std::endl;
    std::cout << "相对误差: " << std::fixed << std::setprecision(4) << error * 100 << "%"
              << std::endl;
    std::cout << "理论误差: " << std::fixed << std::setprecision(4)
              << hll.get_theoretical_error() * 100 << "%" << std::endl;
    std::cout << "处理时间: " << duration.count() << "ms" << std::endl;
}

void testCollisionResistance()
{
    std::cout << "\n====== 碰撞抵抗测试 ======" << std::endl;

    cutl::HyperLogLog hll(12);

    // 测试大量相似字符串:100万
    for (int i = 0; i < 1000000; i++)
    {
        hll.add("user_" + std::to_string(i) + "_session_" + std::to_string(rand()));
    }

    std::cout << "100万相似字符串估计: " << hll.count() << std::endl;
}

void TestHyperLogLog()
{
    try
    {
        testUsage();
        testMerge();
        test64BitPerformance();
        testCollisionResistance();
    }
    catch (const std::exception& e)
    {
        std::cerr << "错误: " << e.what() << std::endl;
    }
}