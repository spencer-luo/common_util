#include "common.hpp"
#include "common_util/bitmap.h"
#include <cassert>

void test_bitmap()
{
    PrintSubTitle("bitmap");

    cutl::bitmap bitmap1(100);
    bitmap1.set(10);
    bitmap1.set(20);
    bitmap1.set(99);

    // 基础功能测试
    assert(bitmap1.get(10) == true);
    assert(bitmap1.get(25) == false);
    assert(bitmap1.count() == 3);

    // 重复设置测试
    bitmap1.set(99);
    assert(bitmap1.count() == 3);

    // 重置测试
    bitmap1.reset(99);
    assert(bitmap1.count() == 2);

    // 字符串转换测试
    std::string hexStr = bitmap1.to_string();
    std::cout << "hexStr: " << hexStr << std::endl;
    cutl::bitmap bitmap2(100);
    bitmap2.from_string(hexStr);
    assert(bitmap2.equals(bitmap1));

    // 位运算测试
    cutl::bitmap bitmap3(100);
    bitmap3.set(20);
    bitmap3.set(30);

    auto andResult = bitmap1 & bitmap3;
    assert(andResult.count() == 1); // 仅20位置为1

    auto orResult = bitmap1 | bitmap3;
    assert(orResult.count() == 3); // 10,20,30

    auto xorResult = bitmap1 ^ bitmap3;
    // std::cout << "xorResult.count: " << xorResult.count() << std::endl;
    assert(xorResult.count() == 2); // 10,30

    auto notResult = ~bitmap1;
    assert(notResult.get(10) == false);
    assert(notResult.get(0) == true);

    std::cout << "bitmap tests passed" << std::endl;
}

void test_dynamic_bitmap()
{
    PrintSubTitle("dynamic_bitmap");

    cutl::dynamic_bitmap dynamicBitmap(10);
    assert(dynamicBitmap.size() == 10);

    // 扩容测试
    dynamicBitmap.set(15);
    assert(dynamicBitmap.size() >= 16); // 扩容为20（原10*2）
    assert(dynamicBitmap.get(15) == true);

    // 更大范围扩容
    dynamicBitmap.set(100);
    assert(dynamicBitmap.size() >= 101);
    assert(dynamicBitmap.get(100) == true);

    std::cout << "dynamic_bitmap tests passed" << std::endl;
}

void test_roaring_bitmap()
{
    PrintSubTitle("roaring_bitmap");

    cutl::roaring_bitmap bitmap1(64);
    bitmap1.set(10);
    bitmap1.set(20);
    bitmap1.set(80); // 属于第二个block（80/64=1）

    // 基础功能测试
    assert(bitmap1.count() == 3);
    assert(bitmap1.get(20) == true);
    assert(bitmap1.get(80) == true);
    assert(bitmap1.size() == 128); // 2个block（0和1）

    // 字符串转换测试
    std::string str = bitmap1.to_string();
    cutl::roaring_bitmap bitmap2(64);
    bitmap2.from_string(str);
    assert(bitmap2.equals(bitmap1));

    // 位运算测试
    cutl::roaring_bitmap bitmap3(64);
    bitmap3.set(20);
    bitmap3.set(40);
    bitmap3.set(130); // 130/64=2（第三个block）

    auto andBitmap = bitmap1 & bitmap3;
    assert(andBitmap.count() == 1); // 仅20

    auto orBitmap = bitmap1 | bitmap3;
    assert(orBitmap.count() == 5); // 10,20,40,80,130

    cutl::roaring_bitmap bitmap4(64);
    bitmap4.set(20);
    bitmap4.set(40);
    bitmap4.set(80);
    // bitmap1: 10,20,80
    auto xorBitmap = bitmap1 ^ bitmap4;
    assert(xorBitmap.count() == 2); // 10,40

    // 重置测试
    bitmap1.reset(20);
    assert(bitmap1.count() == 2);

    std::cout << "roaring_bitmap tests passed" << std::endl;
}

void BitmapTest()
{
    PrintTitle("BitmapTest");
    test_bitmap();
    test_dynamic_bitmap();
    test_roaring_bitmap();
}