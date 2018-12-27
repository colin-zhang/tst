#ifndef MC_RwLock_H_
#define MC_RwLock_H_
//read/write lock for multi core
#include <stdint.h>
#include <emmintrin.h>

typedef struct McRwLock {
    volatile int32_t cnt;
    McRwLock() : cnt(0) {};
} McRwLock;

#define McRwLock_INITIALIZER { 0 }

static inline void
McRwLock_init(McRwLock *rwl, void* p)
{
    rwl->cnt = 0;
}

static inline void
McRwLock_deinit(McRwLock *rwl)
{
    
}

static inline void McRwLock_read_lock(McRwLock *rwl)
{
    int32_t x;
    int success = 0;
    while (success == 0) {
        x = rwl->cnt;
        if (x < 0) {
             _mm_pause();
            continue;
        }
        success = __sync_bool_compare_and_swap((volatile uint32_t *)&rwl->cnt, x, x + 1);
    }
}

static inline void McRwLock_read_unlock(McRwLock *rwl)
{
    __sync_fetch_and_sub(&rwl->cnt, 1);
}

static inline void McRwLock_write_lock(McRwLock *rwl)
{
    int32_t x;
    int success = 0;
    while (success == 0) {
        x = rwl->cnt;
        if (x != 0) {
             _mm_pause();
            continue;
        }
        success = __sync_bool_compare_and_swap((volatile uint32_t *)&rwl->cnt, 0, -1);
    }
}

static inline void McRwLock_write_unlock(McRwLock *rwl)
{
    __sync_fetch_and_add(&rwl->cnt, 1);
}



class McReadLock
{
public:
    McReadLock(McRwLock* rwl) : rwl_(rwl){
        McRwLock_read_lock(rwl_);
    }
    ~McReadLock() {
        McRwLock_read_unlock(rwl_);
    }
private:
    McReadLock(const McReadLock&);
    void operator=(const McReadLock&);
    McRwLock* rwl_;
};

class McWriteLock
{
public:
    McWriteLock(McRwLock* rwl) : rwl_(rwl) {
        McRwLock_write_lock(rwl_);
    }
    ~McWriteLock() {
        McRwLock_write_unlock(rwl_);
    }
private:
    McWriteLock(const McWriteLock&);
    void operator=(const McWriteLock&);
    McRwLock* rwl_;
};

#endif