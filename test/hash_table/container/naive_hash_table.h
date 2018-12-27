#ifndef NAIVE_HASHTABLE_H_
#define NAIVE_HASHTABLE_H_

#include <string>
#include <string.h>
#include <stdint.h>
#include <assert.h>
#include <sys/queue.h>

#include "container/fix_hash_table.h"
#include "container/mc_rwlock.h"

struct NaiveHtHandleBase
{
    HtHandle** list_;
    McRwLock* rwlock_;
};

class NaiveHashTable
{
public:
    explicit NaiveHashTable(uint32_t size, int (*cmp)(HtHandle* h, const void* key, uint32_t key_len));
    ~NaiveHashTable();
    HtHandle* Lookup(const void* key, uint32_t key_len, uint32_t hash);
    HtHandle* Insert(HtHandle* h);
    HtHandle* Remove(const void* key, uint32_t key_len, uint32_t hash);

public:
    void InitList(uint32_t size);
    int (*Cmp)(HtHandle* h, const void* key, uint32_t key_len);
    uint32_t length_;
    uint32_t elems_;
    NaiveHtHandleBase* base_list_;
};


#endif
