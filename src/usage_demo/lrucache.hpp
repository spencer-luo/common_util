#include "common.hpp"
#include "common_util/lrucache.h"

void case_01_02()
{
    PrintSubTitle("case 01");

    // 【Case 1】
    //  [[2], [1, 1], [2, 2], [1], [3, 3], [2], [4, 4], [1], [3], [4]]
    // [null, null, null, 1, null, -1, null, -1, 3, 4]
    auto visit_lru_node = [](int k, int v) { std::cout << k << ":" << v << " "; };
    auto iterate_lru_cache = [visit_lru_node](cutl::lru_cache<int, int>& cache)
    {
        std::cout << "[ ";
        cache.for_each(visit_lru_node);
        std::cout << "]" << std::endl;
    };
    cutl::lru_cache<int, int> cache(2);
    cache.put(1, 1);
    iterate_lru_cache(cache);
    cache.put(2, 2);
    iterate_lru_cache(cache);
    std::cout << "get(1): " << cache.get(1) << std::endl; // 1
    iterate_lru_cache(cache);
    cache.put(3, 3);
    iterate_lru_cache(cache);
    std::cout << "get(2): " << cache.get(2) << std::endl; // -1
    iterate_lru_cache(cache);
    cache.put(4, 4);
    iterate_lru_cache(cache);
    std::cout << "get(1): " << cache.get(1) << std::endl; // -1
    iterate_lru_cache(cache);
    std::cout << "get(3): " << cache.get(3) << std::endl; // 3
    iterate_lru_cache(cache);
    std::cout << "get(4): " << cache.get(4) << std::endl; // 4
    iterate_lru_cache(cache);

    // 清除缓存
    PrintSubTitle("clean cache");
    cache.clear();
    iterate_lru_cache(cache);

    PrintSubTitle("case 02");
    // 【Case 2】
    // ["lru_cache","get","put","get","put","put","get","get"]
    // [[2],[2],[2,6],[1],[1,5],[1,2],[1],[2]]
    std::cout << "get(2): " << cache.get(2) << std::endl; // -1
    iterate_lru_cache(cache);
    cache.put(2, 6);
    iterate_lru_cache(cache);
    std::cout << "get(1): " << cache.get(1) << std::endl; // -1
    iterate_lru_cache(cache);
    cache.put(1, 5);
    iterate_lru_cache(cache);
    cache.put(1, 2);
    iterate_lru_cache(cache);
    std::cout << "get(1): " << cache.get(1) << std::endl; // 2
    iterate_lru_cache(cache);
    std::cout << "get(2): " << cache.get(2) << std::endl; // 6
    iterate_lru_cache(cache);
}

struct Person
{
    Person()
      : name("Undefined")
      , age(0)
    {
    }
    Person(const std::string& name, int age)
      : name(name)
      , age(age)
    {
    }

    std::string name;
    int age;
};

void case_03()
{
    PrintSubTitle("case 03");

    // 【Case 3】
    // ["lru_cache","put","put","put","put","get","get"]
    // [[2],[2,1],[1,1],[2,3],[4,1],[1],[2]]
    using PersonCache = cutl::lru_cache<std::string, Person>;
    PersonCache cache(2);
    auto visit_lru_node = [](const std::string& k, const Person& v) { std::cout << k << " "; };
    auto iterate_lru_cache = [visit_lru_node](PersonCache& cache)
    {
        std::cout << "[ ";
        cache.for_each(visit_lru_node);
        std::cout << "]" << std::endl;
    };

    cache.put("Spencer", Person("Spencer", 25));
    iterate_lru_cache(cache);
    cache.put("Alex", Person("Alex", 35));
    iterate_lru_cache(cache);
    cache.put("Tom", Person("Tom", 30));
    iterate_lru_cache(cache);
    std::cout << "get(Alex): " << cache.get("Alex").name << std::endl; // Alex
    iterate_lru_cache(cache);
    std::cout << "get(Spencer): " << cache.get("Spencer").name << std::endl; // Undefined
}

void TestLRUCache()
{
    PrintTitle("LRU Cache Usage Demo");

    case_01_02();
    // case_02();
    case_03();
}