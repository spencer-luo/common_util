#include "observer.h"
#include "inner/logger.h"
#include <algorithm>

namespace cutl
{

void observable::add_observer(std::shared_ptr<observer> o)
{
    if (!o)
    {
        CUTL_ERROR("observer is null");
        return;
    }
    std::lock_guard<std::mutex> lock(observers_mutex_);
    if (std::find(observers_.begin(), observers_.end(), o) != observers_.end())
    {
        return;
    }
    observers_.emplace_back(std::move(o));
}

void observable::remove_observer(std::shared_ptr<observer> o)
{
    std::lock_guard<std::mutex> lock(observers_mutex_);
    auto it = std::find(observers_.begin(), observers_.end(), o);
    if (it != observers_.end())
    {
        observers_.erase(it);
    }
}

#if __cplusplus >= 201703L
void observable::notify_observers(const std::any& data)
#else
void observable::notify_observers(const void* data)
#endif
{
    std::vector<std::shared_ptr<observer>> snapshot;
    {
        std::lock_guard<std::mutex> lock(observers_mutex_);
        snapshot = observers_;
    }
    for (const auto& obs : snapshot)
    {
        if (obs)
        {
            obs->update(this, data);
        }
    }
}

} // namespace cutl
