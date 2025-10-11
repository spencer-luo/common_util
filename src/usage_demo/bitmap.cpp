#include <cmath>
#include <cstdint>
#include <memory>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

// ibitmap 统一接口类
class ibitmap
{
public:
    virtual ~ibitmap() = default;

public:
    // 基本操作
    virtual void set(size_t position) = 0;
    virtual bool get(size_t position) const = 0;
    virtual void reset(size_t position) = 0;
    virtual size_t count() const = 0;
    virtual size_t size() const = 0;

    // 操作符重载
    virtual bool operator[](size_t position) const = 0;
    // 比较操作
    virtual bool equals(const ibitmap& other) const = 0;

    // // 工具方法
    // virtual void clear()
    // {
    //     for (size_t i = 0; i < size(); ++i)
    //     {
    //         if (get(i))
    //         {
    //             reset(i);
    //         }
    //     }
    // }

    virtual bool empty() const { return count() == 0; }
};

class bitmap : public ibitmap
{
protected:
    std::vector<uint8_t> bits_;
    size_t size_;

public:
    bitmap(size_t size)
      : size_(size)
    {
        // 计算需要的字节数，向上取整: size/8
        // 注意：这里必须除以8.0，才能获取浮点数然后向上取整
        bits_.resize(std::ceil(size / 8.0), 0);
    }

public:
    /**
     * 设置指定位置为 1
     */
    void set(size_t position) override
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
    bool get(size_t position) const override
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
     * 重载 [] 操作符
     */
    bool operator[](size_t position) const override { return get(position); }

    /**
     * 设置指定位置为 0
     */
    void reset(size_t position) override
    {
        if (position >= size_)
        {
            throw std::out_of_range("Position " + std::to_string(position) + " out of range");
        }
        size_t byteIndex = position >> 3;     // 等价于 position / 8
        size_t bitIndex = position & 0x7;     // 等价于 position % 8
        bits_[byteIndex] &= ~(1 << bitIndex); // 对应的bit设置为0
    }

    /**
     * @brief 获取数值为1的位数
     *
     * @return size_t
     */
    size_t count() const override
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

    /**
     * 获取 bitmap 的大小（bit 数）
     */
    size_t size() const override { return size_; }

    bool equals(const ibitmap& other) const override
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
     * 判断是否相等
     */
    bool operator==(const bitmap& other) const { return equals(other); }

    /**
     * 判断是否不相等
     */
    bool operator!=(const bitmap& other) const { return !equals(other); }

    /**
     * 与另一个 bitmap 进行 AND 操作
     */
    bitmap operator&(const bitmap& other) const
    {
        auto minSize = std::min(size_, other.size_);
        bitmap result(minSize);
        for (size_t i = 0; i < minSize; i++)
        {
            result.bits_[i] = bits_[i] & other.bits_[i];
        }
        return result;
    }

    /**
     * 与另一个 bitmap 进行 OR 操作
     */
    bitmap operator|(const bitmap& other) const
    {
        auto minSize = std::min(size_, other.size_);
        auto maxSize = std::max(size_, other.size_);

        bitmap result(maxSize);
        for (size_t i = 0; i < minSize; i++)
        {
            result.bits_[i] = bits_[i] | other.bits_[i];
        }
        for (size_t i = minSize; i < maxSize; i++)
        {
            result.bits_[i] = size_ > other.size_ ? bits_[i] : other.bits_[i];
        }
        return result;
    }

    // 按位取反
    bitmap operator~() const
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
    bitmap operator^(const bitmap& other)
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

    bitmap& operator&=(const bitmap& other)
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

    bitmap& operator|=(const bitmap& other)
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

    bitmap& operator^=(const bitmap& other)
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
};

class dynamic_bitmap : public bitmap
{
public:
    dynamic_bitmap(size_t init_size = 64)
      : bitmap(init_size)
    {
    }

    ~dynamic_bitmap() = default;

public:
    /**
     * 设置指定位置为 1
     */
    void set(size_t position) override
    {
        ensureCapacity(position + 1);

        size_t byteIndex = position >> 3; // 等价于 position / 8
        size_t bitIndex = position & 0x7; // 等价于 position % 8
        bits_[byteIndex] |= (1 << bitIndex);
    }

private:
    /**
     * 动态扩容
     */
    void ensureCapacity(size_t minSize)
    {
        if (minSize <= size_)
            return;

        size_ = std::max(size_ * 2, minSize);
        bits_.resize(size_);
    }
};

class roaring_bitmap : public ibitmap
{
private:
    size_t block_size_{ 0 };
    std::unordered_map<size_t, bitmap> container_;

public:
    roaring_bitmap(size_t blockSize)
      : block_size_(blockSize)
    {
    }

    // ~roaring_bitmap() = default;

public:
    size_t block_size() const { return block_size_; }

    /**
     * 设置指定位置为 1
     */
    void set(size_t position) override
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
    bool get(size_t position) const override
    {
        size_t key = position / block_size_;
        size_t bitPosition = position % block_size_;

        auto itr = container_.find(key);
        if (itr != container_.end())
        {
            return false;
        }
        return itr->second.get(bitPosition);
    }

    /**
     * 重载 [] 操作符
     */
    bool operator[](size_t position) const override { return get(position); }

    /**
     * 设置指定位置为 0
     */
    void reset(size_t position) override
    {
        size_t key = position / block_size_;
        size_t bitPosition = position % block_size_;

        auto itr = container_.find(key);
        if (itr != container_.end())
        {
            throw std::out_of_range("Position " + std::to_string(position) + " not in container");
        }

        itr->second.reset(position);
    }

    /**
     * @brief 获取数值为1的位数
     *
     * @return size_t
     */
    size_t count() const override
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
    size_t size() const override
    {
        size_t size = 0;
        for (auto itr = container_.begin(); itr != container_.end(); itr++)
        {
            size += itr->second.size();
        }
        return size;
    }

    /**
     * 判断是否相等
     */
    // bool operator==(const roaring_bitmap& other) const
    bool equals(const ibitmap& other) const override
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

    bool operator==(const roaring_bitmap& other) const { return equals(other); }
    bool operator!=(const roaring_bitmap& other) const { return !equals(other); }

    // // 按位取反
    // roaring_bitmap operator~(const roaring_bitmap& other) const
    // {
    //     // TODO
    // }

    /**
     * 与另一个 bitmap 进行 AND 操作
     */
    roaring_bitmap operator&(const roaring_bitmap& other) const
    {
        if (block_size() != other.block_size())
        {
            throw std::invalid_argument("RoaringBitmap must have same block_size");
        }
        if (container_.size() != other.container_.size())
        {
            throw std::invalid_argument("RoaringBitmap must have same size");
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
    roaring_bitmap operator|(const roaring_bitmap& other) const
    {
        if (block_size() != other.block_size())
        {
            throw std::invalid_argument("RoaringBitmap must have same block_size");
        }
        if (container_.size() != other.container_.size())
        {
            throw std::invalid_argument("RoaringBitmap must have same size");
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
};

#include <iostream>

void test_bitmap()
{
    std::cout << "=== 基础 bitmap 示例 ===" << std::endl;
    bitmap bitmap1(100);
    bitmap1.set(10);
    bitmap1.set(20);
    bitmap1.set(99);

    std::cout << "位置 10: " << bitmap1.get(10) << std::endl;
    std::cout << "位置 10: " << bitmap1.get(10) << std::endl;
    std::cout << "位置 25: " << bitmap1[25] << std::endl;
    std::cout << "元素数量: " << bitmap1.count() << std::endl;
    bitmap1.set(99); // 添加重复元素，数量保持不变
    std::cout << "元素数量: " << bitmap1.count() << std::endl;
    bitmap1.reset(99); // 重置位置20的只为0，数量减1
    std::cout << "元素数量: " << bitmap1.count() << std::endl;

    // 逻辑位运算
    bitmap bitmap2(100);
    bitmap2.set(20);
    bitmap2.set(30);
    bitmap2.set(40);
    std::cout << "bitmap2 size: " << bitmap2.size() << ", count: " << bitmap2.count() << std::endl;
    std::cout << "bitmap1 == bitmap2: " << (bitmap1 == bitmap2) << std::endl;

    auto andResult = bitmap1 & bitmap2;
    std::cout << "bitmap1 & bitmap2 : " << andResult.count() << std::endl;
    auto orResult = bitmap1 | bitmap2;
    std::cout << "bitmap1 | bitmap2 : " << orResult.count() << std::endl;
}

void test_dynamic_bitmap()
{
    std::cout << "\n=== 动态 bitmap 示例 ===" << std::endl;
    dynamic_bitmap dynamicBitmap;
    dynamicBitmap.set(20);
    dynamicBitmap.set(30);
    std::cout << "size: " << dynamicBitmap.size() << ", count: " << dynamicBitmap.count()
              << std::endl;
    dynamicBitmap.set(80); // 自动扩容
    std::cout << "size: " << dynamicBitmap.size() << ", count: " << dynamicBitmap.count()
              << std::endl;
}

void test_roaring_bitmap()
{
    std::cout << "\n=== RoaringBitmap 示例 ===" << std::endl;
    roaring_bitmap bitmap1(64);
    bitmap1.set(10);
    bitmap1.set(20);
    std::cout << "bitmap1 size: " << bitmap1.size() << ", count: " << bitmap1.count() << std::endl;
    bitmap1.set(80);
    std::cout << "bitmap1 size: " << bitmap1.size() << ", count: " << bitmap1.count() << std::endl;

    roaring_bitmap bitmap2(64);
    bitmap2.set(20);
    bitmap2.set(40);
    bitmap2.set(130);
    std::cout << "bitmap2 size: " << bitmap2.size() << ", count: " << bitmap2.count() << std::endl;
    std::cout << "bitmap1 == bitmap2: " << (bitmap1 == bitmap2) << std::endl;

    roaring_bitmap andBitmap = bitmap1 & bitmap2;
    std::cout << "andBitmap size: " << andBitmap.size() << ", count: " << andBitmap.count()
              << std::endl;
    roaring_bitmap orBitmap = bitmap1 | bitmap2;
    std::cout << "orBitmap size: " << orBitmap.size() << ", count: " << orBitmap.count()
              << std::endl;
}

void bitmapExamples()
{
    test_bitmap();
    test_dynamic_bitmap();
    test_roaring_bitmap();
}

int main()
{
    bitmapExamples();
    return 0;
}