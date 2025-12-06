#include <functional>
#include <unordered_map>

namespace cutl
{

template<typename T>
class state_machine
{
public:
    using callback_type = std::function<void(const T& from, const T& to, bool changed)>;
    explicit state_machine(const T& init_state, callback_type onchange = nullptr)
      : init_state_(init_state)
      , current_state_(init_state)
      , on_change_(onchange)
    {
    }
    ~state_machine() = default;

public:
    void add_roadmap(const T& from, const T& to)
    {
        if (!check_road(from, to))
        {
            roadmap_.emplace(from, to);
        }
    }

    const T& get_state() const { return current_state_; }

    bool set_state(const T& state)
    {
        auto res = check_road(current_state_, state);
        // set status
        auto pre_status = current_state_;
        if (res)
        {
            current_state_ = state;
        }
        // calling callback function
        if (on_change_)
        {
            on_change_(pre_status, state, res);
        }
        return res;
    }

    void reset() { current_state_ = init_state_; }

private:
    bool check_road(const T& from, const T& to)
    {
        auto range = roadmap_.equal_range(from);
        for (auto it = range.first; it != range.second; ++it)
        {
            if (it->second == to)
            {
                return true;
            }
        }
        return false;
    }

private:
    T current_state_;
    T init_state_;
    callback_type on_change_;
    std::unordered_multimap<T, T> roadmap_;
};

} // namespace cutl