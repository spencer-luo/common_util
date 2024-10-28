#include "common.hpp"
#include "config.hpp"
#include "datetime.hpp"
#include "dlloader.hpp"
#include "filepath.hpp"
#include "fileutil.hpp"
#include "singleton.hpp"
#include "strfmt.hpp"
#include "strutil.hpp"
#include "sysutil.hpp"
#include "timecount.hpp"
#include "timeutil.hpp"
#include "verutil.hpp"

int main(int argc, char *argv[])
{
    // LibraryInit();
    // TestSingleton();
    // std::cout << "test done" << std::endl;
    // TestSysutil();
    // TestStrfmt();
    // TestTimeutil();
    // TestTimecount();
    // TestDatetime();
    // TestVerUtil();
    // TestStrUtil();
    // TestFilePath();
    // TestFileUtil();
    TestDlLoader();

    return 0;
}