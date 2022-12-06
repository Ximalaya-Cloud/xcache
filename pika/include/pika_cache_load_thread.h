#ifndef PIKA_CACHE_LOAD_THREAD_H_
#define PIKA_CACHE_LOAD_THREAD_H_

#include <string>
#include <vector>
#include <atomic>
#include <unordered_map>

#include "pika_define.h"
#include "pink/include/pink_thread.h"
#include "blackwidow/blackwidow.h"

#define CACHE_LOAD_QUEUE_MAX_SIZE   2048
#define CACHE_VALUE_ITEM_MAX_SIZE   2048
#define CACHE_LOAD_NUM_ONE_TIME     256

class PikaCacheLoadThread : public pink::Thread
{
public:
    PikaCacheLoadThread(int cache_start_pos, int cache_items_per_key);
    ~PikaCacheLoadThread();

    uint64_t AsyncLoadKeysNum(void) { return async_load_keys_num_; }
    uint32_t WaittingLoadKeysNum(void) { return waitting_load_keys_num_; }
    void Push(const char key_type, std::string &key);

private:
    bool LoadKv(std::string &key);
    bool LoadHash(std::string &key);
    bool LoadList(std::string &key);
    bool LoadSet(std::string &key);
    bool LoadZset(std::string &key);
    bool LoadKey(const char key_type, std::string &key);
    virtual void* ThreadMain();

private:
    std::atomic<bool> should_exit_;
    std::deque<std::pair<const char, std::string>> loadkeys_queue_;
    slash::CondVar loadkeys_cond_;
    slash::Mutex loadkeys_mutex_;

    std::unordered_map<std::string, std::string> loadkeys_map_;
    slash::Mutex loadkeys_map_mutex_;

    std::atomic<uint64_t> async_load_keys_num_;
    std::atomic<uint32_t> waitting_load_keys_num_;
    // currently only take effects to zset
    int cache_start_pos_;
    int cache_items_per_key_;
};

#endif
