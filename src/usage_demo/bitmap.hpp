#include "common_util/bitmap.h"

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