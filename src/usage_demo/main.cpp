#include "common.hpp"
#include "config.hpp"
#include "sysutil.hpp"
#include "strfmt.hpp"
#include "timeutil.hpp"
#include "timecount.hpp"
#include "datetime.hpp"
#include "verutil.hpp"
#include "strutil.hpp"

int main()
{
    LibraryInit();
    TestSysutil();
    TestStrfmt();
    // TestTimeutil();
    // TestTimecount();
    // TestDatetime();
    // TestVerUtil();
    TestStrUtil();

    return 0;
}