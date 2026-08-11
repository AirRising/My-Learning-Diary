## 主题：RingBuffer 带互斥锁的环形缓冲区

## 功能
多线程安全的环形缓冲区，支持阻塞式读写

### 应用场景
在嵌入式领域中常用环形缓冲器，它不会在运行时申请从堆中释放内存，避免碎片内存，时间复杂度低，最重要的是**天然解耦“中断上下文”和“任务上下文”**。

### 设计思路
项目使用两个索引，`indexRead`和`indexWrite`，指向`buffer`的读取和写入位置，代替指针，buffer最大有maxNum位（define为1024），两个索引前进时会进行`(index + 1) % maxNum`的计算以形成环形缓冲。使用互斥锁和条件变量防止资源竞争引起错误，协调读取`outputThread`和写入`inputThread`两线程之间对公共资源的使用。

程序首先创建两个线程，如果输出线程先拿到锁，因为此时缓存区没有任何数据，则会直接进入`while`循环，进入`cond`条件队列排队，此刻锁立即被释放，然后被输入线程拿到，进入输入线程的线程函数`pthreadInput`，该函数会将`str`中的内容填入`buffer`中，如果缓冲区已满则立即停止进入`cond`条件队列排队——你可能会说这样两个线程都进入了`cond`条件队列排队不就死锁了吗——然而`pthreadInput()`将str填入buffer是调用`getInput()`单个字节单个字节填入的，每填入一个字节就会进行一次`pthread_cond_signal(&cond)`操作唤醒输出线程。这样直到填入完成后`inputThread`将锁释放，进行`outputThread`持有锁的时间，挨个字节将buffer中的内容输出。

之后程序实现完毕，回收线程，销毁锁和条件变量。

### 结果验证
[OK]输出：Hello, World! This is a demo of the ring buffer.