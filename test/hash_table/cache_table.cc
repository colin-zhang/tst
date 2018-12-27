#include <stdio.h>
#include <stdint.h>
#include <assert.h>
#include <string.h>
#include <vector>

inline void GetPowerOf2(uint32_t len, uint32_t* len2)
{
    while (*len2 < len) {
        *len2 *= 2;
    }
}

template <typename T>
class MemoryStack
{
public:
    MemoryStack(uint32_t len)
    :
      index_(0),
      size_(1)
    {
        GetPowerOf2(len, &size_);
        array_ = new T*[size_];
        for (uint32_t i = 0; i < size_; i++) {
            array_[i] = new T;
            //printf("array_[%d] = %p\n", i, array_[i]);  
        }
    }

    ~MemoryStack()
    {
        for (uint32_t i = 0; i < size_; i++) {
            //printf("del, array_[%d] = %p\n", i, array_[i]); 
            delete array_[i];
        }
        delete[] array_;
    }

    T* Take()
    {
        if (index_ == size_) {
            return NULL;
        }
        T* p = array_[index_];
        index_++;
        return p;
    }

    void TakeBack(T* p)
    {
        assert(index_ != 0);
        index_--;
        array_[index_] = p;   
    }

private:
    uint32_t size_;
    uint32_t index_;
    T** array_;
};

struct CacheHandle
{
    void* value;
    CacheHandle* next_hash;
    CacheHandle* next;
    CacheHandle* prev;
    uint32_t hash;
    uint32_t key_len;
    char key[1];
};

inline bool EqualKey(CacheHandle* handle, const char* key, uint32_t len) {
    if (len == handle->key_len) {
        if (0 == memcmp(key, handle->key, len)) {
            return true;
        }
    }
    return false;
}

class HandleList
{
public:
    HandleList()
      : size_(0)
    {
        head_.next = &head_;
        head_.prev = &head_;
    }

    ~HandleList()
    {

    }

    int NewFixSize(uint32_t n, uint32_t key_size, uint32_t value_size)
    {

        return 0;
    }

    void Remove(CacheHandle* e) 
    {
        e->next->prev = e->prev;
        e->prev->next = e->next;
    }

    void Append(CacheHandle* list, CacheHandle* e) 
    {
        e->next = list;
        e->prev = list->prev;
        e->prev->next = e;
        e->next->prev = e;
    }

private:
    uint32_t size_;
    CacheHandle head_;
};

class HandleTable
{
public:
    HandleTable(uint32_t size)
    : length_(1),
      elements_(0),
      list_(NULL)
    {
        GetPowerOf2(size, &length_);
        mask_ = length_ - 1;
        list_ = new CacheHandle*[length_];
    }

    ~HandleTable()
    {
        delete[] list_;
    }

    uint32_t Size()
    {
        return length_;
    }

    CacheHandle** FindPointer(const char* key, uint32_t len, uint32_t hash) 
    {
        CacheHandle** ptr = &list_[hash & mask_];
        while (*ptr != NULL &&
            ((*ptr)->hash != hash || 
             !EqualKey(*ptr, key, len))) {
            ptr = &(*ptr)->next_hash;
        }
        return ptr;
    }

    CacheHandle* Lookup(const char* key, uint32_t len, uint32_t hash) {
        return *FindPointer(key, len, hash);
    }

    CacheHandle* InsertHandle(CacheHandle* h)
    {
        CacheHandle** ptr = FindPointer(h->key, h->key_len, h->hash);
        CacheHandle* old = *ptr;
        h->next_hash = (old == NULL ? NULL : old->next_hash);
        *ptr = h;
        if (old == NULL) {
            ++elements_;
            if (elements_ > length_) {
                //Resize();
            }
        }
        return old;
    }

    CacheHandle* Remove(const char* key, uint32_t len, uint32_t hash) 
    {
        CacheHandle** ptr = FindPointer(key, len, hash);
        CacheHandle* result = *ptr;
        if (result != NULL) {
            *ptr = result->next_hash;
            --elements_;
        }
        return result;
    }

private:
    uint32_t length_;
    uint32_t mask_;
    uint32_t elements_;
    CacheHandle** list_;
};


int main()
{
    char x[1];
    x[0] = 0x31;
    printf("%d\n", *x);
    HandleTable ht(299);
    printf("ht.size = %u\n", ht.Size());

    MemoryStack<CacheHandle> ms(8);

    CacheHandle* p = ms.Take();
    printf("p = %p\n", p);    
    p = ms.Take();
    printf("p = %p\n", p); 
    ms.TakeBack(p);   
    p = ms.Take();
    printf("p = %p\n", p);

    CacheHandle* ch = new CacheHandle;
    printf("%u %u\n", ch->key_len, ch->hash);
}