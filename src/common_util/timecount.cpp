#include "timecount.h"
#include "timeutil.h"
#include "strfmt.h"
#include "logger.h"

namespace cutl
{
    timecount::timecount(const std::string &func_name)
        : func_name_(func_name)
    {
        start_time_ = clocktime_ms();
    }

    timecount::~timecount()
    {
        auto end_time = clocktime_ms();
        auto duration = end_time - start_time_;
        auto text = "[timecount] " + func_name_ + " used " + fmt_timeduration_ms(duration);
        LOG.info(text);
    }
} // namespace