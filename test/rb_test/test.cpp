#include <signal.h>
#include <iostream>
#include <map>
#include <string>
#include <chrono>
#include <mutex>
#include <functional>
#include <time.h>
#include <vector>
#include <unistd.h>

#include <assert.h>

#include "thread.h"
#include "random.h"
    
#include "ring_buffer.h"

static bool StopRunning = false;

static std::vector<Thread*> gProducerThreads;
static std::vector<Thread*> gConsumerThreads;

static std::vector<uint32_t> gNumberArrays[64];

FreeLockRingBuffer<uint32_t>* gRingBuff;

uint64_t gSum = 0;

struct Rte
{
    int  producer_core_num;
    int  consumer_core_num;
    int  number_num;
    int  ring_mode;
    uint32_t ring_size;
    bool ring_single_producer;
    bool ring_single_consumer;
};

Rte GlobalRte = {
    3,
    1,
    1000 * 10000,
    FreeLockRingBuffer<uint32_t>::kRingQueueVariable,
    4096,
    false,
    true
};


static void signal_handler(int sig)
{
    printf("StopRunning\n\n");
    StopRunning = true;
}

static void Input(ThreadOption& opt)
{
    printf("%s %d started\n", opt.name.c_str(), opt.id);

    auto nums = gNumberArrays[opt.id];
    uint32_t free_space;
    uint64_t sum = 0;

    while (1) {
        if (Unlikely(StopRunning)) {
            break;
        }

        for (auto it : nums) {
            sum += it;
            gRingBuff->DoEnqueue(it, &free_space);
            assert(free_space != 0);
        }
        break;
    }
    printf("%s %d exited!, sum = %lu\n", opt.name.c_str(), opt.id, sum);
}

static void Sum(ThreadOption& opt)
{
    printf("%s %d started\n", opt.name.c_str(), opt.id);

    uint32_t available;
    uint32_t threshold = 512;
    uint32_t* data = new uint32_t[threshold];

    uint32_t how_much;
    uint64_t sum = 0;

    while (1) {
        if (Unlikely(StopRunning)) {
            break;
        }

        how_much = gRingBuff->DoDequeue(data, threshold, &available);

        for (auto i = 0; i < how_much; i++) {
            sum += data[i];
        }

        //usleep(1);
    }
    printf("%s %d exited! ", opt.name.c_str(), opt.id);
    printf("sum = %lu \n", sum);
    assert(sum == gSum);

}

void ThreadInit()
{

    for (int i = 0; i < GlobalRte.producer_core_num; i++) {
        Thread* thd = new Thread(Input);
        thd->Option.name = "Input thread";
        thd->Option.id = i;
        thd->Option.cores.push_back(i + 1 + GlobalRte.producer_core_num);
        gProducerThreads.push_back(thd);
    }

    for (int i = 0; i < GlobalRte.consumer_core_num; i++) {
        Thread* thd = new Thread(Sum);
        thd->Option.name = "Sum thread";
        thd->Option.id = i;
        thd->Option.cores.push_back(i + 1);
        gConsumerThreads.push_back(thd);
    }

    for (auto th : gConsumerThreads) {
        th->Start();
    }

    for (auto th : gProducerThreads) {
        th->Start();
    }


}

void ConsumerThreadDestory()
{
    for (auto th : gConsumerThreads) {
        th->Join();
        delete th;
    }
}

void ProducerThreadDestory()
{
    for (auto th : gProducerThreads) {
        th->Join();
        delete th;
    }
}

void Init()
{
    signal(SIGINT, signal_handler);

    gRingBuff = new FreeLockRingBuffer<uint32_t>(GlobalRte.ring_size,
            GlobalRte.ring_mode,
            GlobalRte.ring_single_producer,
            GlobalRte.ring_single_consumer);

    Random random(time(nullptr));

    for (auto i = 0; i < GlobalRte.producer_core_num; i++) {
        for (auto j = 0 ; j < GlobalRte.number_num; j++) {
            uint32_t next = random.Next();
            gSum += next;
            gNumberArrays[i].push_back(next);
        }
    }
    printf("gSum = %lu \n", gSum);
    ThreadInit();
}

int main()
{
    Init();
    ProducerThreadDestory();
    StopRunning = 1;
    ConsumerThreadDestory();
    return 0;
}
