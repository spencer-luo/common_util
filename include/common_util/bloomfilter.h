#pragma once

#include "bitmap.h"
#include <functional>
#include <memory>
#include <random>
#include <string>
#include <vector>

namespace cutl
{

// 误判率
enum bloom_error_rate
{
    percent_01 = 1,
    percent_10 = 10,
    percent_20 = 20,
    percent_30 = 30,
    percent_40 = 40,
    percent_50 = 50,
};

class bloom_filter
{
public:
    bloom_filter(size_t size, size_t hash_size);

    bloom_filter(size_t expected_size, bloom_error_rate error_rate = bloom_error_rate::percent_01);

    ~bloom_filter() = default;

public:
    /**
     * 添加元素
     */
    void add(const std::string& value);

    /**
     * 判断元素是否存在（可能有误判）
     */
    bool contains(const std::string& value) const;

    /**
     * 清空布隆过滤器
     */
    void clear() { bitmap_->reset(); }

    // /**
    //  * 获取位图中设置位的数量(测试函数，实际业务中不建议使用)
    //  */
    // size_t test_getSetBitCount() const;

    // // 估算元素数量(测试函数，实际业务中不建议使用)
    // size_t test_estimateCount() const;

    // // 获取相对误差(测试函数，实际业务中不建议使用)
    // double test_getRelativeError();

private:
    // 双重哈希函数，生成k个哈希值
    std::pair<size_t, size_t> hash(const std::string& str) const;

private:
    size_t size_;
    std::shared_ptr<bitmap> bitmap_;
    size_t hash_size_;
};

/**
 * @brief 计算最优哈希函数数量
 *
 * @param m 位数组大小（比特数）
 * @param n 预期插入元素数量
 * @return size_t
 */
size_t bloom_optimal_k(size_t m, size_t n);

/**
 * @brief 计算最优位数组大小
 *
 * @param n 预期插入元素数量
 * @param p 误判率
 * @return size_t
 */
size_t bloom_optimal_m(size_t n, double p);

/**
 * @brief 计算实际误判率
 *
 * @param m 位数组大小（比特数）
 * @param n 预期插入元素数量
 * @param k 哈希函数数量
 * @return double
 */
double bloom_false_positive_rate(size_t m, size_t n, size_t k);

/**
 * @brief 综合计算所有最优参数
 *
 * @param n 预期的元素数量
 * @param target_fp_rate 目标误判率
 * @param optimal_m [out] 最优位数组大小
 * @param optimal_k [out] 最优哈西函数数量
 * @param actual_fp_rate [out] 实际误判率
 */
void bloom_optimal_parameters(size_t n,
                              double target_fp_rate,
                              size_t& optimal_m,
                              size_t& optimal_k,
                              double& actual_fp_rate);

} // namespace cutl