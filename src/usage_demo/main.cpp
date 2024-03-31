#include "common.hpp"
#include "config.hpp"
#include "sysutil.hpp"
#include "strfmt.hpp"
#include "timeutil.hpp"
#include "timecount.hpp"
#include "datetime.hpp"

int main()
{
    LibraryInit();
    // TestSysutil();
    // TestStrfmt();
    // TestTimeutil();
    // TestTimecount();
    TestDatetime();

    return 0;
}