#include "pthreadpool.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <signal.h>
#include <errno.h>

static tpool_t*tpool = NULL;

//工作者线程，从任务链表中取出任务并执行
static void *thread_function(void *arg)
{
    tpool_work_t *work;

    while(1)
    {
        //任务队列为空，且线程池未关闭，线程阻塞等待任务
        pthread_mutex_lock(&tpool->queue_lock);
        while(!tpool->queue_head && !tpool->shutdown)
        {
            pthread_cond_wait(&tpool->queue_ready,&tpool->queue_lock);
        }

        //是否销毁线程
        if(tpool->shutdown)
        {
            pthread_mutex_unlock(&tpool->queue_lock);
            pthread_exit(NULL);
        }

        //取出任务，执行任务
        work = tpool->queue_head;
        tpool->queue_head = tpool->queue_head->next;
        pthread_mutex_unlock(&tpool->queue_lock);
        work->function(work->arg);

        //线程完成后，释放任务
        // free(work->arg);
        free(work);
    }
    return NULL;
}

//创建线程池
int tpool_create(int maxNum)
{
    int i;

    //创建线程池结构体
    tpool = calloc(1,sizeof(tpool_t));
    if(!tpool)
    {
        perror("create error!");
        exit(1);
    }

    //初始化任务链表，互斥量，条件变量
    tpool->maxNum = maxNum;
    tpool->shutdown = 0;
    tpool->queue_head = NULL;
    tpool->queue_tail = NULL;
    if(pthread_mutex_init(&tpool->queue_lock,NULL) != 0)
    {
        perror("mutex error!");
        exit(1);
    }
    if(pthread_cond_init(&tpool->queue_ready,NULL) != 0)
    {
        perror("cond error!");
        exit(1);
    } 

    //创建worker线程
    tpool->threadID = calloc(maxNum,sizeof(pthread_t));
    if(!tpool->threadID)
    {
        perror("threadID error!");
        exit(1);
    } 
    for(i=0; i<maxNum; i++)
    {
        pthread_create(&tpool->threadID[i],NULL,thread_function,NULL);
        // pthread_detach(threadID[i]);
    }
    return 0;
}

//向线程池添加任务
int tpool_add_work(void *(*function)(void*),void *arg)
{
    //work指向等待加入的任务
    tpool_work_t *work;
    if(!function)
    {
        perror("add error!");
        return -1;
    }

    work = malloc(sizeof(tpool_work_t));

    work->function = function;
    work->arg = arg;
    work->next = NULL;

    //将任务结点添加到任务链表
    pthread_mutex_lock(&tpool->queue_lock);
    //链表为空
    if(!tpool->queue_head)
    {
        tpool->queue_head = work;
        tpool->queue_tail = work;
    }
    //链表非空
    else
    {
        tpool->queue_tail->next = work;
        tpool->queue_tail = tpool->queue_tail->next;
    }
    //通知工作者线程，有新任务
    pthread_cond_signal(&tpool->queue_ready);
    pthread_mutex_unlock(&tpool->queue_lock);
    return 0;
}

//销毁线程池
void tpool_destroy()
{
    int i;
    tpool_work_t*member;
    if(tpool->shutdown)
    {
        return;
    }
    //关闭线程池开关
    tpool->shutdown = 1;

    //唤醒所有阻塞的线程
    pthread_mutex_lock(&tpool->queue_lock);
    pthread_cond_broadcast(&tpool->queue_ready);
    pthread_mutex_unlock(&tpool->queue_lock);

    //回收结束线程剩余资源
    for(i=0; i<tpool->maxNum; i++)
    {
        pthread_join(tpool->threadID[i],NULL);
    }

    //释放threadID数组
    free(tpool->threadID);

    //释放未完成的任务
    while(tpool->queue_head)
    {
        member = tpool->queue_head;
        tpool->queue_head = tpool->queue_head->next;
        // free(member->arg);
        free(member);
    }

    //销毁互斥量，条件变量
    pthread_mutex_destroy(&tpool->queue_lock);
    pthread_cond_destroy(&tpool->queue_ready);

    //释放进程池结构体
    free(tpool);
}
