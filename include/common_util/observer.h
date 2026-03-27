#pragma once

#include <memory>
#include <mutex>
#include <vector>
#if __cplusplus >= 201703L
#include <any>
#endif

namespace cutl
{

class observable;

class observer
{
public:
    virtual ~observer() = default;

#if __cplusplus >= 201703L
    virtual void update(const observable* subject, const std::any& data) = 0;
#else
    virtual void update(const observable* subject, const void* data = nullptr) = 0;
#endif
};

class observable
{
public:
    virtual ~observable() = default;

public:
    void add_observer(std::shared_ptr<observer> o);
    void remove_observer(std::shared_ptr<observer> o);
#if __cplusplus >= 201703L
    void notify_observers(const std::any& data = std::any());
#else
    void notify_observers(const void* data = nullptr);
#endif

protected:
    std::mutex observers_mutex_;
    std::vector<std::shared_ptr<observer>> observers_;
};

} // namespace cutl