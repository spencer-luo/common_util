#include "common.hpp"
#include "common_util/bloomfilter.h"

void TestBsetUsage()
{
    PrintSubTitle("TestBsetUsage");

    // 创建布隆过滤器：位数组大小1000，使用3个哈希函数
    cutl::bloom_filter bloom(1000, 3);

    // 添加一些元素
    bloom.add("apple");
    bloom.add("banana");
    bloom.add("orange");

    // 测试存在性
    std::cout << "Contains 'apple': " << bloom.contains("apple") << std::endl;   // 应该为true
    std::cout << "Contains 'banana': " << bloom.contains("banana") << std::endl; // 应该为true
    std::cout << "Contains 'grape': " << bloom.contains("grape")
              << std::endl; // 可能为false，或小概率为true（误判）
}

void TestBoolParam()
{
    PrintSubTitle("TestBoolParam");

    size_t optimal_m = 0;
    size_t optimal_k = 0;
    double actual_fp_rate = 0.0;
    bloom_optimal_parameters(
      512, cutl::bloom_error_rate::percent_01, optimal_m, optimal_k, actual_fp_rate);

    std::cout << "optimal_m: " << optimal_m << std::endl;
    std::cout << "optimal_k: " << optimal_k << std::endl;
    std::cout << "actual_fp_rate: " << actual_fp_rate << std::endl;
}

// 误差测试
void TestAccuracy()
{
    PrintSubTitle("TestAccuracy");

    int actualCount = 1000;
    cutl::bloom_filter bloom(actualCount, 3);

    std::cout << "Start test, add " << actualCount << " element..." << std::endl;
    // 1. 使用随机设备作为种子
    std::random_device rd;
    // 2. 使用 Mersenne Twister 引擎（高质量随机数）
    std::mt19937 gen(rd());
    // 3. 定义分布范围 [1, 10000]
    std::uniform_int_distribution<int> dist(1, 10000);
    for (size_t i = 0; i < actualCount; i++)
    {
        // 确保生成唯一值
        bloom.add("element - " + std::to_string(dist(gen)));
    }

    int estimateCount = bloom.test_estimateCount();
    // int estimateCount = bloom.estimate_cardinality_ml();
    double error = std::abs(estimateCount - actualCount) / (double)actualCount;
    std::cout << "actualCount: " << actualCount << ", estimateCount:" << estimateCount
              << std::endl;
    std::cout << "Actual error: " << std::round(error * 100) << "%" << std::endl;
    std::cout << "Theoretical error: " << std::round(bloom.test_getRelativeError() * 100) << "%"
              << std::endl;
}

void TestBloomFilter()
{
    PrintTitle("TestBloomFilter");

    TestBsetUsage();
    TestBoolParam();
    TestAccuracy();
}