#include "RingBuffer.h"
#include <stdlib.h>

int main()
{
    pthread_t inputThread, outputThread;
    RingBuffer rb;
    ThreadArgs args;

    rbInit(&rb);

    args.rb = &rb;
    args.str = "Hello, World! This is a demo of the ring buffer.\n";
    args.done = 0;

    if (pthread_create(&inputThread, NULL, pthreadInput, &args) != 0)
    {
        perror("Failed to create input thread");
        return EXIT_FAILURE;
    }

    if (pthread_create(&outputThread, NULL, pthreadOutput, &args) != 0)
    {
        perror("Failed to create output thread");
        return EXIT_FAILURE;
    }

    pthread_join(inputThread, NULL);
    pthread_join(outputThread, NULL);

    rbDestroy(&rb);

    return 0;
}
