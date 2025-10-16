#include "bloomfilter.h"

namespace cutl
{

bloom_filter::bloom_filter(size_t size, size_t hash_size)
  : size_(size)
  , hash_size_(hash_size)
  , bitmap_(std::make_shared<bitmap>(size_))
{
}

bloom_filter::bloom_filter(size_t expected_size, bloom_error_rate error_rate)
{
    size_t optimal_m = 0;
    size_t optimal_k = 0;
    double actual_fp_rate = 0.0;
    bloom_optimal_parameters(
      expected_size, (error_rate / 100.0f), optimal_m, optimal_k, actual_fp_rate);

    size_ = optimal_m;
    hash_size_ = optimal_k;
    bitmap_ = std::make_shared<bitmap>(size_);
}

/**
 * 添加元素
 */
void bloom_filter::add(const std::string& value)
{
    auto pair = hash(value);
    auto h1 = pair.first;
    auto h2 = pair.second;
    // 确保h2是奇数，提高分布性
    if (h2 & 0x1 == 0) // 等同于 h2 % 2 == 0
    {
        h2 += 1;
    }

    for (size_t i = 0; i < hash_size_; ++i)
    {
        size_t index = (h1 + i * h2) % size_;
        bitmap_->set(index);
    }
}

/**
 * 判断元素是否存在（可能有误判）
 */
bool bloom_filter::contains(const std::string& value) const
{
    auto pair = hash(value);
    auto h1 = pair.first;
    auto h2 = pair.second;
    if (h2 & 0x1 == 0) // 等同于 h2 % 2 == 0
    {
        h2 += 1;
    }

    for (size_t i = 0; i < hash_size_; ++i)
    {
        size_t index = (h1 + i * h2) % size_;
        if (!bitmap_->get(index))
        {
            return false;
        }
    }
    return true;
}

// 估算元素数量(测试函数，实际业务中不建议使用)
size_t bloom_filter::test_estimateCount() const
{
    // double m = size_;
    // double k = hash_size_;
    // double x = getSetBitCount();
    // return std::round(-(m / k) * std::log(1 - x / m));

    size_t set_bits = test_getSetBitCount();
    if (set_bits == size_)
    {
        // 所有位都被设置，估算不可靠
        return std::numeric_limits<size_t>::max();
    }

    double x = static_cast<double>(set_bits) / size_;
    return std::round(-static_cast<double>(size_) / hash_size_ * std::log(1 - x));
}

// 双重哈希函数，生成k个哈希值
std::pair<size_t, size_t> bloom_filter::hash(const std::string& str) const
{
    std::hash<std::string> hasher1;
    std::hash<std::string> hasher2;

    size_t h1 = hasher1(str);
    size_t h2 = hasher2(str + "salt"); // 加盐获得不同的哈希值

    return { h1, h2 };
}

/**
 * @brief 计算最优哈希函数数量
 *
 * @param m 位数组大小（比特数）
 * @param n 预期插入元素数量
 * @return size_t
 */
size_t bloom_optimal_k(size_t m, size_t n)
{
    return static_cast<size_t>(std::round(static_cast<double>(m) / n * std::log(2)));
}

/**
 * @brief 计算最优位数组大小
 *
 * @param n 预期插入元素数量
 * @param p 误判率
 * @return size_t
 */
size_t bloom_optimal_m(size_t n, double p)
{
    return static_cast<size_t>(std::ceil(-(n * std::log(p)) / std::pow(std::log(2), 2)));
}

/**
 * @brief 计算实际误判率
 *
 * @param m 位数组大小（比特数）
 * @param n 预期插入元素数量
 * @param k 哈希函数数量
 * @return double
 */
double bloom_false_positive_rate(size_t m, size_t n, size_t k)
{
    return std::pow(1 - std::exp(-static_cast<double>(k) * n / m), k);
}

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
                              double& actual_fp_rate)
{
    optimal_m = bloom_optimal_m(n, target_fp_rate);
    optimal_k = bloom_optimal_k(optimal_m, n);
    actual_fp_rate = bloom_false_positive_rate(optimal_m, n, optimal_k);
}

// class OptimizedBloomFilter
// {
// private:
//     BloomFilter bf_;
//     std::vector<bool> bit_array;
//     size_t m; // 位数组大小
//     size_t k; // 哈希函数数量
//     DoubleHashingStrategy hasher;

// public:
//     // 自动计算最优参数
//     OptimizedBloomFilter(size_t expected_elements, double target_false_positive_rate = 0.01)
//       : hasher(0, 0)
//     { // 临时初始化，后面重新设置

//         size_t optimal_m, optimal_k;
//         double actual_fp;

//         bloom_optimal_parameters(
//           expected_elements, target_false_positive_rate, optimal_m, optimal_k, actual_fp);
//         bf_ = BloomFilter(optimal_m, optimal_k);

//         // 实际使用中建议增加20%的冗余
//         m = static_cast<size_t>(optimal_m * 1.2);
//         k = optimal_k;
//     }

//     // 手动指定参数
//     OptimizedBloomFilter(size_t bit_array_size, size_t num_hashes)
//       : m(bit_array_size)
//       , k(num_hashes)
//       , hasher(num_hashes, bit_array_size)
//     {
//         bit_array.resize(m, false);
//     }

//     void add(const std::string& element)
//     {
//         auto hashes = hasher.get_hashes(element);
//         for (size_t hash_val : hashes)
//         {
//             bit_array[hash_val] = true;
//         }
//     }

//     bool contains(const std::string& element) const
//     {
//         auto hashes = hasher.get_hashes(element);
//         for (size_t hash_val : hashes)
//         {
//             if (!bit_array[hash_val])
//             {
//                 return false;
//             }
//         }
//         return true;
//     }

//     // 获取实际统计信息
//     void print_stats() const
//     {
//         size_t set_bits = 0;
//         for (bool bit : bit_array)
//         {
//             if (bit)
//                 set_bits++;
//         }

//         double utilization = static_cast<double>(set_bits) / m;
//         std::cout << "Set bits: " << set_bits << "/" << m << " (" << (utilization * 100) << "%)"
//                   << std::endl;
//     }
// };

} // namespace cutl