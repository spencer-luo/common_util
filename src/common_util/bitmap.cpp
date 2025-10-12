#include "bitmap.h"
#include "inner/logger.h"
#include "strfmt.h"
#include <algorithm>
#include <cmath>
#include <stdexcept>
// #include <bitset>
// #include <iostream>

namespace cutl
{

bitmap::bitmap(size_t size)
  : size_(size)
{
    // 计算需要的字节数，向上取整: size/8
    // 注意：这里必须除以8.0，才能获取浮点数然后向上取整
    bits_.resize(std::ceil(size / 8.0), 0);
}

/**
 * 设置指定位置为 1
 */
void bitmap::set(size_t position)
{
    if (position >= size_)
    {
        throw std::out_of_range("Position " + std::to_string(position) + " out of range");
    }

    size_t byteIndex = position >> 3; // 等价于 position / 8
    size_t bitIndex = position & 0x7; // 等价于 position % 8
    bits_[byteIndex] |= (1 << bitIndex);
}

/**
 * 获取指定位置的值
 */
bool bitmap::get(size_t position) const
{
    if (position >= size_)
    {
        throw std::out_of_range("Position " + std::to_string(position) + " out of range");
    }
    size_t byteIndex = position >> 3; // 等价于 position / 8
    size_t bitIndex = position & 0x7; // 等价于 position % 8
    return (bits_[byteIndex] & (1 << bitIndex)) != 0;
}

/**
 * 设置指定位置为 0
 */
void bitmap::reset(size_t position)
{
    if (position >= size_)
    {
        throw std::out_of_range("Position " + std::to_string(position) + " out of range");
    }
    size_t byteIndex = position >> 3;     // 等价于 position / 8
    size_t bitIndex = position & 0x7;     // 等价于 position % 8
    bits_[byteIndex] &= ~(1 << bitIndex); // 对应的bit设置为0
}

// 将所有元素重置为0
void bitmap::reset()
{
    std::fill(bits_.begin(), bits_.end(), 0);
}

/**
 * @brief 获取数值为1的位数
 *
 * @return size_t
 */
size_t bitmap::count() const
{
    size_t count = 0;
    for (size_t i = 0; i < bits_.size(); i++)
    {
        uint8_t byte = bits_[i] & 0xff;
        for (size_t j = 0; j < 8; j++)
        {
            // 方法一
            if ((byte & (1 << j)) != 0)
            {
                count++;
            }
            // 方法二
            // if (((byte >>> j) & 0x01) == 1)
            // {
            //     count ++;
            // }
        }
    }
    return count;
}

std::string bitmap::to_hex(int compress) const
{
    if (compress == 0)
    {
        return cutl::to_hex(bits_.data(), size_, true, 0);
    }
    else
    {
        // 找到最后一个非零字节的索引
        int lastNonZeroIndex = 0;
        for (int i = bits_.size() - 1; i >= 0; i--)
        {
            if (bits_[i] != 0)
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
        return cutl::to_hex(bits_.data(), lastNonZeroIndex + 1, true, 0);
    }
}

std::string bitmap::to_string() const
{
    return this->to_hex(1);
}

void bitmap::from_string(const std::string& text)
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
    const size_t expectedLength = bits_.size() * 2;
    size_t strLen = std::min(expectedLength, text.length());
    size_t byteSize = strLen >> 1; // strLen / 2
    // 将十六进制字符串转换为字节数组
    for (size_t i = 0; i < byteSize; i++)
    {
        std::string hexByte = text.substr(i * 2, 2);
        bits_[i] = std::stoi(hexByte, nullptr, 16);
    }
}

std::vector<size_t> bitmap::valuelist() const
{
    std::vector<size_t> result;
    for (size_t i = 0; i < bits_.size(); i++)
    {
        uint8_t byte = bits_[i];
        if (byte == 0)
        {
            continue;
        }

        size_t base = i << 3; // i * 8
        for (size_t j = 0; j < 8; j++)
        {
            if ((byte & (1 << j)) != 0)
            {
                result.emplace_back(base + j);
            }
        }
    }
    return result;
}

bool bitmap::equals(const ibitmap& other) const
{
    const bitmap* other_bitmap = dynamic_cast<const bitmap*>(&other);
    if (!other_bitmap || size_ != other_bitmap->size_)
    {
        return false;
    }

    for (size_t i = 0; i < bits_.size(); i++)
    {
        if (bits_[i] != other_bitmap->bits_[i])
            return false;
    }
    return true;
}

/**
 * 与另一个 bitmap 进行 AND 操作
 */
bitmap bitmap::operator&(const bitmap& other) const
{
    auto minSize = std::min(bits_.size(), other.bits_.size());
    bitmap result(minSize << 3); // minSize * 8
    for (size_t i = 0; i < minSize; i++)
    {
        result.bits_[i] = bits_[i] & other.bits_[i];
    }
    return result;
}

// #include <iostream>

/**
 * 与另一个 bitmap 进行 OR 操作
 */
bitmap bitmap::operator|(const bitmap& other) const
{
    auto minSize = std::min(bits_.size(), other.bits_.size());
    auto maxSize = std::max(bits_.size(), other.bits_.size());

    bitmap result(maxSize << 3); // maxSize * 8
    for (size_t i = 0; i < minSize; i++)
    {
        result.bits_[i] = bits_[i] | other.bits_[i];
    }
    for (size_t i = minSize; i < maxSize; i++)
    {
        result.bits_[i] = bits_.size() > other.bits_.size() ? bits_[i] : other.bits_[i];
    }
    return result;
}

// 按位取反
bitmap bitmap::operator~() const
{
    bitmap result(size_);
    for (size_t i = 0; i < bits_.size(); i++)
    {
        // uint8_t类型本身可以进行按位取反操作
        result.bits_[i] = ~bits_[i];
    }
    return result;
}

// 按位异或
bitmap bitmap::operator^(const bitmap& other) const
{
    if (size_ != other.size_)
    {
        throw std::invalid_argument("Bitmaps must have same size");
    }
    bitmap result(size_);
    for (size_t i = 0; i < bits_.size(); i++)
    {
        result.bits_[i] = bits_[i] ^ other.bits_[i];
    }
    return result;
}

bitmap& bitmap::operator&=(const bitmap& other)
{
    if (size_ != other.size_)
    {
        throw std::invalid_argument("Bitmaps must have same size");
    }
    for (size_t i = 0; i < bits_.size(); i++)
    {
        bits_[i] = bits_[i] & other.bits_[i];
    }
    return *this;
}

bitmap& bitmap::operator|=(const bitmap& other)
{
    if (size_ != other.size_)
    {
        throw std::invalid_argument("Bitmaps must have same size");
    }
    for (size_t i = 0; i < bits_.size(); i++)
    {
        bits_[i] = bits_[i] | other.bits_[i];
    }
    return *this;
}

bitmap& bitmap::operator^=(const bitmap& other)
{
    if (size_ != other.size_)
    {
        throw std::invalid_argument("Bitmaps must have same size");
    }
    for (size_t i = 0; i < bits_.size(); i++)
    {
        bits_[i] = bits_[i] ^ other.bits_[i];
    }
    return *this;
}

dynamic_bitmap::dynamic_bitmap(size_t init_size)
  : bitmap(init_size)
{
}

/**
 * 设置指定位置为 1
 */
void dynamic_bitmap::set(size_t position)
{
    ensureCapacity(position + 1);

    size_t byteIndex = position >> 3; // 等价于 position / 8
    size_t bitIndex = position & 0x7; // 等价于 position % 8
    bits_[byteIndex] |= (1 << bitIndex);
}

/**
 * 动态扩容
 */
void dynamic_bitmap::ensureCapacity(size_t minSize)
{
    if (minSize <= size_)
        return;

    size_ = std::max(size_ * 2, minSize);
    bits_.resize(size_);
}

roaring_bitmap::roaring_bitmap(size_t blockSize)
  : block_size_(blockSize)
{
}

/**
 * 设置指定位置为 1
 */
void roaring_bitmap::set(size_t position)
{
    size_t key = position / block_size_;
    size_t bitPosition = position % block_size_;

    if (!container_.count(key))
    {
        // block不存在则添加新的block
        container_.emplace(key, bitmap(block_size_));
    }
    container_.at(key).set(bitPosition);
}

/**
 * 获取指定位置的值
 */
bool roaring_bitmap::get(size_t position) const
{
    size_t key = position / block_size_;
    size_t bitPosition = position % block_size_;

    auto itr = container_.find(key);
    if (itr == container_.end())
    {
        return false;
    }
    return itr->second.get(bitPosition);
}

/**
 * 设置指定位置为 0
 */
void roaring_bitmap::reset(size_t position)
{
    size_t key = position / block_size_;
    size_t bitPosition = position % block_size_;

    auto itr = container_.find(key);
    if (itr == container_.end())
    {
        throw std::out_of_range("Position " + std::to_string(position) + " not in container");
    }

    itr->second.reset(bitPosition);
}

// 将所有元素重置为0
void roaring_bitmap::reset()
{
    for (auto itr = container_.begin(); itr != container_.end(); itr++)
    {
        itr->second.reset();
    }
}

/**
 * @brief 获取数值为1的位数
 *
 * @return size_t
 */
size_t roaring_bitmap::count() const
{
    size_t count = 0;
    for (auto itr = container_.begin(); itr != container_.end(); itr++)
    {
        count += itr->second.count();
    }
    return count;
}

/**
 * 获取 bitmap 的大小（bit 数）
 */
size_t roaring_bitmap::size() const
{
    size_t size = 0;
    for (auto itr = container_.begin(); itr != container_.end(); itr++)
    {
        size += itr->second.size();
    }
    return size;
}

std::string roaring_bitmap::to_string() const
{
    std::vector<size_t> keys;
    for (const auto& pair : container_)
    {
        keys.emplace_back(pair.first);
    }
    std::sort(keys.begin(), keys.end());

    std::string result("{");
    for (size_t i = 0; i < keys.size(); i++)
    {
        auto& key = keys[i];
        auto& val = container_.at(key);
        result += "\"" + std::to_string(key) + "\":\"" + val.to_string() + "\"";
        if (i != keys.size() - 1)
        {
            result += ",";
        }
    }
    result += "}";
    return result;
}

void roaring_bitmap::from_string(const std::string& text)
{
    // 清空现有数据
    container_.clear();

    // 简单校验格式（{...}结构）
    if (text.empty() || text.front() != '{' || text.back() != '}')
    {
        throw std::runtime_error("Invalid roaring bitmap string format");
    }

    // 提取中间内容
    std::string content = text.substr(1, text.size() - 2);
    if (content.empty())
    {
        return; // 空 bitmap
    }

    // 分割键值对
    size_t pos = 0;
    while (pos < content.size())
    {
        // 查找键的引号
        size_t keyStart = content.find('"', pos);
        if (keyStart == std::string::npos)
            break;

        size_t keyEnd = content.find('"', keyStart + 1);
        if (keyEnd == std::string::npos)
            break;

        // 解析键（block索引）
        std::string keyStr = content.substr(keyStart + 1, keyEnd - keyStart - 1);
        size_t key = std::stoull(keyStr);

        // 查找值的引号
        size_t valStart = content.find('"', keyEnd + 1);
        if (valStart == std::string::npos)
            break;

        size_t valEnd = content.find('"', valStart + 1);
        if (valEnd == std::string::npos)
            break;

        // 解析值（bitmap的十六进制字符串）
        std::string valStr = content.substr(valStart + 1, valEnd - valStart - 1);

        // 创建对应block并从字符串加载数据
        bitmap block(block_size_);
        block.from_string(valStr);
        container_.emplace(key, block);

        // 移动到下一个键值对
        pos = valEnd + 1;
        if (pos < content.size() && content[pos] == ',')
        {
            pos++;
        }
    }
}

std::vector<size_t> roaring_bitmap::valuelist() const
{
    std::vector<size_t> keys;
    for (const auto& pair : container_)
    {
        keys.emplace_back(pair.first);
    }
    std::sort(keys.begin(), keys.end());

    std::vector<size_t> result;
    for (const auto& key : keys)
    {
        auto vec = container_.at(key).valuelist();
        result.insert(result.end(), vec.begin(), vec.end());
    }
    return result;
}

/**
 * 判断是否相等
 */
bool roaring_bitmap::equals(const ibitmap& other) const
{
    auto otherPtr = dynamic_cast<const roaring_bitmap*>(&other);
    if (block_size() != otherPtr->block_size())
    {
        return false;
    }
    if (container_.size() != otherPtr->container_.size())
    {
        return false;
    }
    for (auto itr = container_.begin(); itr != container_.end(); itr++)
    {
        auto& key = itr->first;
        auto& val = itr->second;
        if (!otherPtr->container_.count(key))
        {
            return false;
        }
        if (val != otherPtr->container_.at(key))
        {
            return false;
        }
    }
    return true;
}

/**
 * 与另一个 bitmap 进行 AND 操作
 */
roaring_bitmap roaring_bitmap::operator&(const roaring_bitmap& other) const
{
    if (block_size() != other.block_size())
    {
        throw std::invalid_argument("RoaringBitmap must have same block_size");
    }
    roaring_bitmap rBitmap(block_size_);
    for (auto itr = container_.begin(); itr != container_.end(); itr++)
    {
        auto& key = itr->first;
        auto& val = itr->second;

        if (other.container_.count(key))
        {
            bitmap result = val & other.container_.at(key);
            rBitmap.container_.emplace(key, result);
        }
    }

    return rBitmap;
}

/**
 * 与另一个 bitmap 进行 OR 操作
 */
roaring_bitmap roaring_bitmap::operator|(const roaring_bitmap& other) const
{
    if (block_size() != other.block_size())
    {
        throw std::invalid_argument("RoaringBitmap must have same block_size");
    }

    roaring_bitmap rBitmap(block_size_);
    for (auto itr = container_.begin(); itr != container_.end(); itr++)
    {
        auto& key = itr->first;
        auto& val = itr->second;

        if (other.container_.count(key))
        {
            bitmap result = val | other.container_.at(key);
            rBitmap.container_.emplace(key, result);
        }
        else
        {
            rBitmap.container_.emplace(key, val);
        }
    }

    for (auto itr = other.container_.begin(); itr != other.container_.end(); itr++)
    {
        auto& key = itr->first;
        auto& val = itr->second;
        if (!rBitmap.container_.count(key))
        {
            rBitmap.container_.emplace(key, val);
        }
    }

    return rBitmap;
}

/**
 * 与另一个 bitmap 进行 Not 操作(按位取反)
 */
roaring_bitmap roaring_bitmap::operator~() const
{
    roaring_bitmap rBitmap(block_size_);
    for (auto itr = container_.begin(); itr != container_.end(); itr++)
    {
        auto& key = itr->first;
        auto& val = itr->second;

        rBitmap.container_.emplace(key, ~val);
    }
    return rBitmap;
}

/**
 * 与另一个 bitmap 进行 异或 操作
 */
roaring_bitmap roaring_bitmap::operator^(const roaring_bitmap& other) const
{
    if (block_size() != other.block_size())
    {
        throw std::invalid_argument("RoaringBitmap must have same block_size");
    }
    roaring_bitmap rBitmap(block_size_);
    for (auto itr = container_.begin(); itr != container_.end(); itr++)
    {
        auto& key = itr->first;
        auto& val = itr->second;

        if (other.container_.count(key))
        {
            bitmap result = val ^ other.container_.at(key);
            // std::cout << "key:" << key << ", result: " << result.to_string() << std::endl;
            rBitmap.container_.emplace(key, result);
        }
        else
        {
            throw std::invalid_argument("Key " + std::to_string(key) + " not in other container");
        }
    }

    return rBitmap;
}

// 与
roaring_bitmap& roaring_bitmap::operator&=(const roaring_bitmap& other)
{
    if (block_size() != other.block_size())
    {
        throw std::invalid_argument("RoaringBitmap must have same block_size");
    }

    for (auto itr = container_.begin(); itr != container_.end(); itr++)
    {
        auto& key = itr->first;
        auto& val = itr->second;

        if (other.container_.count(key))
        {
            val &= other.container_.at(key);
        }
        else
        {
            std::string errMsg = "Key " + std::to_string(key) + " not in other container.";
            throw std::invalid_argument(errMsg);
        }
    }

    return *this;
}

// 或
roaring_bitmap& roaring_bitmap::operator|=(const roaring_bitmap& other)
{
    if (block_size() != other.block_size())
    {
        throw std::invalid_argument("RoaringBitmap must have same block_size");
    }

    for (auto itr = container_.begin(); itr != container_.end(); itr++)
    {
        auto& key = itr->first;
        auto& val = itr->second;

        if (other.container_.count(key))
        {
            val |= other.container_.at(key);
        }
        else
        {
            std::string errMsg = "Key " + std::to_string(key) + " not in other container.";
            throw std::invalid_argument(errMsg);
        }
    }

    return *this;
}

// 异或
roaring_bitmap& roaring_bitmap::operator^=(const roaring_bitmap& other)
{
    if (block_size() != other.block_size())
    {
        throw std::invalid_argument("RoaringBitmap must have same block_size");
    }

    for (auto itr = container_.begin(); itr != container_.end(); itr++)
    {
        auto& key = itr->first;
        auto& val = itr->second;

        if (other.container_.count(key))
        {
            val ^= other.container_.at(key);
        }
        else
        {
            std::string errMsg = "Key " + std::to_string(key) + " not in other container.";
            throw std::invalid_argument(errMsg);
        }
    }

    return *this;
}

} // namespace cutl