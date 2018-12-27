#ifndef FIX_HASHTABLE_H_
#define FIX_HASHTABLE_H_

#include <string>
#include <string.h>
#include <stdint.h>
#include <assert.h>
#include <sys/queue.h>

#define HASH_TABLE_DEBUG 0

struct HtHandle
{
    uint32_t hash;
    void* value;
    void* key;
    uint32_t key_len;
    uint32_t thread_id;
    uint32_t id;
    HtHandle* next_hash;
    TAILQ_ENTRY(HtHandle) entry;
    HtHandle()
        : hash(0)
        , key_len(0)
        , key(NULL)
        , value(NULL)
        , next_hash(NULL)
    {
        memset(&entry, 0, sizeof(entry));
    }

    static int Cmp(HtHandle* h, const void* key, uint32_t key_len) 
    {
        assert(h != NULL);
        if (key_len == h->key_len) {
            return memcmp((char*)key, h->key, key_len);
        }
        return 1;
    }
    static void SetValue(HtHandle* h, uint32_t hash, void* key, uint32_t key_len, void* value)
    {
        h->hash = hash;
        h->key = key;
        h->key_len = key_len;
        h->value = value;
    }

    static void InitHanle(HtHandle* h) {
        h->hash = 0;
        h->key_len = 0;
        h->key = NULL;
        h->value = NULL;
        h->next_hash = NULL;
    }
};

struct HtHandleBase
{
    HtHandle** list_;
};

class HashTable
{
public:
    explicit HashTable(uint32_t size, int (*cmp)(HtHandle* h, const void* key, uint32_t key_len));
    ~HashTable();
    HtHandle* Lookup(const void* key, uint32_t key_len, uint32_t hash);
    HtHandle* Insert(HtHandle* h);
    HtHandle* Remove(const void* key, uint32_t key_len, uint32_t hash);
    void InitList(uint32_t size);
private:
    HtHandle** FindPointer(const void* key, uint32_t key_len, uint32_t hash);
    int (*Cmp)(HtHandle* h, const void* key, uint32_t key_len);
    uint32_t length_;
    uint32_t elems_;
    HtHandleBase* base_list_;
};

#endif
