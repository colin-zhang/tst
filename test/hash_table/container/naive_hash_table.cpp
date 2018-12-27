#include "container/mc_rwlock.h"
#include "container/naive_hash_table.h"

NaiveHashTable::NaiveHashTable(uint32_t size, int (*cmp)(HtHandle* handle, const void* key, uint32_t key_len))
    : length_(0)
    , elems_(0)
    , base_list_(NULL)
{
    InitList(size);
    if (cmp == NULL) {
        Cmp = HtHandle::Cmp;
    } else {
       Cmp = cmp;    
    }
}

NaiveHashTable::~NaiveHashTable()
{
    delete[] base_list_->list_;
    delete base_list_;
}

void NaiveHashTable::InitList(uint32_t size)
{
    uint32_t new_length = 1;
    while (new_length < size) {
        new_length <<= 1;
    }
    base_list_ = new NaiveHtHandleBase;
    memset(base_list_, 0, sizeof(*base_list_));
    base_list_->rwlock_ = new McRwLock[new_length];
    HtHandle** new_list = new HtHandle* [new_length];
    memset(new_list, 0, sizeof(new_list[0]) * new_length);
    length_ = new_length;
    base_list_->list_ = new_list;
}

HtHandle* NaiveHashTable::Remove(const void* key, uint32_t key_len, uint32_t hash)
{
    uint32_t idx = hash & (length_ - 1);
    McWriteLock write_lock(&base_list_->rwlock_[idx]);
    HtHandle** ptr = &base_list_->list_[idx];
    while (*ptr != NULL && ((*ptr)->hash != hash || Cmp(*ptr, key, key_len))) {
        ptr = &(*ptr)->next_hash;
    }
    HtHandle* result = *ptr;
    if (result != NULL) {
        *ptr = result->next_hash;
        elems_--;
    }
    return result;
}

HtHandle* NaiveHashTable::Insert(HtHandle* h) 
{
    uint32_t idx = h->hash & (length_ - 1);
    McWriteLock write_lock(&base_list_->rwlock_[idx]);
    HtHandle** ptr = &base_list_->list_[idx];
    while (*ptr != NULL && ((*ptr)->hash != h->hash || Cmp(*ptr, h->key, h->key_len))) {
        ptr = &(*ptr)->next_hash;
    }

    HtHandle* old = *ptr;
    h->next_hash = (old == NULL ? NULL : old->next_hash);
    *ptr = h;
    if (old == NULL) {
        elems_++;
        if (elems_ > length_) {
            //printf("%s\n", "full !!!!");
        }
    }
    return old;
}

HtHandle* NaiveHashTable::Lookup(const void* key, uint32_t key_len, uint32_t hash)
{
    uint32_t idx = hash & (length_ - 1);
    McReadLock read_lock(&base_list_->rwlock_[idx]);
    HtHandle** ptr = &base_list_->list_[idx];
    while (*ptr != NULL && ((*ptr)->hash != hash || Cmp(*ptr, key, key_len))) {
        ptr = &(*ptr)->next_hash;
    }
    return *ptr;
}
