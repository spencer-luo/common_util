#pragma once

#include "common.hpp"
#include "common_util/observer.h"
#include <iostream>
#include <memory>
#include <string>

namespace
{

class demo_subject : public cutl::observable
{
public:
    void publish_without_data()
    {
        std::cout << "subject notify without data" << std::endl;
        notify_observers();
    }

#if __cplusplus >= 201703L
    void publish_with_data(const std::string& msg)
    {
        std::cout << " [C++dft] subject notify with data: " << msg << std::endl;
        notify_observers(std::string(msg));
    }
#else
    void publish_with_data(const std::string& msg)
    {
        std::cout << " [C++11] subject notify with data: " << msg << std::endl;
        notify_observers(&msg);
    }
#endif
};

class demo_observer : public cutl::observer
{
public:
    explicit demo_observer(const std::string& name)
      : name_(name)
    {
    }

#if __cplusplus >= 201703L
    void update(const cutl::observable* subject, const std::any& data) override
    {
        std::cout << "[" << name_ << "] update from " << subject;
        if (!data.has_value())
        {
            std::cout << ", data is empty" << std::endl;
            return;
        }

        if (data.type() == typeid(std::string))
        {
            std::cout << ", data: " << std::any_cast<std::string>(data) << std::endl;
            return;
        }

        std::cout << ", data type: " << data.type().name() << std::endl;
    }
#else
    void update(const cutl::observable* subject, const void* data) override
    {
        std::cout << "[" << name_ << "] update from " << subject;
        if (data == nullptr)
        {
            std::cout << ", data is empty" << std::endl;
            return;
        }

        const auto* msg = static_cast<const std::string*>(data);
        std::cout << ", data: " << *msg << std::endl;
    }
#endif

private:
    std::string name_;
};

} // namespace

void TestObserver()
{
    PrintTitle("observer");
    PrintSubTitle("notify with empty and non-empty data");

    demo_subject subject;
    auto obs_a = std::make_shared<demo_observer>("observer_A");
    auto obs_b = std::make_shared<demo_observer>("observer_B");

    subject.add_observer(obs_a);
    subject.add_observer(obs_b);

    subject.publish_without_data();
    subject.publish_with_data("hello observer");

    subject.remove_observer(obs_b);
    PrintSubTitle("after remove observer_B");
    subject.publish_with_data("only observer_A receives this");
}
