#include <cmath>
#include <cstdint>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

class Bitmap
{
protected:
    std::vector<uint8_t> bits_;
    size_t size_;

public:
    Bitmap(size_t size)
      : size_(size)
    {
        // 计算需要的字节数，向上取整: size/8
        // 注意：这里必须除以8.0，才能获取浮点数然后向上取整
        bits_.resize(std::ceil(size / 8.0), 0);
    }

    ~Bitmap() = default;

    /**
     * 设置指定位置为 1
     */
    void set(size_t position)
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
    bool get(size_t position) const
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
    bool operator[](size_t position) const { return get(position); }

    /**
     * 设置指定位置为 0
     */
    void reset(size_t position)
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
    size_t count()
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
     * 获取 Bitmap 的大小（bit 数）
     */
    size_t size() const { return size_; }

    /**
     * 判断是否相等
     */
    bool operator==(const Bitmap& other) const
    {
        if (size_ != other.size_)
            return false;
        for (size_t i = 0; i < bits_.size(); i++)
        {
            if (bits_[i] != other.bits_[i])
                return false;
        }
        return true;
    }

    /**
     * 判断是否不相等
     */
    bool operator!=(const Bitmap& other) const
    {
        bool equal = (*this == other);
        return !equal;
    }

    /**
     * 与另一个 Bitmap 进行 AND 操作
     */
    Bitmap operator&(const Bitmap& other) const
    {
        if (this->size_ != other.size_)
        {
            throw std::invalid_argument("Bitmaps must have same size");
        }
        Bitmap result(size_);
        for (size_t i = 0; i < bits_.size(); i++)
        {
            result.bits_[i] = bits_[i] & other.bits_[i];
        }
        return result;
    }

    /**
     * 与另一个 Bitmap 进行 OR 操作
     */
    Bitmap operator|(const Bitmap& other) const
    {
        if (this->size_ != other.size_)
        {
            throw std::invalid_argument("Bitmaps must have same size");
        }
        Bitmap result(size_);
        for (size_t i = 0; i < bits_.size(); i++)
        {
            result.bits_[i] = bits_[i] | other.bits_[i];
        }
        return result;
    }
};

class DynamicBitmap : public Bitmap
{
public:
    DynamicBitmap(size_t init_size = 64)
      : Bitmap(init_size)
    {
    }

    ~DynamicBitmap() = default;

public:
    /**
     * 设置指定位置为 1
     */
    void set(size_t position)
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

class RoaringBitmap
{
private:
    size_t block_size_{ 0 };
    std::unordered_map<size_t, Bitmap> container_;

public:
    RoaringBitmap(size_t blockSize)
      : block_size_(blockSize)
    {
    }

    ~RoaringBitmap() = default;

public:
    size_t block_size() const { return block_size_; }

    /**
     * 设置指定位置为 1
     */
    void set(size_t position)
    {
        size_t key = position / block_size_;
        size_t bitPosition = position % block_size_;

        if (!container_.count(key))
        {
            // block不存在则添加新的block
            container_.emplace(key, Bitmap(block_size_));
        }
        container_.at(key).set(bitPosition);
    }

    /**
     * 获取指定位置的值
     */
    bool get(size_t position) const
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
    bool operator[](size_t position) const { return get(position); }

    /**
     * 设置指定位置为 0
     */
    void reset(size_t position)
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
    size_t count()
    {
        size_t count = 0;
        for (auto itr = container_.begin(); itr != container_.end(); itr++)
        {
            count += itr->second.count();
        }
        return count;
    }

    /**
     * 获取 Bitmap 的大小（bit 数）
     */
    size_t size() const
    {
        size_t size = 0;
        for (auto itr = container_.begin(); itr != container_.end(); itr++)
        {
            size += itr->second.size();
        }
        return size;
    }

    /**
     * 与另一个 Bitmap 进行 AND 操作
     */
    RoaringBitmap operator&(const RoaringBitmap& other) const
    {
        if (block_size() != other.block_size())
        {
            throw std::invalid_argument("RoaringBitmap must have same block_size");
        }
        if (container_.size() != other.container_.size())
        {
            throw std::invalid_argument("RoaringBitmap must have same size");
        }
        RoaringBitmap rBitmap(block_size_);
        for (auto itr = container_.begin(); itr != container_.end(); itr++)
        {
            auto& key = itr->first;
            auto& val = itr->second;

            if (other.container_.count(key))
            {
                Bitmap result = val & other.container_.at(key);
                rBitmap.container_.emplace(key, result);
            }
        }

        return rBitmap;
    }

    /**
     * 与另一个 Bitmap 进行 OR 操作
     */
    RoaringBitmap operator|(const RoaringBitmap& other) const
    {
        if (block_size() != other.block_size())
        {
            throw std::invalid_argument("RoaringBitmap must have same block_size");
        }
        if (container_.size() != other.container_.size())
        {
            throw std::invalid_argument("RoaringBitmap must have same size");
        }
        RoaringBitmap rBitmap(block_size_);
        for (auto itr = container_.begin(); itr != container_.end(); itr++)
        {
            auto& key = itr->first;
            auto& val = itr->second;

            if (other.container_.count(key))
            {
                Bitmap result = val | other.container_.at(key);
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
     * 判断是否相等
     */
    bool operator==(const RoaringBitmap& other) const
    {
        if (block_size() != other.block_size())
        {
            return false;
        }
        if (container_.size() != other.container_.size())
        {
            return false;
        }
        for (auto itr = container_.begin(); itr != container_.end(); itr++)
        {
            auto& key = itr->first;
            auto& val = itr->second;
            if (!other.container_.count(key))
            {
                return false;
            }
            if (val != other.container_.at(key))
            {
                return false;
            }
        }
        return true;
    }
};

#include <iostream>

void test_bitmap()
{
    std::cout << "=== 基础 Bitmap 示例 ===" << std::endl;
    Bitmap bitmap1(100);
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
    Bitmap bitmap2(100);
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
    std::cout << "\n=== 动态 Bitmap 示例 ===" << std::endl;
    DynamicBitmap dynamicBitmap;
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
    RoaringBitmap bitmap1(64);
    bitmap1.set(10);
    bitmap1.set(20);
    std::cout << "bitmap1 size: " << bitmap1.size() << ", count: " << bitmap1.count() << std::endl;
    bitmap1.set(80);
    std::cout << "bitmap1 size: " << bitmap1.size() << ", count: " << bitmap1.count() << std::endl;

    RoaringBitmap bitmap2(64);
    bitmap2.set(20);
    bitmap2.set(40);
    bitmap2.set(130);
    std::cout << "bitmap2 size: " << bitmap2.size() << ", count: " << bitmap2.count() << std::endl;
    std::cout << "bitmap1 == bitmap2: " << (bitmap1 == bitmap2) << std::endl;

    RoaringBitmap andBitmap = bitmap1 & bitmap2;
    std::cout << "andBitmap size: " << andBitmap.size() << ", count: " << andBitmap.count()
              << std::endl;
    RoaringBitmap orBitmap = bitmap1 | bitmap2;
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