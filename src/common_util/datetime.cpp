#include "datetime.h"
#include "timeutil.h"

namespace cutl
{
    datetime::datetime()
    {
        timestamp_ms_ = timestamp_ms();
    }

    datetime::datetime(uint64_t timestamp_ms) : timestamp_ms_(timestamp_ms)
    {
    }

    datetime::~datetime()
    {
    }

    datetime datetime::now()
    {
        return datetime();
    }

    datetime datetime::get(const std::string &time_text)
    {
        // todo
        return datetime();
    }

    std::string datetime::format(date_format dfmt, time_format tfmt) const
    {
        return "";
    }

    std::string datetime::format(std::string &reggex_pattern) const
    {
        return "";
    }

} // namespace
