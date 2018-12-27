#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "container/url_cache.h"
#include "container/mc_rwlock.h"

#include "blake2.h"

static int32_t gid = 0;
int32_t GetThreadNum()
{
    return gid;
}

int32_t GetThreadId() 
{
    static __thread int32_t id = -1;
    if (Unlikely(-1 == id)) {
      id = __sync_fetch_and_add(&gid, 1);
    }
    return id;
}

struct UrlCacheKey
{
    uint8_t hash_str[64];
    char*  url;
    uint32_t timestamp;
    UrlCacheKey()
    : timestamp(0) {}
};

struct UrlCacheValue
{
    uint64_t ref;
    UrlCacheValue()
    : ref(0) {}
};

UrlCacheValue* NaiveUrlCache::Value(HtHandle* h)
{
    return reinterpret_cast<UrlCacheValue*>(h->value);
}

UrlCacheKey* NaiveUrlCache::Key(HtHandle* h)
{
    return reinterpret_cast<UrlCacheKey*>(h->key);
}

uint32_t NaiveUrlCache::Hash(HtHandle* h)
{
    h->hash  = *(reinterpret_cast<uint32_t*>(NaiveUrlCache::Key(h)->hash_str));
    return h->hash;
}


NaiveUrlCache::NaiveUrlCache(uint32_t size, uint32_t thread_num, uint32_t key_len)
    : size_(size)
    , hash_enable_(true)
    , thread_num_(thread_num)
    , key_len_(key_len)
{

#ifdef THREAD_CACHE

#else
    for (int i = 0; i < size; i++) {
        HtHandle* hptr;
        hptr = new HtHandle;
        hptr->key = new UrlCacheKey;
        hptr->value = new UrlCacheValue;
        PushMemStack(hptr);
    }
#endif
    if (key_len <= BLAKE2B_OUTBYTES) {
        hash_enable_ = true;
    } else {
        hash_enable_ = false;
    }

    Init();
}

NaiveUrlCache::NaiveUrlCache()
{

}

int NaiveUrlCache::Setup(uint32_t size, uint32_t thread_num, uint32_t key_len)
{
    size_ = size;
    hash_enable_ = true;
    thread_num_ = thread_num;
    key_len_ = key_len;
    if (key_len <= BLAKE2B_OUTBYTES) {
        hash_enable_ = true;
    } else {
        hash_enable_ = false;
    }
    Init();
}

int NaiveUrlCache::Init()
{
    table_ = new NaiveHashTable(size_ << 1, NULL);
    thread_cache_ = new NaiveUrlThreadCache*[thread_num_];
    memset(thread_cache_, 0, sizeof(NaiveUrlThreadCache*) * thread_num_);
    for (int i = 0; i < thread_num_; i++) {
        thread_cache_[i] = new NaiveUrlThreadCache(size_ / thread_num_, key_len_);
        thread_cache_[i]->Alloc(i);
    }
}

NaiveUrlThreadCache* NaiveUrlCache::ThreadCache() 
{
    uint32_t id = GetThreadId();
    if (Unlikely(thread_cache_[id] == NULL)) {
        thread_cache_[id] = new NaiveUrlThreadCache(size_ / thread_num_, key_len_);
        thread_cache_[id]->Alloc(id);
    }
    return thread_cache_[id];
}

NaiveUrlThreadCache* NaiveUrlCache::ThreadCache(uint32_t thread_id)
{
    uint64_t id = thread_id;
    if (Unlikely(thread_cache_[thread_id] == NULL)) {
        thread_cache_[id] = new NaiveUrlThreadCache(size_ / thread_num_, key_len_);
        thread_cache_[id]->Alloc(id);
    }
    return thread_cache_[id];
}

uint32_t NaiveUrlCache::FormateKey(UrlCacheKey* key, const char* url, int url_len, uint32_t timestamp, uint32_t* hash)
{
    uint8_t blkey[BLAKE2B_KEYBYTES] = {0};
    uint32_t key_len;
    if (hash_enable_) {
        blake2(key->hash_str, url, blkey, BLAKE2B_OUTBYTES, url_len, BLAKE2B_KEYBYTES);
        key_len = BLAKE2B_OUTBYTES;
        *hash = *(reinterpret_cast<uint32_t*>(key->hash_str));
    } else {
        snprintf(key->url, key_len_, "%s", url);
        key_len = url_len;
        *hash = 0x10;
    }
    key->timestamp = timestamp;
    return key_len;
}

void NaiveUrlCache::UpdateHandle(HtHandle* h, UrlCacheKey* key, uint32_t hash, uint32_t ref, uint32_t key_len)
{
    memcpy(h->key, key, sizeof(UrlCacheKey));
    h->hash = hash;
    h->key_len = key_len;
    this->Value(h)->ref = ref + 1;
}

static uint64_t url_cnt = 0;

int NaiveUrlCache::InsertUrl(const char* url, int url_len, uint32_t timestamp)
{
    UrlCacheKey cache_key; 
    uint32_t hash = 0;
    if (Unlikely(this->ThreadCache()->CheckStopping())) {
        return 0;
    }
    uint32_t key_len = FormateKey(&cache_key, url, url_len, timestamp, &hash);
    ThreadCacheRefAuto ref_auto(this->ThreadCache());

    HtHandle* old_ptr = table_->Lookup(&cache_key, key_len, hash);
    if (old_ptr != NULL) {
        __sync_fetch_and_add(&this->Value(old_ptr)->ref, 1);
        //printf("ref = %u \n", this->Value(old_ptr)->ref);
        return 1;
    }

    HtHandle* new_ptr = this->PopMemStack();
    if (new_ptr == NULL) {
        //printf("OUT of ptr, url_cnt=%lu, size=%u, thread_id = %u, thread_num = %u\n", url_cnt, size_, GetThreadId(), GetThreadNum());
        return -1;
    }
    UpdateHandle(new_ptr, &cache_key, hash, 0, key_len);
    HtHandle* insert_ptr = table_->Insert(new_ptr);
    assert(insert_ptr == NULL);
    if (insert_ptr != NULL) {
        this->PushMemStack(insert_ptr);
    } 
    __sync_fetch_and_add(&url_cnt, 1);

    return 1;
}

int NaiveUrlCache::FindUrl(const char* url, int url_len, uint32_t timestamp)
{
    UrlCacheKey cache_key;
    uint32_t hash = 0;
    if (Unlikely(this->ThreadCache()->CheckStopping())) {
        return 0;
    }

    uint32_t key_len = FormateKey(&cache_key, url, url_len, timestamp, &hash);
     ThreadCacheRefAuto ref_auto(this->ThreadCache());

    HtHandle* old = table_->Lookup(&cache_key, key_len, hash);
    if (NULL == old) {
        return 0;
    }
    return 1;
}

int NaiveUrlCache::RemoveUrl(const char* url, int url_len)
{
    if (Unlikely(this->ThreadCache()->CheckStopping())) {
        return 0;
    }
    UrlCacheKey cache_key;
    uint32_t hash = 0;
    ThreadCacheRefAuto ref_auto(this->ThreadCache());

    uint32_t key_len = FormateKey(&cache_key, url, url_len, 0, &hash);
    HtHandle* old = table_->Remove(&cache_key, key_len, hash);
    if (NULL == old) {
        return 0;
    } 
    PushMemStack(old);
    return 1;
}

void NaiveUrlCache::ClearAll()
{
    for (uint32_t i = 0; i < GetThreadNum(); i++) {
        thread_cache_[i]->SetStopping(true);
    }

    uint32_t sum = 0;
    do 
    {
        sum = 0;
        for (uint32_t i = 0; i < GetThreadNum(); i++) {
            if (i != GetThreadId()) {
                sum += thread_cache_[i]->GetRef();
            }
        }
        
    } while (sum != 0);

    for (uint32_t idx = 0; idx < table_->length_; idx++) {
        //printf("table_->length_ = %d\n", table_->length_);
        HtHandle* ptr = table_->base_list_->list_[idx];
        while (ptr != NULL) {
            this->PushMemStack(ptr);
            ptr = ptr->next_hash;
        }
        ptr = table_->base_list_->list_[idx] = NULL;
    }

    for (uint32_t i = 0; i < GetThreadNum(); i++) {
        thread_cache_[i]->SetStopping(false);
    }


}

void NaiveUrlCache::PushMemStack(HtHandle* hd) 
{
#ifdef THREAD_CACHE
    NaiveUrlThreadCache* cache = this->ThreadCache(hd->thread_id);
    cache->Push(hd);

#else
    HtHandle*  curr = stack_top_;
    HtHandle*  old = curr;
    HtHandle::InitHanle(hd);
    hd->next_hash = curr;
    while (old != (curr = __sync_val_compare_and_swap(&stack_top_, old, hd))) {
        old = curr;
        hd->next_hash = curr;       
    }
#endif
}

HtHandle* NaiveUrlCache::PopMemStack() 
{
#ifdef THREAD_CACHE
    NaiveUrlThreadCache* cache = this->ThreadCache();
    HtHandle* handle = cache->Pop();
    return handle;
#else
    HtHandle* curr = stack_top_;
    HtHandle* old = curr;
    while (NULL != curr && old != (curr = __sync_val_compare_and_swap(&stack_top_, old,  curr->next_hash))) {
        old = curr;
    }
    return curr;
#endif
}

NaiveUrlThreadCache::NaiveUrlThreadCache(uint32_t ring_size, uint32_t key_size)
: ring_size_(ring_size)
, key_size_(key_size)
, thread_id_(-1)
, ref_(0)
, stopping_(false)
{

}

int NaiveUrlThreadCache::Alloc(int thread_id)
{
    if (thread_id_  >= 0) {
        return 0;
    }
    thread_id_ = thread_id;
    uint32_t cnt = 0;
    for (uint32_t i = 0; i < kRingNum; i++) {
        alloc_rings_[i] =  new FreeLockRingBuffer<HtHandle*>(
                                                    ring_size_, FreeLockRingBuffer<HtHandle*>::kRingQueueVariable,
                                                    false, false);
        using_rings_[i] =  new FreeLockRingBuffer<HtHandle*>(
                                                    ring_size_, FreeLockRingBuffer<HtHandle*>::kRingQueueVariable,
                                                    false, false);
        for (uint32_t j = 0; j < (ring_size_ / kRingNum); j++) {
            HtHandle* hptr;
            hptr = new HtHandle;
            hptr->key = new UrlCacheKey;
            if (key_size_ > 0) {
            }
            hptr->thread_id = thread_id;
            hptr->id =  cnt++;
            hptr->value = new UrlCacheValue;
            uint32_t free_space;
            alloc_rings_[i]->DoEnqueue(&hptr, 1, &free_space);
            //printf("hptr->thread_id  %u , hptr->id  = %u , ring_size_ = %u\n", hptr->thread_id, hptr->id, ring_size_);
        }
    }
}

NaiveUrlThreadCache::~NaiveUrlThreadCache()
{

}

void NaiveUrlThreadCache::Push(HtHandle* hd)
{
    uint32_t free_space, how_much;
    for (uint32_t i = 0; i < kRingNum; i++) {
        how_much = alloc_rings_[(hd->id + i) & (kRingNum - 1)]->DoEnqueue(&hd, 1, &free_space);
        if (how_much & 0x01) {
            break;
        }
    }
}

HtHandle* NaiveUrlThreadCache::Pop()
{
    HtHandle* handle = NULL;
    uint32_t available, how_much;
    uint32_t idx;
    for (uint32_t i = 0; i < kRingNum; i++) {
        how_much = alloc_rings_[(idx + i) & (kRingNum - 1)]->DoDequeue(&handle, 1, &available);
        if (how_much & 0x01) {
            break;
        }
    }
    return handle;
}

void NaiveUrlThreadCache::CachePush(HtHandle* hd)
{
    uint32_t free_space, how_much;
    for (uint32_t i = 0; i < kRingNum; i++) {
        how_much = using_rings_[(hd->id + i) & (kRingNum - 1)]->DoEnqueue(&hd, 1, &free_space);
        if (how_much & 0x01) {
            break;
        }
    }
}

HtHandle* NaiveUrlThreadCache::CachePop()
{
    HtHandle* handle = NULL;
    uint32_t available, how_much;
    uint32_t idx;
    for (uint32_t i = 0; i < kRingNum; i++) {
        how_much = using_rings_[(idx + i) & (kRingNum - 1)]->DoDequeue(&handle, 1, &available);
        if (how_much & 0x01) {
            break;
        }
    }
    return handle;
}

static McRwLock gInstanceLock;
static NaiveUrlCache* gUrlCache = NULL;
NaiveUrlCache* UrlCacheTestInstance()
{
    if (Unlikely(gUrlCache == NULL))
    {
        McWriteLock lock(&gInstanceLock);
        if (gUrlCache == NULL) {
            gUrlCache = new NaiveUrlCache();
            //gUrlCache->Setup(1 << 10, 4, 0);
            gUrlCache->Setup(32, 4, 0);
        }
    }
    return gUrlCache;
}
