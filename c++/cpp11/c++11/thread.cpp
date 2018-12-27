//
// Created by colin on 18-5-17.
//

#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>
#include <string.h>

static pthread_cond_t cond;
static pthread_mutex_t mutex1;
static pthread_mutex_t mutex2;

static bool exit_ = 0;

static void PthreadCall(const char* label, int result) {
    if (result != 0) {
        fprintf(stderr, "pthread %s: %s\n", label, strerror(result));
    }
}


void *thread1_entry(void *arg)
{
    while (!exit_)
    {
        PthreadCall("thread 1 lock ", pthread_mutex_lock(&mutex1));
        printf("cond wait cond1\n");
        //pthread_cond_wait(&cond, &mutex1);
        sleep(3);
        printf("recv cond1\n");
//        PthreadCall("thread 1 unlock ", pthread_mutex_unlock(&mutex1));
//        PthreadCall("thread 1 unlock ", pthread_mutex_unlock(&mutex1));
//        PthreadCall("thread 1 unlock ", pthread_mutex_unlock(&mutex1));
        sleep(1);
    }
}

void *thread2_entry(void *arg)
{
    while (!exit_)
    {
        pthread_mutex_lock(&mutex1);
        printf("cond wait cond2\n");
        //pthread_cond_wait(&cond, &mutex1);
        sleep(3);
        printf("recv cond2\n");
        pthread_mutex_unlock(&mutex1);
        sleep(1);
    }

}

int main(void)
{
    int ret =0;
    pthread_t tid1, tid2, tid3;
    ret = pthread_cond_init(&cond, NULL);
    if(ret < 0)
    {
        printf("pthread_cond_init error\n");
    }
    ret = pthread_mutex_init(&mutex1, NULL);
    if(ret < 0)
    {
        printf("pthread_mutex_init error\n");
    }

    ret = pthread_mutex_init(&mutex2,NULL);
    if(ret < 0)
    {
        printf("pthread_mutex_init error\n");
    }

    ret=  pthread_create(&tid1, NULL, thread1_entry, NULL);
    if(ret < 0)
    {
        printf("pthread_create thread1 error\n");
    }

    ret = pthread_create(&tid2, NULL, thread2_entry, NULL);
    if(ret < 0)
    {
        printf("pthread_create thread2 error\n");
    }

    sleep(5);
    pthread_cond_broadcast(&cond);

    sleep(100);
    exit_ = 1;

    return 0;
}