#pragma once

// #include <algorithm>
// #include <cmath>
#include <cstdint>
// #include <stdexcept>
#include <string>
#include <unordered_map>
#include <vector>

namespace cutl
{

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
    virtual void reset() = 0;
    virtual size_t count() const = 0;
    virtual size_t size() const = 0;
    virtual std::string to_string() const = 0;
    virtual void from_string() = 0;
    virtual std::vector<size_t> valuelist() const = 0;

    // 操作符重载
    virtual bool operator[](size_t position) const = 0;
    // 比较操作
    virtual bool equals(const ibitmap& other) const = 0;
};

class bitmap : public ibitmap
{
protected:
    std::vector<uint8_t> bits_;
    size_t size_;

public:
    bitmap(size_t size);
    ~bitmap() = default;

public:
    /**
     * 设置指定位置为 1
     */
    void set(size_t position) override;

    /**
     * 获取指定位置的值
     */
    bool get(size_t position) const override;

    /**
     * 重载 [] 操作符
     */
    bool operator[](size_t position) const override { return get(position); }

    /**
     * 设置指定位置为 0
     */
    void reset(size_t position) override;

    // 将所有元素重置为0
    void reset() override;

    /**
     * @brief 获取数值为1的位数
     *
     * @return size_t
     */
    size_t count() const override;

    /**
     * 获取 bitmap 的大小（bit 数）
     */
    size_t size() const override { return size_; }

    std::string to_string() const;

    void from_string();

    std::vector<size_t> valuelist() const;

    bool equals(const ibitmap& other) const override;

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
    bitmap operator&(const bitmap& other) const;

    /**
     * 与另一个 bitmap 进行 OR 操作
     */
    bitmap operator|(const bitmap& other) const;

    // 按位取反
    bitmap operator~() const;

    // 按位异或
    bitmap operator^(const bitmap& other) const;

    bitmap& operator&=(const bitmap& other);

    bitmap& operator|=(const bitmap& other);

    bitmap& operator^=(const bitmap& other);
};

class dynamic_bitmap : public bitmap
{
public:
    dynamic_bitmap(size_t init_size = 64);
    ~dynamic_bitmap() = default;

public:
    /**
     * 设置指定位置为 1
     */
    void set(size_t position) override;

private:
    /**
     * 动态扩容
     */
    void ensureCapacity(size_t minSize);
};

class roaring_bitmap : public ibitmap
{
private:
    size_t block_size_{ 0 };
    std::unordered_map<size_t, bitmap> container_;

public:
    roaring_bitmap(size_t blockSize);
    ~roaring_bitmap() = default;

public:
    size_t block_size() const { return block_size_; }

    /**
     * 设置指定位置为 1
     */
    void set(size_t position) override;

    /**
     * 获取指定位置的值
     */
    bool get(size_t position) const override;

    /**
     * 重载 [] 操作符
     */
    bool operator[](size_t position) const override { return get(position); }

    /**
     * 设置指定位置为 0
     */
    void reset(size_t position) override;

    // 将所有元素重置为0
    void reset() override;

    /**
     * @brief 获取数值为1的位数
     *
     * @return size_t
     */
    size_t count() const override;

    /**
     * 获取 bitmap 的大小（bit 数）
     */
    size_t size() const override;

    std::string to_string() const;

    void from_string();

    std::vector<size_t> valuelist() const;

    /**
     * 判断是否相等
     */
    // bool operator==(const roaring_bitmap& other) const
    bool equals(const ibitmap& other) const override;

    bool operator==(const roaring_bitmap& other) const { return equals(other); }
    bool operator!=(const roaring_bitmap& other) const { return !equals(other); }

    /**
     * 与另一个 bitmap 进行 AND 操作
     */
    roaring_bitmap operator&(const roaring_bitmap& other) const;

    /**
     * 与另一个 bitmap 进行 OR 操作
     */
    roaring_bitmap operator|(const roaring_bitmap& other) const;

    /**
     * 与另一个 bitmap 进行 Not 操作(按位取反)
     */
    roaring_bitmap operator~() const;

    /**
     * 与另一个 bitmap 进行 异或 操作
     */
    roaring_bitmap operator^(const roaring_bitmap& other) const;

    // 与
    roaring_bitmap& operator&=(const roaring_bitmap& other);

    // 或
    roaring_bitmap& operator|=(const roaring_bitmap& other);

    // 异或
    roaring_bitmap& operator^=(const roaring_bitmap& other);
};

} // namespace cutl