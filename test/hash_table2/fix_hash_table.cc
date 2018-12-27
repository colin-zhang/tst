#include "stdio.h"
#include "string.h"
#include "fix_hash_table.h"

void HashTable::Internal() 
{
#if HASH_TABLE_DEBUG
    printf("length_ = %u, elems_ = %u \n", length_, elems_);
    for (int i = 0; i < length_; ++i) {
        HtHandle* h = base_list_->list_[i];
        if (h != NULL) {
            while (h != NULL) {
                printf("key(%s),hash(%u) --> ", h->key, h->hash);
                h = h->next_hash;
            }
            printf("\n");
        } else {
            printf("null\n");
        }
    }
    printf("\n");
#endif
}

HashTable::HashTable(uint32_t size, int (*cmp)(HtHandle* handle, const void* key, uint32_t key_len))
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

HashTable::~HashTable()
{
    delete[] base_list_->list_;
    delete base_list_;
}

void HashTable::InitList(uint32_t size)
{
    uint32_t new_length = 1;
    while (new_length < size) {
        new_length <<= 1;
    }
    base_list_ = new HtHandleBase;
    memset(base_list_, 0, sizeof(*base_list_));
    HtHandle** new_list = new HtHandle* [new_length];
    memset(new_list, 0, sizeof(new_list[0]) * new_length);
    length_ = new_length;
    base_list_->list_ = new_list;
}

HtHandle** HashTable::FindPointer(const void* key, uint32_t key_len, uint32_t hash) 
{
#if HASH_TABLE_DEBUG
    assert(key != NULL);
    assert(key_len > 0);
#endif
    HtHandle** ptr = &base_list_->list_[hash & (length_ - 1)];
    while (*ptr != NULL && ((*ptr)->hash != hash || Cmp(*ptr, key, key_len))) {
        ptr = &(*ptr)->next_hash;
    }
    return ptr;
}

HtHandle* HashTable::Remove(const void* key, uint32_t key_len, uint32_t hash)
{
    HtHandle** ptr = FindPointer(key, key_len, hash);
    HtHandle* result = *ptr;
    if (result != NULL) {
        *ptr = result->next_hash;
        elems_--;
    }
    return result;
}

HtHandle* HashTable::Insert(HtHandle* h) 
{
    HtHandle** ptr = FindPointer(h->key, h->key_len, h->hash);
    HtHandle* old = *ptr;
    h->next_hash = (old == NULL ? NULL : old->next_hash);
    *ptr = h;
    if (old == NULL) {
        elems_++;
        if (elems_ > length_) {
                printf("%s\n", "full !!!!");
        }
    }
#if HASH_TABLE_DEBUG
    Internal();
#endif
    return old;
}

HtHandle* HashTable::Lookup(const void* key, uint32_t key_len, uint32_t hash)
{
    return *FindPointer(key, key_len, hash);
}

