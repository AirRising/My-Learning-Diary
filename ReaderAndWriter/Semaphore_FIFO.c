#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>

#define readThreadCount 8 // 定义线程的数量
#define writeThreadCount 2

typedef struct
{
    int id;
    char msg[50];
} sharedData;

sharedData data = {0, ""}; // 定义共享数据结构
sem_t queueSem; // 排队锁，强制让所有请求按 FIFO 顺序排队
sem_t lockSem; // 临界区互斥锁，保护共享资源
sem_t mutexSem; // 互斥锁，保护共享资源
int readCount = 0; // 读者计数器，记录当前有多少读者正在读取数据

void* WriteThread(void* arg)
{
    sem_wait(&queueSem); // 请求排队锁，按 FIFO 顺序排队
    sem_wait(&lockSem); // 请求临界区互斥锁，保护共享
    sem_post(&queueSem); // 释放排队锁，允许下一个线程进入排队
    data.id = pthread_self(); // 获取当前线程 ID
    snprintf(data.msg, sizeof(data.msg), "写者线程 %d 写入数据", data.id);
    printf("写者线程写入:%s\n", data.msg);
    sem_post(&lockSem); // 释放临界区互斥锁
    return NULL;
}

void* ReadThread(void* arg)
{
    sem_wait(&queueSem); // 请求排队锁，按 FIFO 顺序排队
    sem_wait(&mutexSem); // 请求互斥锁，保护读者计数器
    readCount++; // 增加读者计数器
    if (readCount == 1) sem_wait(&lockSem); // 如果是第一个读者，锁定，防止写者进入
    printf("当前共 %d 个读者，线程读取到:%s\n", readCount, data.msg);
    sem_post(&mutexSem); // 释放互斥锁，允许其他读者增加计数器
    sem_post(&queueSem); // 释放排队锁，允许下一个线程进入队列
    sem_wait(&mutexSem); // 请求互斥锁，保护读者计数器
    readCount--; // 减少读者计数器
    if (readCount == 0) sem_post(&lockSem); // 如果是最后一个读者，释放锁，允许写者进入
    sem_post(&mutexSem); // 释放互斥锁，允许其他读者 
    return NULL;
}

int main()
{
    sem_init(&queueSem, 0, 1);
    sem_init(&lockSem, 0, 1);
    sem_init(&mutexSem, 0, 1);

    pthread_t readThreads[readThreadCount];
    pthread_t writeThreads[writeThreadCount];

    for (int i = 0; i < writeThreadCount; i++)
    {
        pthread_create(&writeThreads[i], NULL, WriteThread, NULL);
    }
    for (int i = 0; i < readThreadCount; i++)
    {
        pthread_create(&readThreads[i], NULL, ReadThread, NULL);
    }

    for (int i = 0; i < readThreadCount; i++)
    {
        pthread_join(readThreads[i], NULL);
    }
    for (int i = 0; i < writeThreadCount; i++)
    {
        pthread_join(writeThreads[i], NULL);
    }

    sem_destroy(&mutexSem);
    sem_destroy(&lockSem);
    sem_destroy(&queueSem);

    return 0;
}