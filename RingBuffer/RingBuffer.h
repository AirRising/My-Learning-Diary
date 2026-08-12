#ifndef RINGBUFFER_H
#define RINGBUFFER_H

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

typedef struct {
    RingBuffer *rb;
    const char *str;
    int done;  // 输入完成标志
} ThreadArgs;

extern int done; // 标志位，表示输入线程是否完成

void rbInit(RingBuffer *rb);
void rbDestroy(RingBuffer *rb);
bool isEmpty(RingBuffer* rb);
int getSize(RingBuffer* rb);
void clearBuffer(RingBuffer* rb);
bool getInput(RingBuffer* rb, char c);
bool getOutput(RingBuffer* rb, char* c);
void* pthreadInput(void* arg);
void* pthreadOutput(void* arg);

#endif