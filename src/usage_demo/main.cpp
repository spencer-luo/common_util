#include "common.hpp"
#include "config.hpp"
#include "datetime.hpp"
#include "dlloader.hpp"
#include "eventloop.hpp"
#include "filepath.hpp"
#include "fileutil.hpp"
#include "lrucache.hpp"
#include "print.hpp"
#include "singleton.hpp"
#include "strfmt.hpp"
#include "strutil.hpp"
#include "sysutil.hpp"
#include "threadpool.hpp"
#include "timecount.hpp"
#include "timer.hpp"
#include "timeutil.hpp"
#include "verutil.hpp"
#include "threadutil.hpp"

int main(int argc, char *argv[])
{
    LibraryInit();
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
    // TestDlLoader();
    // TestPrint();
    // TestTimer();
    // TestLRUCache();
    TestThreadUtil();
    // TestEventLoop();
    // TestThreadPool();

    return 0;
}