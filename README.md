## 欢迎大家！😄 

这是我自己记录自己学习过程的地方，希望能和大家一起努力！
这是我的主页，主要记录工程常用的代码，算法类代码请看其他的branch!

Welcome everyone!
This is where I record my own learning process. I hope to work hard with you all!

#### 1.RingBuffer
在嵌入式领域中常用环形缓冲器，它不会在运行时申请从堆中释放内存，避免碎片内存，时间复杂度低，最重要的是**天然解耦“中断上下文”和“任务上下文”**。
项目使用两个索引，indexRead和indexWrite，指向buffer的读取和写入位置，
