#pragma once

#include <cstdint>
#include <mutex>
#include <unordered_map>

namespace cutl
{

template<typename K, typename V>
class lru_cache
{
private:
    // lru_node以 private的方式定义在LRUCache类的内部，防止被外部直接调用
    struct lru_node
    {
        lru_node(K k, V val)
        {
            prev = nullptr;
            next = nullptr;
            key = k;
            value = val;
        }

        lru_node* prev;
        lru_node* next;
        K key;
        V value;
    };

public:
    lru_cache(int capacity)
      : capacity_(capacity)
      , count_(0)
    {
        // std::cout << "lru_cache() called" << std::endl;
    }

    ~lru_cache() { clear(); }

    using visit_lru_node_func = std::function<void(const K& key, const V& value)>;
    void for_each(visit_lru_node_func callback)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        lru_node* itr = head_;
        while (itr)
        {
            callback(itr->key, itr->value);
            itr = itr->next;
        }
    }

    bool exist(K key) const
    {
        std::lock_guard<std::mutex> lock(mutex_);
        return map_.count(key);
    }

    V get(K key)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        // key不存在
        if (!map_.count(key))
        {
            // TODO
            // throw std::out_of_range("key not found");
            return V();
        }

        // key存在
        auto node = map_[key];
        // 移动到队头，get相当于(最近)使用了该元素
        move_to_head(node);

        return node->value;
    }

    void put(K key, V value)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        if (map_.count(key))
        {
            // key在队列中已经存在
            auto node = map_[key];
            node->value = value;
            // 移动对对头，put相当于(最近)使用了该元素
            move_to_head(node);
        }
        else
        {
            // key在队列中不存在
            lru_node* node = new lru_node(key, value);
            // 添加到队列
            push_to_queue(node);
            map_[key] = node;
        }
    }

    void clear()
    {
        // std::cout << "start ~lru_cache() count:" << count_ << std::endl;
        std::lock_guard<std::mutex> lock(mutex_);
        map_.clear();
        lru_node* itr = head_;
        while (itr)
        {
            lru_node* tmp = itr;
            itr = itr->next;
            delete tmp;
        }
        head_ = nullptr;
        tail_ = nullptr;
        count_ = 0;
        // std::cout << "~lru_cache() called, count:" << count_ << std::endl;
    }

private:
    void push_to_queue(lru_node* node)
    {
        if (count_ == 0)
        { // 空队列
            head_ = node;
            tail_ = node;
            count_++;
        }
        else
        { // 插入对头
            head_->prev = node;
            node->next = head_;
            head_ = node;
            head_->prev = nullptr;
            count_++;

            // 超出队列的最大容量，删除队尾元素
            if (count_ > capacity_)
            {
                pop_from_queue();
            }
        }
    }
    void pop_from_queue()
    {
        // 空队列，不做任何处理
        if (count_ == 0)
        {
            return;
        }

        auto key = tail_->key;
        lru_node* tmp = tail_;
        if (count_ == 1)
        { // 只有一个元素，清空队列
            head_ = nullptr;
            tail_ = nullptr;
        }
        else
        { // 出队列：移除队尾元素
            tail_ = tail_->prev;
            tail_->next = nullptr;
        }
        // 删除节点和map_的映射
        map_.erase(key);
        delete tmp;
        count_--;
    }

    void remove_from_queue(lru_node* node)
    {
        // 只有一个元素
        if (node->prev == nullptr && node->next == nullptr)
        {
            head_ = nullptr;
            tail_ = nullptr;
        }
        else if (node->prev == nullptr && node->next != nullptr)
        {
            // 队头元素
            head_ = node->next;
            node->next->prev = nullptr;
        }
        else if (node->prev != nullptr && node->next == nullptr)
        {
            // 队尾元素
            tail_ = node->prev;
            node->prev->next = nullptr;
        }
        else if (node->prev != nullptr && node->next != nullptr)
        {
            // 队中元素
            node->prev->next = node->next;
            node->next->prev = node->prev;
        }
        map_.erase(node->key);
        delete node;
    }

    void move_to_head(lru_node* node)
    {
        // node本身就是对头节点，不做任何操作
        if (node == head_)
        {
            return;
        }

        // 移除元素
        if (node->prev != nullptr && node->next == nullptr)
        {
            // 队尾元素
            tail_ = node->prev;
            tail_->next = nullptr;
        }
        else if (node->prev != nullptr && node->next != nullptr)
        {
            // 队中元素
            node->prev->next = node->next;
            node->next->prev = node->prev;
        }

        // 插入到对头
        head_->prev = node;
        node->next = head_;
        head_ = node;
        head_->prev = nullptr;
    }

private:
    std::mutex mutex_;
    lru_node* head_;
    lru_node* tail_;
    uint64_t capacity_;
    uint64_t count_;
    std::unordered_map<K, lru_node*> map_;
};

} // namespace cutl
