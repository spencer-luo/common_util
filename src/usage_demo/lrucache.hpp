#include "common.hpp"
#include "lrucache.h"

void case_01()
{
    PrintSubTitle("case 01");

    // 【Case 1】
    //  [[2], [1, 1], [2, 2], [1], [3, 3], [2], [4, 4], [1], [3], [4]]
    // [null, null, null, 1, null, -1, null, -1, 3, 4]
    cutl::LRUCache<int, int> cache(2);
    cache.put(1, 1);
    cache.put(2, 2);
    std::cout << cache.get(1) << std::endl; // 1
    cache.put(3, 3);
    std::cout << cache.get(2) << std::endl; // -1
    cache.put(4, 4);
    std::cout << cache.get(1) << std::endl; // -1
    std::cout << cache.get(3) << std::endl; // 3
    std::cout << cache.get(4) << std::endl; // 4
}

void case_02()
{
    PrintSubTitle("case 02");

    // 【Case 2】
    // ["LRUCache","get","put","get","put","put","get","get"]
    // [[2],[2],[2,6],[1],[1,5],[1,2],[1],[2]]
    cutl::LRUCache<int, int> cache(2);
    std::cout << cache.get(2) << std::endl; // -1
    cache.put(2, 6);
    std::cout << cache.get(1) << std::endl; // -1
    cache.put(1, 5);
    cache.put(1, 2);
    std::cout << cache.get(1) << std::endl; // 2
    std::cout << cache.get(2) << std::endl; // 6
}

void case_03()
{
    PrintSubTitle("case 03");

    // 【Case 3】
    // ["LRUCache","put","put","put","put","get","get"]
    // [[2],[2,1],[1,1],[2,3],[4,1],[1],[2]]
    cutl::LRUCache<int, int> cache(2);
    cache.put(2, 1);
    cache.put(1, 1);
    cache.put(2, 3);
    cache.put(4, 1);
    std::cout << cache.get(1) << std::endl; // -1
    std::cout << cache.get(2) << std::endl; // 3
}

void TestLRUCache()
{
    PrintTitle("LRU Cache Usage Demo");

    case_01();
    case_02();
    case_03();
}