#pragma once

#include <iostream>
#include "timecount.h"
#include "common.hpp"

void TestTimecount()
{
    cutl::timecount tct("TestTimecount");
    PrintTitle("timecount");
    std::cout << "TestTimecount begin" << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(1));
    std::cout << "TestTimecount end" << std::endl;
}
