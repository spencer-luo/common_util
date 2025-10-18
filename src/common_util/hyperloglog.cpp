#include "hyperloglog.h"
#include "hash.h"
#include "strfmt.h"
#include <algorithm>
#include <stdexcept>

namespace cutl
{

// 构造函数
HyperLogLog::HyperLogLog(int precision)
  : p_(precision)
  , m_(1 << precision)
  , registers_(m_, 0)
{
    if (p_ < 4 || p_ > 20)
    {
        throw std::invalid_argument("Precision must be between 4 and 20");
    }
    alpha_ = compute_alpha();
}

// 添加uint64_t类型的元素
void HyperLogLog::add(uint64_t value)
{
    // 前 p 位作为桶索引
    uint64_t index = value >> (64 - p_);

    // 后 (64-p) 位统计前导零
    uint64_t bits = value & ((1ULL << (64 - p_)) - 1);
    int leadingZeros = countLeading_zeros(bits) + 1;

    if (leadingZeros > registers_[index])
    {
        registers_[index] = static_cast<uint8_t>(leadingZeros);
    }
}

// 添加字符串类型的元素
void HyperLogLog::add(const std::string& value)
{
    uint64_t hashValue = cutl::hash_fnv1a_64(value);
    add(hashValue);
}

// 估算基数（返回64位整数）
uint64_t HyperLogLog::count() const
{
    double sum = 0.0;
    int zeroCount = 0;

    for (int i = 0; i < m_; i++)
    {
        if (registers_[i] == 0)
        {
            zeroCount++;
        }
        sum += std::pow(2.0, -static_cast<double>(registers_[i]));
    }

    // 基础估算
    double estimate = (alpha_ * m_ * m_) / sum;

    // 小范围数值 修正阈值
    const uint64_t SMALL_CORRECTION_THRESHOLD = 2.5 * m_;
    // 大范围数值 修正阈值
    const uint64_t LARGE_CORRECTION_THRESHOLD = (1ULL << 32);
    // 64位版本的修正策略
    if (estimate <= SMALL_CORRECTION_THRESHOLD)
    {
        if (zeroCount > 0)
        {
            estimate = m_ * std::log(static_cast<double>(m_) / zeroCount);
        }
    }
    else if (estimate > LARGE_CORRECTION_THRESHOLD)
    {
        estimate =
          -LARGE_CORRECTION_THRESHOLD * std::log(1.0 - estimate / LARGE_CORRECTION_THRESHOLD);
    }

    return static_cast<uint64_t>(std::round(estimate));
}

// 合并另一个 HyperLogLog
void HyperLogLog::merge(const HyperLogLog& other)
{
    if (m_ != other.m_)
    {
        throw std::invalid_argument("Precision mismatch");
    }

    for (int i = 0; i < m_; i++)
    {
        if (other.registers_[i] > registers_[i])
        {
            registers_[i] = other.registers_[i];
        }
    }
}

void HyperLogLog::from_string(const std::string& text)
{
    for (char c : text)
    {
        if (!isxdigit(c))
        {
            throw std::runtime_error("Invalid hexadecimal string");
        }
    }

    // 先将bitmap所有数据重置成0
    this->reset();

    // 每两个字符表示一个字节
    const size_t expectedLength = registers_.size() * 2;
    size_t strLen = std::min(expectedLength, text.length());
    size_t byteSize = strLen >> 1; // strLen / 2
    // 将十六进制字符串转换为字节数组
    for (size_t i = 0; i < byteSize; i++)
    {
        std::string hexByte = text.substr(i * 2, 2);
        registers_[i] = std::stoi(hexByte, nullptr, 16);
    }
}

// 计算 alpha 常数
double HyperLogLog::compute_alpha() const
{
    switch (m_)
    {
        case 16:
            return 0.673;
        case 32:
            return 0.697;
        case 64:
            return 0.709;
        default:
            return 0.7213 / (1.0 + 1.079 / m_);
    }
}

// 统计前导零的数量（64位版本）
int HyperLogLog::countLeading_zeros(uint64_t bits) const
{
    if (bits == 0)
        return 64 - p_;

    const int totalBits = 64 - p_;
    int position = totalBits - 1;

    while (position >= 0 && (bits & (1ULL << position)) == 0)
    {
        position--;
    }

    return totalBits - (position + 1);
}

std::string HyperLogLog::to_hex(int compress) const
{
    if (compress == 0)
    {
        return cutl::to_hex(registers_.data(), this->m_, true, 0);
    }
    else
    {
        // 找到最后一个非零字节的索引
        int lastNonZeroIndex = 0;
        for (int i = registers_.size() - 1; i >= 0; i--)
        {
            if (registers_[i] != 0)
            {
                lastNonZeroIndex = i;
                break;
            }
        }

        // 如果所有字节都是零，返回空字符串
        if (lastNonZeroIndex == 0)
        {
            return "";
        }

        // 只序列化到最后一个非零字节
        return cutl::to_hex(registers_.data(), lastNonZeroIndex + 1, true, 0);
    }
}

} // namespace cutl
