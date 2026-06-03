/**
 * @file test_singleton.cpp
 * @brief Unit tests for the macros defined in common_util/singleton.h.
 */

#include "common_util/singleton.h"
#include <gtest/gtest.h>

namespace
{

class PtrSingleton
{
    CUTL_SINGLETON_PTR_DEFAULT_CTOR(PtrSingleton)

public:
    int value{0};
};

PtrSingleton::~PtrSingleton() = default;

class RefSingleton
{
    CUTL_SINGLETON_REF_DEFAULT_CTOR(RefSingleton)

public:
    int value{0};
};

RefSingleton::~RefSingleton() = default;

// 使用私有构造函数的两种宏
class PrivateCtorPtrSingleton
{
    CUTL_SINGLETON_PTR(PrivateCtorPtrSingleton)

public:
    int value{0};
};

PrivateCtorPtrSingleton::PrivateCtorPtrSingleton()
  : value(7)
{
}
PrivateCtorPtrSingleton::~PrivateCtorPtrSingleton() = default;

class PrivateCtorRefSingleton
{
    CUTL_SINGLETON_REF(PrivateCtorRefSingleton)

public:
    int value{0};
};

PrivateCtorRefSingleton::PrivateCtorRefSingleton()
  : value(11)
{
}
PrivateCtorRefSingleton::~PrivateCtorRefSingleton() = default;

} // namespace

TEST(SingletonTest, PtrSingletonReturnsSameInstance)
{
    auto a = PtrSingleton::get_instance();
    auto b = PtrSingleton::get_instance();
    EXPECT_EQ(a, b);
    a->value = 42;
    EXPECT_EQ(b->value, 42);
}

TEST(SingletonTest, RefSingletonReturnsSameInstance)
{
    auto& a = RefSingleton::get_instance();
    auto& b = RefSingleton::get_instance();
    EXPECT_EQ(&a, &b);
    a.value = 7;
    EXPECT_EQ(b.value, 7);
}

TEST(SingletonTest, PrivateCtorPtrSingleton)
{
    auto a = PrivateCtorPtrSingleton::get_instance();
    auto b = PrivateCtorPtrSingleton::get_instance();
    ASSERT_NE(a, nullptr);
    EXPECT_EQ(a, b);
    // 用户提供的私有构造把 value 初始化为 7
    EXPECT_EQ(a->value, 7);
}

TEST(SingletonTest, PrivateCtorRefSingleton)
{
    auto& a = PrivateCtorRefSingleton::get_instance();
    auto& b = PrivateCtorRefSingleton::get_instance();
    EXPECT_EQ(&a, &b);
    // 用户提供的私有构造把 value 初始化为 11
    EXPECT_EQ(a.value, 11);
}
