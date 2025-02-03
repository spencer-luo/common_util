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

class C
{
public:
    C();
    static C* get_instance();

public:
    int c_;

private:
    ~C();
};

C::C()
{
    std::cout << "C()" << std::endl;
}

C::~C()
{
    std::cout << "~C()" << std::endl;
}

static C* c_obj = nullptr;
C* C::get_instance()
{
    if (!c_obj)
    {
        static std::once_flag flag;
        std::call_once(flag, [&] { c_obj = new (std::nothrow) C(); });
    }
    return c_obj;
}

void TestSingleton()
{
    PrintTitle("TestSingleton");
    A::get_instance().a_ = 5;
    std::cout << "a.a_ = " << A::get_instance().a_ << std::endl;
    B::get_instance()->b_ = 10;
    std::cout << "b.b_ = " << B::get_instance()->b_ << std::endl;
    C::get_instance()->c_ = 15;
    std::cout << "c.c_ = " << C::get_instance()->c_ << std::endl;
}
