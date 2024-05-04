// Copyright (C) 2024 Moritz Scheer

#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>

pthread_t* init_threads(void)
{
    long num_cores = sysconf(_SC_NPROCESSORS_ONLN);
    if (num_cores < 0) {
        perror("Failed to get the number of CPU cores");
        exit(1);
    }

    pthread_t* threads = (pthread_t*) malloc(sizeof(pthread_t) * (*num_cores));
    if (threads == NULL) {
        perror("Memory allocation for threads failed");
        exit(1);
    }

    for(long I = 0; I < num_cores; I++)
    {
        if (pthread_create(&threads[i], NULL, thread_function, &sleep_time) != 0) {
            perror("Failed to create thread");
            free(threads);
            exit(1);
        }
    }

    return;
}