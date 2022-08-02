#ifndef PTHREADPOOL_H
#define PTHREADPOOL_H

#include <pthread.h>

//任务结点
typedef struct tpool_work
{
    void *(*function)(void*);   //任务函数
    void *arg;                  //传入参数
    struct tpool_work *next;
}tpool_work_t;

//线程池
typedef struct tpool
{
    int shutdown;               //销毁线程池
    int maxNum;                 //最大线程数
    pthread_t *threadID;        //线程ID数组首地址
    tpool_work_t *queue_head;   //任务链表队首
    tpool_work_t *queue_tail;
    pthread_mutex_t queue_lock;
    pthread_cond_t queue_ready;
}tpool_t;

//创建线程
int tpool_create(int maxNum);

//销毁线程池
void tpool_destroy();

//向线程池中添加任务
int tpool_add_work(void*(*function)(void *),void*arg);

#endif // !