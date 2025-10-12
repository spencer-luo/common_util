#include "algoutil.hpp"
#include "bitmap.hpp"
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
#include "threadutil.hpp"
#include "timecount.hpp"
#include "timer.hpp"
#include "timeutil.hpp"
#include "verutil.hpp"

void usage_demo()
{
    // Data formatting
    std::cout << "fmt_uint: " << cutl::fmt_uint(12, 5) << std::endl;
    std::cout << "fmt_double: " << cutl::fmt_double(3.141592653, 4) << std::endl;
    std::cout << "fmt_filesize: " << cutl::fmt_filesize(33600) << std::endl;
    // Get the current system time
    auto now = cutl::datetime::now();
    std::cout << "current time(UTC time): " << now.utctime() << std::endl;
    std::cout << "current time(local time): " << now.format() << std::endl;
}

int main(int argc, char* argv[])
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
    // TestThreadUtil();
    // TestEventLoop();
    // TestThreadPool();
    // TestAlgorithmUtil();
    BitmapTest();

    // usage_demo();

    return 0;
}