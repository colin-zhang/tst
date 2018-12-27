#ifndef URL_CACHE_H_
#define URL_CACHE_H_

#include <stdint.h>
#include "container/naive_hash_table.h"
#include "container/ring_buffer.h"

#define THREAD_CACHE 1
struct UrlCacheKey;
struct UrlCacheValue;

// struct NaiveUrlThreadCache
// {
//     TAILQ_HEAD(, HtHandle) cache_list;
//     TAILQ_HEAD(, HtHandle) alloc_list;

//     FreeLockRingBuffer<HtHandle*>* using_ring[4];
//     FreeLockRingBuffer<HtHandle*>* alloc_ring[4];

//     NaiveUrlThreadCache()
//     {
//         TAILQ_INIT(&cache_list);
//         TAILQ_INIT(&alloc_list);
//     }
// };

class NaiveUrlThreadCache
{
public:
    static const uint32_t kRingNum=4;
public:
    explicit NaiveUrlThreadCache(uint32_t ring_size, uint32_t key_size);
    ~NaiveUrlThreadCache();

    int Alloc(int thread_id);

    void Push(HtHandle* hd);
    HtHandle* Pop();    
    void CachePush(HtHandle* hd);
    HtHandle* CachePop();

    bool CheckStopping() {
        return stopping_ == true;
    }

    void AddRef()
    {
        ref_++;
    }

    void SubRef()
    {
        ref_--;
    }

    uint32_t GetRef()
    {
        return ref_;
    }

    void SetStopping(bool stop) {
        __sync_lock_test_and_set(&stopping_, stop);
    }

private:
    FreeLockRingBuffer<HtHandle*>* using_rings_[kRingNum];
    FreeLockRingBuffer<HtHandle*>* alloc_rings_[kRingNum];
    uint32_t ring_size_;
    uint32_t key_size_;
    uint32_t ref_;
    bool stopping_;
    int thread_id_;
};

class ThreadCacheRefAuto
{
public:
    ThreadCacheRefAuto(NaiveUrlThreadCache* thread_cache)
    : thread_cache_(thread_cache)
    {
        thread_cache_->AddRef();
    }
    ~ThreadCacheRefAuto() {
        thread_cache_->SubRef();
    }
private:
    NaiveUrlThreadCache* thread_cache_;
};

class NaiveUrlCache
{
public:
    static const uint32_t kKeyLen=2048;
public:
    NaiveUrlCache();
    NaiveUrlCache(uint32_t size, uint32_t thread_num, uint32_t key_len);
    ~NaiveUrlCache();
    int Setup(uint32_t size, uint32_t thread_num, uint32_t key_len);

    int InsertUrl(const char* url, int url_len, uint32_t timestamp);
    int FindUrl(const char* url, int url_len, uint32_t timestamp);
    int RemoveUrl(const char* url, int url_len);
    void ClearAll();

    static UrlCacheValue* Value(HtHandle* h);
    static UrlCacheKey* Key(HtHandle* h);
    static uint32_t Hash(HtHandle* h);
    uint32_t FormateKey(UrlCacheKey* key, const char* url, int url_len, uint32_t timestamp, uint32_t* hash);
    void UpdateHandle(HtHandle* h, UrlCacheKey* key, uint32_t hash, uint32_t ref, uint32_t key_len);
private:
    int Init();
    NaiveUrlThreadCache* ThreadCache();
    NaiveUrlThreadCache* ThreadCache(uint32_t thread_id);
    void PushMemStack(HtHandle* hd);
    HtHandle* PopMemStack();
private:
#if THREAD_CACHE
    NaiveUrlThreadCache** thread_cache_;
#else
    HtHandle* stack_top_;
#endif
    uint32_t size_;
    uint32_t key_len_;
    uint32_t thread_num_;
    bool hash_enable_;
    NaiveHashTable* table_;
};

NaiveUrlCache* UrlCacheTestInstance();

#endif
