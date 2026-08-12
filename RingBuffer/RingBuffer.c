#include <stdio.h>
#include <pthread.h>
#include <stdbool.h>

#define maxNum 1024

typedef struct {
    char buffer[maxNum];
    int readIdx; // 缓存读取索引
    int writeIdx; // 缓存写入索引
    pthread_mutex_t mutex;
    pthread_cond_t empty; // 表示有空位
    pthread_cond_t full; // 表示有数据
} RingBuffer;

// 初始化环形缓冲区
void rbInit(RingBuffer *rb) {
    rb->readIdx = 0;
    rb->writeIdx = 0;
    pthread_mutex_init(&rb->mutex, NULL);
    pthread_cond_init(&rb->empty, NULL);
    pthread_cond_init(&rb->full, NULL);
}

// 销毁环形缓冲区
void rbDestroy(RingBuffer *rb) {
    pthread_mutex_destroy(&rb->mutex);
    pthread_cond_destroy(&rb->empty);
    pthread_cond_destroy(&rb->full);
}

bool isEmpty(RingBuffer* rb)
{
    return rb->readIdx == rb->writeIdx;
}

int getSize(RingBuffer* rb)
{
    return (rb->writeIdx - rb->readIdx + maxNum) % maxNum;
}

void clearBuffer(RingBuffer* rb)
{
    rb->readIdx = 0;
    rb->writeIdx = 0;
}

bool getInput(RingBuffer* rb, char c)
{
    pthread_mutex_lock(&rb->mutex);
    // 调用者持有互斥锁
    //int nextIndex = (indexWrite + 1) % maxNum; // 下一个写入的索引
    while ((rb->writeIdx + 1) % maxNum == rb->readIdx)  // 缓冲区已满，停止写入
    {
        pthread_cond_wait(&rb->empty, &rb->mutex);
    }
    rb->buffer[rb->writeIdx] = c;
    rb->writeIdx = (rb->writeIdx + 1) % maxNum;
    // 唤醒读取线程,缓冲区现在至少有一个数据
    pthread_cond_signal(&rb->full);
    pthread_mutex_unlock(&rb->mutex);
    return true;
}

bool getOutput(RingBuffer* rb, char* c)
{
    pthread_mutex_lock(&rb->mutex);
    // 调用者持有互斥锁
    while (rb->readIdx == rb->writeIdx)  // 缓冲区为空
    {
        // 等待生产者放入数据
        pthread_cond_wait(&rb->full, &rb->mutex);
    }
    *c = rb->buffer[rb->readIdx];
    rb->readIdx = (rb->readIdx + 1) % maxNum;
    // 唤醒写入线程,缓冲区现在至少有一个空位
    pthread_cond_signal(&rb->empty);
    pthread_mutex_unlock(&rb->mutex);
    return true;
}

// 线程参数结构体，传递参数用
typedef struct {
    RingBuffer *rb;
    const char *str;
    int done;  // 输入完成标志
} ThreadArgs;

void* pthreadInput(void* arg)
{
    ThreadArgs *args = (ThreadArgs *)arg;
    RingBuffer *rb = args->rb;
    const char* p = args->str;

    while (*p != '\0') {
        getInput(rb, *p); // 内部会等待直到有空位
        p++;
    }

    // 生产完毕，通知消费者
    pthread_mutex_lock(&rb->mutex);
    args->done = 1;
    pthread_cond_broadcast(&rb->full);  // 唤醒所有可能等待的消费者
    pthread_mutex_unlock(&rb->mutex);
    return NULL;
}

void* pthreadOutput(void* arg)
{
    ThreadArgs *args = (ThreadArgs *)arg;
    RingBuffer *rb = args->rb;
    char c;
    while (1)
    {
        pthread_mutex_lock(&rb->mutex);
        while (isEmpty(rb))
        {
            if (args->done) {
                pthread_mutex_unlock(&rb->mutex);
                return NULL; // 如果输入线程已经完成且缓冲区为空，则解锁退出
            }
            pthread_cond_wait(&rb->full, &rb->mutex); // 等待生产者放入数据
        }
        c = rb->buffer[rb->readIdx];
        rb->readIdx = (rb->readIdx + 1) % maxNum;

        pthread_cond_signal(&rb->empty); // 唤醒可能等待的生产者
        pthread_mutex_unlock(&rb->mutex);

        printf("%c", c);
        fflush(stdout); // 确保立即输出
    }
    
    return NULL;
}
