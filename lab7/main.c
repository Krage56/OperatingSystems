#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <pthread.h>
#include <time.h>

#include <sys/shm.h>
#include <errno.h>
#define true 1

pthread_rwlock_t rwlock;
unsigned long long count;

void *
reader_handler(void *args)
{
    while (true)
    {
        pthread_rwlock_rdlock(&rwlock);
        printf("My TID is %lu, count = %llu\n", (pthread_self()), count);
        if (count >= 100)
        {
            break;
        }
        pthread_rwlock_unlock(&rwlock);
        usleep(5);
    }
    return NULL;
}

void *writer_handler(void *args)
{

    while (count < 100)
    {
        pthread_rwlock_wrlock(&rwlock);
        ++count;
        usleep(100000);
        pthread_rwlock_unlock(&rwlock);
    }
    return NULL;
}

int main()
{
    const int readres_num = 10, writers_num = 1;
    pthread_t readers[readres_num], writers[writers_num];
    void *(*reader)(void *) = reader_handler;
    void *(*writer)(void *) = writer_handler;
    pthread_rwlock_init(&rwlock, NULL);
    // rwlock = PTHREAD_RWLOCK_INITIALIZER;
    for (int i = 0; i < writers_num; ++i)
    {
        pthread_create(&(writers[i]), NULL, writer, NULL);
    }
    for (int i = 0; i < readres_num; ++i)
    {
        pthread_create(&(readers[i]), NULL, reader, NULL);
    }

    for (int i = 0; i < writers_num; ++i)
    {
        pthread_join(writers[i], NULL);
    }
    for (int i = 0; i < readres_num; ++i)
    {
        pthread_join(readers[i], NULL);
    }

    pthread_rwlock_destroy(&rwlock);
    return 0;
}