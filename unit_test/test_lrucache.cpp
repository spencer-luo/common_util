/**
 * @file test_lrucache.cpp
 * @brief Unit tests for cutl::lru_cache.
 */

#include "common_util/lrucache.h"
#include <functional>
#include <gtest/gtest.h>
#include <string>
#include <vector>

TEST(LRUCacheTest, BasicPutGet)
{
    cutl::lru_cache<int, int> cache(2);
    cache.put(1, 100);
    cache.put(2, 200);
    EXPECT_EQ(cache.get(1), 100);
    EXPECT_EQ(cache.get(2), 200);
}

TEST(LRUCacheTest, EvictsLeastRecentlyUsed)
{
    cutl::lru_cache<int, int> cache(2);
    cache.put(1, 1);
    cache.put(2, 2);
    EXPECT_EQ(cache.get(1), 1);
    cache.put(3, 3); // 此时应淘汰 key=2
    // key 2 应被淘汰，get 返回默认值 0
    EXPECT_EQ(cache.get(2), 0);
    EXPECT_EQ(cache.get(1), 1);
    EXPECT_EQ(cache.get(3), 3);
}

TEST(LRUCacheTest, UpdateExistingKey)
{
    cutl::lru_cache<std::string, int> cache(2);
    cache.put("a", 1);
    cache.put("b", 2);
    cache.put("a", 100); // 更新
    EXPECT_EQ(cache.get("a"), 100);
    cache.put("c", 3); // 应淘汰 b（a 是最近使用的）
    EXPECT_EQ(cache.get("b"), 0); // 默认值
    EXPECT_EQ(cache.get("a"), 100);
    EXPECT_EQ(cache.get("c"), 3);
}

TEST(LRUCacheTest, ClearEmptyCache)
{
    cutl::lru_cache<int, int> cache(3);
    cache.put(1, 1);
    cache.put(2, 2);
    cache.put(3, 3);
    cache.clear();
    EXPECT_EQ(cache.get(1), 0);
    EXPECT_EQ(cache.get(2), 0);
    EXPECT_EQ(cache.get(3), 0);
}

TEST(LRUCacheTest, ForEachVisitsInOrderAfterAccess)
{
    cutl::lru_cache<int, int> cache(3);
    cache.put(1, 1);
    cache.put(2, 2);
    cache.put(3, 3);
    cache.get(1); // 1 移到队头

    std::vector<int> keys;
    using Visit = std::function<void(const int&, const int&)>;
    Visit collector = [&keys](const int& k, const int&) { keys.push_back(k); };
    cache.for_each(collector);
    ASSERT_EQ(keys.size(), 3u);
    EXPECT_EQ(keys.front(), 1);
}

TEST(LRUCacheTest, CapacityOneBehavesLikeSingleSlot)
{
    cutl::lru_cache<int, int> cache(1);
    cache.put(1, 1);
    EXPECT_EQ(cache.get(1), 1);
    cache.put(2, 2);
    EXPECT_EQ(cache.get(1), 0); // 已被淘汰
    EXPECT_EQ(cache.get(2), 2);
}

TEST(LRUCacheTest, ExistConstQueriesWork)
{
    cutl::lru_cache<int, int> cache(3);
    cache.put(1, 100);
    cache.put(2, 200);

    // exist() 是 const 方法，需要库内 mutex_ 已声明为 mutable。
    const auto& cref = cache;
    EXPECT_TRUE(cref.exist(1));
    EXPECT_TRUE(cref.exist(2));
    EXPECT_FALSE(cref.exist(99));
}
