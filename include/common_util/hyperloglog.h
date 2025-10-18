#include "hash.h"
#include <cmath>
#include <cstdint>
#include <string>
#include <vector>

namespace cutl
{

class HyperLogLog
{
public:
    // 构造函数
    explicit HyperLogLog(int precision = 14);

    ~HyperLogLog() { registers_.clear(); }

public:
    // 添加uint64_t类型的元素
    void add(uint64_t value);

    // 添加字符串类型的元素
    void add(const std::string& value);

    // 估算基数（返回64位整数）
    uint64_t count() const;

    // 重置所有寄存器
    void reset() { std::fill(registers_.begin(), registers_.end(), 0); }

    // 合并另一个 HyperLogLog
    void merge(const HyperLogLog& other);

    std::string to_string() const { return this->to_hex(1); }

    void from_string(const std::string& text);

    // 获取理论误差
    double get_theoretical_error() const { return 1.04 / std::sqrt(m_); }

private:
    // 计算 alpha 常数
    double compute_alpha() const;

    // 统计前导零的数量（64位版本）
    int countLeading_zeros(uint64_t bits) const;

    std::string to_hex(int compress = 1) const;

private:
    int p_;
    int m_;
    std::vector<uint8_t> registers_;
    double alpha_;
};

} // namespace cutl
