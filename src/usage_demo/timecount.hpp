#pragma once

#include "common.hpp"
#include "common_util/timecount.h"
#include <cmath>
#include <iostream>
#include <map>
#include <unordered_map>
#include <unordered_set>
#include <vector>

using StrVec = std::vector<std::string>;
using USet = std::unordered_set<std::string>;
using Map = std::map<std::string, int>;
using UMap = std::unordered_map<std::string, int>;

std::string GetFromVec(const StrVec& vec, const std::string& key)
{
    cutl::timecount tcount(__func__);
    // auto itr = vec.find(key);
    auto itr = std::find(vec.begin(), vec.end(), key);
    if (itr != vec.end())
    {
        return *itr;
    }
    return "undefined";
}

std::string GetFromUSet_01(const USet& s, const std::string& key)
{
    cutl::timecount tcount(__func__);
    auto itr = s.find(key);
    if (itr != s.end())
    {
        return *itr;
    }
    return "undefined";
}

std::string GetFromUSet_02(const USet& s, const std::string& key)
{
    cutl::timecount tcount(__func__);
    if (s.count(key) > 0)
    {
        return key;
    }
    return "undefined";
}

int GetFromMap(const Map& mp, const std::string& key)
{
    cutl::timecount tcount(__func__);
    try
    {
        return mp.at(key);
    }
    catch (const std::exception& e)
    {
        std::cerr << e.what() << '\n';
    }
    return -1;
}

int GetFromUMap_01(const UMap& mp, const std::string& key)
{
    cutl::timecount tcount(__func__);
    auto itr = mp.find(key);
    if (itr != mp.end())
    {
        return itr->second;
    }
    return -1;
}

int GetFromUMap_02(const UMap& mp, const std::string& key)
{
    cutl::timecount tcount(__func__);
    try
    {
        return mp.at(key);
    }
    catch (const std::out_of_range& e)
    {
        std::cerr << e.what() << '\n';
    }

    return -1;
}

void TestGetFromContainer()
{
    PrintSubTitle("TestGetFromContainer");

    StrVec vec;
    USet uset;
    Map mp;
    UMap ump;

    // initialize containers
    // 100000
    for (int i = 0; i < 1000000; i++)
    {
        std::string key = "num_" + std::to_string(i);
        // printf("insert key: %s -> %d\n", key.c_str(), i);
        vec.emplace_back(key);
        uset.emplace(key);
        mp.insert(std::make_pair(key, i));
        ump.insert(std::make_pair(key, i));
    }

    auto getKey = "num_1000001";
    auto r1 = GetFromVec(vec, getKey);
    std::cout << "GetFromVec:" << r1 << std::endl;
    auto r2 = GetFromMap(mp, getKey);
    std::cout << "GetFromMap:" << r2 << std::endl;
    auto r3 = GetFromUMap_01(ump, getKey);
    std::cout << "GetFromUMap_01:" << r3 << std::endl;
    auto r4 = GetFromUMap_02(ump, getKey);
    std::cout << "GetFromUMap_02:" << r4 << std::endl;
    auto r5 = GetFromUSet_01(uset, getKey);
    std::cout << "GetFromUSet_01:" << r5 << std::endl;
    auto r6 = GetFromUSet_02(uset, getKey);
    std::cout << "GetFromUSet_02:" << r6 << std::endl;

    // std::unordered_map find的效率比 at()+trycatch的效率高
}

void TimecountUsage()
{
    PrintTitle("TimecountUsage");

    cutl::timecount tcount(__func__);
    // 统计CPU占用时间，不包含休眠或等待时间
    cutl::cpu_timecounter cpu_counter("CPU_Timer_Count");
    std::cout << "TestTimecount begin" << std::endl;

    // 执行一些计算密集型操作
    std::vector<double> numbers(1000000);
    for (size_t i = 0; i < numbers.size(); ++i)
    {
        numbers[i] = std::sin(i) * std::cos(i);
    }
    // 休眠1s
    std::this_thread::sleep_for(std::chrono::seconds(1));

    std::cout << "TestTimecount end" << std::endl;
}

void TestTimecount()
{
    PrintTitle("timecount");

    TimecountUsage();
    // TestGetFromContainer();
}
