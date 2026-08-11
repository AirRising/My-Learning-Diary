#include "RingBuffer.h"

int main()
{
    pthread_t inputThread, outputThread;
    char str[] = "Hello, World! This is a demo of the ring buffer.";
    if (pthread_create(&inputThread, NULL, pthreadInput, str) != 0)
    {
        perror("Failed to create input thread");
        return EXIT_FAILURE;
    }

    if (pthread_create(&outputThread, NULL, pthreadOutput, NULL) != 0)
    {
        perror("Failed to create output thread");
        return EXIT_FAILURE;
    }

    pthread_join(inputThread, NULL);
    pthread_join(outputThread, NULL);

    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond);

    return 0;
}
