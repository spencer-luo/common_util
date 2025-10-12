#include "common.hpp"
#include "common_util/bitmap.h"
#include <algorithm> // 用于测试valuelist排序
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
    std::cout << "bitmap1 str: " << hexStr << std::endl;
    cutl::bitmap bitmap2(100);
    bitmap2.from_string(hexStr);
    assert(bitmap2.equals(bitmap1));

    // 位运算测试
    cutl::bitmap bitmap3(100);
    bitmap3.set(20);
    bitmap3.set(30);

    // bitmap1: 10,20
    auto andResult = bitmap1 & bitmap3;
    assert(andResult.count() == 1); // 仅20位置为1

    auto orResult = bitmap1 | bitmap3;
    assert(orResult.count() == 3); // 10,20,30

    auto xorResult = bitmap1 ^ bitmap3;
    assert(xorResult.count() == 2); // 10,30

    auto notResult = ~bitmap1;
    assert(notResult.count() == 102);
    assert(notResult.get(10) == false);
    assert(notResult.get(0) == true);

    // 新增：测试operator&=
    cutl::bitmap andAssign(100);
    andAssign.set(10);
    andAssign.set(20);
    andAssign.set(30);
    andAssign &= bitmap3; // bitmap3:20,30
    assert(andAssign.count() == 2);
    assert(andAssign.get(20) && andAssign.get(30) && !andAssign.get(10));

    // 新增：测试operator|=
    cutl::bitmap orAssign(100);
    orAssign.set(10);
    orAssign |= bitmap3; // bitmap3:20,30
    assert(orAssign.count() == 3);
    assert(orAssign.get(10) && orAssign.get(20) && orAssign.get(30));

    // 新增：测试operator^=
    cutl::bitmap xorAssign(100);
    xorAssign.set(10);
    xorAssign.set(20);
    xorAssign ^= bitmap3; // bitmap3:20,30
    assert(xorAssign.count() == 2);
    assert(xorAssign.get(10) && xorAssign.get(30) && !xorAssign.get(20));

    // 新增：测试valuelist()
    std::vector<size_t> expected = { 10, 20 };
    std::vector<size_t> actual = bitmap1.valuelist();
    assert(actual == expected);

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

    // 新增：测试operator&=
    cutl::dynamic_bitmap dbAnd1(20);
    dbAnd1.set(5);
    dbAnd1.set(10);
    dbAnd1.set(15);

    cutl::dynamic_bitmap dbAnd2(20);
    dbAnd2.set(10);
    dbAnd2.set(15);
    dbAnd2.set(20);

    dbAnd1 &= dbAnd2;
    assert(dbAnd1.count() == 2);
    assert(dbAnd1.get(10) && dbAnd1.get(15) && !dbAnd1.get(5));
    assert(dbAnd1.size() != dbAnd2.size());

    // 新增：测试operator|=
    cutl::dynamic_bitmap dbOr1(10);
    dbOr1.set(3);
    dbOr1.set(7);

    cutl::dynamic_bitmap dbOr2(15);
    dbOr2.set(7);
    dbOr2.set(12);

    dbOr1 |= dbOr2;
    assert(dbOr1.count() == 3);
    assert(dbOr1.get(3) && dbOr1.get(7) && dbOr1.get(12));
    assert(dbOr1.size() >= 15);

    // 新增：测试operator^=
    cutl::dynamic_bitmap dbXor1(20);
    dbXor1.set(4);
    dbXor1.set(8);
    dbXor1.set(12);

    cutl::dynamic_bitmap dbXor2(20);
    dbXor2.set(8);
    dbXor2.set(16);

    dbXor1 ^= dbXor2;
    assert(dbXor1.count() == 3);
    assert(dbXor1.get(4) && dbXor1.get(12) && dbXor1.get(16) && !dbXor1.get(8));

    // 新增：测试valuelist()
    dbXor1.set(20);
    std::vector<size_t> expected = { 4, 12, 16, 20 };
    std::vector<size_t> actual = dbXor1.valuelist();
    assert(actual == expected);

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
    assert(bitmap1.get(25) == false);
    assert(bitmap1.get(80) == true);
    assert(bitmap1.size() == 128); // 2个block（0和1）

    // 字符串转换测试
    std::string str = bitmap1.to_string();
    std::cout << "bitmap1 str: " << str << std::endl;
    cutl::roaring_bitmap bitmap2(64);
    bitmap2.from_string(str);
    assert(bitmap2.equals(bitmap1));

    // 位运算测试
    cutl::roaring_bitmap bitmap3(64);
    bitmap3.set(20);
    bitmap3.set(40);
    bitmap3.set(130); // 130/64=2（第三个block）

    // bitmap1: 10,20,80
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
    assert(bitmap1.count() == 2); // 10, 80

    // 新增：测试operator&=
    cutl::roaring_bitmap rbAnd1(64);
    rbAnd1.set(10);
    rbAnd1.set(80);
    rbAnd1.set(130);

    cutl::roaring_bitmap rbAnd2(64);
    rbAnd2.set(80);
    rbAnd2.set(130);
    rbAnd2.set(150);

    rbAnd1 &= rbAnd2;
    assert(rbAnd1.count() == 2);
    assert(rbAnd1.get(80) && rbAnd1.get(130) && !rbAnd1.get(10));

    // 新增：测试operator|=
    cutl::roaring_bitmap rbOr1(64);
    rbOr1.set(10);
    rbOr1.set(80);

    cutl::roaring_bitmap rbOr2(64);
    rbOr2.set(80);
    rbOr2.set(130);

    rbOr1 |= rbOr2;
    assert(rbOr1.count() == 3);
    assert(rbOr1.get(10) && rbOr1.get(80) && rbOr1.get(130));

    // 新增：测试operator^=
    cutl::roaring_bitmap rbXor1(64);
    rbXor1.set(10);
    rbXor1.set(80);
    rbXor1.set(130);

    cutl::roaring_bitmap rbXor2(64);
    rbXor2.set(80);
    rbXor2.set(150);

    rbXor1 ^= rbXor2;
    assert(rbXor1.count() == 3);
    // 10, 130, 150
    assert(rbXor1.get(10) && rbXor1.get(130) && rbXor1.get(150) && !rbXor1.get(80));

    // 新增：测试valuelist()
    std::vector<size_t> expected = { 10, 130, 150 };
    std::vector<size_t> actual = rbXor1.valuelist();
    std::cout << "rbXor1 valuelist: " << cutl::fmt_vec(actual) << std::endl;
    std::sort(actual.begin(), actual.end()); // 排序后比较
    assert(actual == expected);

    std::cout << "roaring_bitmap tests passed" << std::endl;
}

void BitmapTest()
{
    PrintTitle("BitmapTest");
    test_bitmap();
    test_dynamic_bitmap();
    test_roaring_bitmap();
}