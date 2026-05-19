/**
 * @file test_observer.cpp
 * @brief Unit tests for cutl::observer / observable.
 */

#include "common_util/observer.h"
#include <gtest/gtest.h>
#include <memory>
#include <string>

namespace
{

class CountingObservable : public cutl::observable
{
public:
#if __cplusplus >= 201703L
    void publish(const std::any& d) { notify_observers(d); }
#else
    void publish(const void* d) { notify_observers(d); }
#endif
    void publish_empty() { notify_observers(); }
};

class CountingObserver : public cutl::observer
{
public:
#if __cplusplus >= 201703L
    void update(const cutl::observable* /*subject*/, const std::any& data) override
    {
        ++calls;
        if (data.has_value() && data.type() == typeid(std::string))
        {
            last = std::any_cast<std::string>(data);
        }
        else
        {
            last.clear();
        }
    }
#else
    void update(const cutl::observable* /*subject*/, const void* data) override
    {
        ++calls;
        if (data == nullptr)
        {
            last.clear();
        }
        else
        {
            last = *static_cast<const std::string*>(data);
        }
    }
#endif

    int calls = 0;
    std::string last;
};

} // namespace

TEST(ObserverTest, NotifiesAllObservers)
{
    CountingObservable subject;
    auto a = std::make_shared<CountingObserver>();
    auto b = std::make_shared<CountingObserver>();
    subject.add_observer(a);
    subject.add_observer(b);

    subject.publish_empty();
    EXPECT_EQ(a->calls, 1);
    EXPECT_EQ(b->calls, 1);

    std::string msg = "hello observer";
#if __cplusplus >= 201703L
    subject.publish(std::any(msg));
#else
    subject.publish(&msg);
#endif

    EXPECT_EQ(a->calls, 2);
    EXPECT_EQ(b->calls, 2);
    EXPECT_EQ(a->last, msg);
    EXPECT_EQ(b->last, msg);
}

TEST(ObserverTest, RemovedObserverNoLongerNotified)
{
    CountingObservable subject;
    auto a = std::make_shared<CountingObserver>();
    auto b = std::make_shared<CountingObserver>();
    subject.add_observer(a);
    subject.add_observer(b);

    subject.publish_empty();
    EXPECT_EQ(a->calls, 1);
    EXPECT_EQ(b->calls, 1);

    subject.remove_observer(b);
    subject.publish_empty();
    EXPECT_EQ(a->calls, 2);
    EXPECT_EQ(b->calls, 1);
}
