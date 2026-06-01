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
