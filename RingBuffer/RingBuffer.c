#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>

#define maxNum 1024

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER; 
pthread_cond_t empty = PTHREAD_COND_INITIALIZER; // 表示有空位
pthread_cond_t full = PTHREAD_COND_INITIALIZER; // 表示有数据

int done = 0; // 标志位，表示输入线程是否完成

char buffer[maxNum];
int indexRead = 0; // 缓存读取索引
int indexWrite = 0; // 缓存写入索引

bool isEmpty()
{
    return indexRead == indexWrite;
}

int getSize()
{
    return (indexWrite - indexRead + maxNum) % maxNum;
}

void clearBuffer()
{
    indexRead = 0;
    indexWrite = 0;
}

bool getInput(char c)
{
    // 调用者持有互斥锁
    //int nextIndex = (indexWrite + 1) % maxNum; // 下一个写入的索引
    while ((indexWrite + 1) % maxNum == indexRead)  // 缓冲区已满，停止写入
    {
        pthread_cond_wait(&empty, &mutex);
    }
    buffer[indexWrite] = c;
    indexWrite = (indexWrite + 1) % maxNum;
    // 写入后唤醒消费者（因为缓冲区现在至少有一个字符）
    pthread_cond_signal(&full);
    return true;
}

bool getOutput(char* c)
{
    // 调用者持有互斥锁
    while (indexRead == indexWrite)  // 缓冲区为空，等待生产者写入
    {
        if (done)  // 输入线程已完成，且缓冲区为空
        {
            return false;
        }
        // 否则等待生产者放入数据
        pthread_cond_wait(&full, &mutex);
    }
    *c = buffer[indexRead];
    indexRead = (indexRead + 1) % maxNum;
    // 读取后唤醒写入线程（因为缓冲区现在至少有一个空位）
    pthread_cond_signal(&empty);
    return true;
}

void* pthreadInput(void* arg)
{
    char* str = (char*)arg;
    char* p = str;
    while (*p != '\0')
    {
        pthread_mutex_lock(&mutex);
        getInput(*p); // 内部会等待直到有空位
        p++;
        pthread_mutex_unlock(&mutex);
    }

    // 生产完毕，通知消费者
    pthread_mutex_lock(&mutex);
    done = 1;
    pthread_cond_broadcast(&full);  // 唤醒所有可能等待的消费者
    pthread_mutex_unlock(&mutex);
    return NULL;
}

void* pthreadOutput(void* arg)
{
    char c;
    while (1) {
        pthread_mutex_lock(&mutex);
        bool hasData = getOutput(&c);  // 内部会等待直到有数据或结束
        if (!hasData) {
            pthread_mutex_unlock(&mutex);
            break;
        }
        pthread_mutex_unlock(&mutex);
        printf("%c", c);
        fflush(stdout);  // 实时输出
    }
    printf("\n");
    return NULL;
}
