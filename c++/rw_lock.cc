#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <xmmintrin.h>
#include <string.h>
#include <unistd.h>
#include <math.h> 
#include <sched.h>
#include <pthread.h>
#include <sys/time.h>

#include <map>
#include <vector>
#include <string>

typedef struct rw_lock {
    volatile int32_t cnt; /**< -1 when W lock held, > 0 when R locks held. */
    rw_lock() : cnt(0) {};
} rwlock_t;

#define RWLOCK_INITIALIZER { 0 }

static inline void
rwlock_init(rwlock_t *rwl)
{
    rwl->cnt = 0;
}

static void rwlock_read_lock(rwlock_t *rwl)
{
    int32_t x;
    int success = 0;

    while (success == 0) {
        x = rwl->cnt;
        /* write lock is held */
        if (x < 0) {
            _mm_pause();
            continue;
        }
        success = __sync_bool_compare_and_swap((volatile uint32_t *)&rwl->cnt, x, x + 1);
    }
}

static bool rwlock_read_trylock(rwlock_t *rwl)
{
    //
    // int32_t x = rwl->cnt;
    // if (x < 0) {
    //     return 0;
    // }
    // return __sync_add_and_fetch(&rwl->cnt, 1) != 0;
    int32_t x;
    int success = 0;

    while (success == 0) {
        x = rwl->cnt;
        /* write lock is held */
        if (x < 0) {
            return 0;
        }
        success = __sync_bool_compare_and_swap((volatile uint32_t *)&rwl->cnt, x, x + 1);
    }
    return 1;
}

static void rwlock_read_unlock(rwlock_t *rwl)
{
    __sync_fetch_and_sub(&rwl->cnt, 1);
}

static void rwlock_write_lock(rwlock_t *rwl)
{
    int32_t x;
    int success = 0;

    while (success == 0) {
        x = rwl->cnt;
        /* a lock is held */
        if (x != 0) {
            _mm_pause();
            continue;
        }
        success = __sync_bool_compare_and_swap((volatile uint32_t *)&rwl->cnt, 0, -1);
    }
}

static void rwlock_write_unlock(rwlock_t *rwl)
{
    __sync_fetch_and_add(&rwl->cnt, 1);
}


struct ThreadInfo
{
    pthread_t tid;
    int core_num;
    int run;
};

static rwlock_t g_lock2;
static pthread_rwlock_t g_lock; 

static volatile int g_running = 0;
static volatile int g_locking = 0;
static volatile uint64_t g_ref = 0;

std::map<uint64_t, std::string> g_map;

uint64_t g_cnt = 0;

uint64_t getCurrTV()
{
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec *1000 + tv.tv_usec /1000;
}

double cal(void)
{
    int i = 0;
    double y = 0;
    double x = 33333333;
    for (i = 0; i < 1000; i++) {
        y += sqrt(x) + sqrt(x -i) + sqrt(x + i) + sin(y);
        x++;
    }
    return y;
}

void set_map(int x)
{
    int i;
    char buf[32] = {0};

    for (i = 0; i < x; i++) {
        snprintf(buf, sizeof(buf), "n=%d", i);
        g_map[i] = std::string(buf);
    }

}

int add_ref()
{
    if (g_locking) {
        return 0; 
    }
    return __sync_add_and_fetch(&g_ref, 1);
}

void sub_ref()
{
   __sync_fetch_and_sub(&g_ref, 1); 
}

void* load(void* arg)
{
    cpu_set_t mask;
    int core = (int)(*(int*)arg);

    printf("core id=%d \n", core);
    uint64_t n = 0;
    char buf[32] = {0};

    CPU_ZERO(&mask);
    CPU_SET(core, &mask);
    if (pthread_setaffinity_np(pthread_self(), sizeof(mask), &mask) < 0) {
        fprintf(stderr, "set thread affinity failed\n");
    }

    while (g_running) {
        if (core == 1) {
            //snprintf(buf, sizeof(buf), "n=%lu", n);
            //printf("%s\n", buf);
            //g_map[n] = std::string(buf);
        } else {
            //if (rwlock_read_trylock(&g_lock2)) 
            //pthread_rwlock_rdlock(&g_lock);
            //if (!pthread_rwlock_tryrdlock(&g_lock))
            if (add_ref())
            {
                //rwlock_read_lock(&g_lock2);
                auto it = g_map.find(100);
                if (it == g_map.end()) {
                    printf("%s\n", "can not find 100");
                } else {
                    if ( n % 999999 == 0) {
                        printf("core%02d:%s, n = %lu\n", core, it->second.c_str(), n);
                    }
                }
                //pthread_rwlock_unlock(&g_lock);
                //rwlock_read_unlock(&g_lock2);
                sub_ref();
            }
            //pthread_rwlock_unlock(&g_lock);

        }
        n++;
        __sync_fetch_and_add(&g_cnt, 1);
    }
    pthread_exit(NULL);
    return NULL;
}


int main(int argc, char const *argv[])
{
    int i, flag;
    int num = sysconf(_SC_NPROCESSORS_CONF);
    struct ThreadInfo* thread_info = NULL;
    float rate;

    uint64_t a , b;

    g_running = 1;
    set_map(1000);
    //rwlock_init(&g_lock2);
    pthread_rwlock_init(&g_lock, NULL);

    thread_info = (struct ThreadInfo*)calloc(sizeof(struct ThreadInfo), num);
    if (thread_info == NULL) {
        fprintf(stderr, "thread_info is NULL\n");
        exit(-1);
    }

    a = getCurrTV(); 
    for (i = 0; i < num; i++) {
        thread_info[i].core_num = i;
        thread_info[i].run = 0;
        if (pthread_create(&thread_info[i].tid, NULL, load, (void*)&thread_info[i].core_num) != 0) {
            fprintf(stderr, "thread create failed\n");
            break;
        }
        thread_info[i].run = 1;
    }

    while (1) {
        printf("g_map size = %lu\n", g_map.size());
	
        //rwlock_write_lock(&g_lock2);
        //pthread_rwlock_wrlock(&g_lock);
        g_locking = 1;
        if (g_locking) {
            _mm_pause();
	    while ( !__sync_bool_compare_and_swap(&g_ref, 0, -1)) {
	           _mm_pause();
            }
            g_map.clear();
            set_map(1000);
            g_ref = 0;
            g_locking = 0;
        }
        //rwlock_write_unlock(&g_lock2);
        //pthread_rwlock_unlock(&g_lock);
        printf("g_map size = %lu\n", g_map.size());
        sleep(5);
	if (g_cnt > 10000 * 10000 * 1) {
		g_running = 0;
		break;
	}
    }
    b = getCurrTV();
    printf("b - a = %lu\n", b - a);

    for (i = 0; i < num; i++) {
        if (thread_info[i].run) {
            pthread_join(thread_info[i].tid, NULL);
        }
    }

    free(thread_info);
    return 0;
}


