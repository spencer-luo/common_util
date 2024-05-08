#include "common.hpp"
#include "config.hpp"
#include "sysutil.hpp"
#include "strfmt.hpp"
#include "timeutil.hpp"
#include "timecount.hpp"
#include "datetime.hpp"
#include "verutil.hpp"
#include "strutil.hpp"
#include "filepath.hpp"
#include "fileutil.hpp"

int main(int argc, char *argv[])
{
    LibraryInit();
    TestSysutil();
    TestStrfmt();
    TestTimeutil();
    TestTimecount();
    TestDatetime();
    TestVerUtil();
    TestStrUtil();
    TestFilePath();
    TestFileUtil();

    return 0;
}