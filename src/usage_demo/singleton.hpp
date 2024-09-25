#include "common.hpp"
#include "singleton.h"
#include <iostream>

class A
{
    CUTL_SINGLETON_REF(A)
public:
    int a_;
};

A::A()
{
    std::cout << "A()" << std::endl;
}

A::~A()
{
    std::cout << "~A()" << std::endl;
}

class B
{
    CUTL_SINGLETON_PTR(B)
public:
    int b_;
};

B::B()
{
    std::cout << "B()" << std::endl;
}

B::~B()
{
    std::cout << "~B()" << std::endl;
}

void TestSingleton()
{
    PrintTitle("TestSingleton");
    A::get_instance().a_ = 5;
    std::cout << "a.a_ = " << A::get_instance().a_ << std::endl;
    B::get_instance()->b_ = 7;
    std::cout << "b.b_ = " << B::get_instance()->b_ << std::endl;
}
