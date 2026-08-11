#ifndef RINGBUFFER_H
#define RINGBUFFER_H

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdbool.h>

#define maxNum 1024

extern pthread_mutex_t mutex; 
extern pthread_cond_t empty; 
extern pthread_cond_t full; 

extern int done; // 标志位，表示输入线程是否完成

extern char buffer[maxNum];
extern int indexRead; // 缓存读取索引
extern int indexWrite; // 缓存写入索引

bool isEmpty();
int getSize();
void clearBuffer();
bool getInput(char c);
bool getOutput(char* c);
void* pthreadInput(void* arg);
void* pthreadOutput(void* arg);

#endif