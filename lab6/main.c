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

pthread_mutex_t mutex;
pthread_cond_t cond;
unsigned long long count = 0;

void *reader_handler(void *args)
{
    pthread_mutex_lock(&mutex);
    if (count < 10)
    {
        pthread_cond_wait(&cond, &mutex);
    }
    printf("My TID is %lu, count = %llu\n", (pthread_self()), count);
    sleep(2);
    pthread_mutex_unlock(&mutex);

    return NULL;
}

void *writer_handler(void *args)
{

    while (count < 10)
    {
        pthread_mutex_lock(&mutex);
        ++count;
        pthread_mutex_unlock(&mutex);
        sleep(1);
    }
    pthread_cond_broadcast(&cond);
    return NULL;
}

int main()
{
    const int readres_num = 10, writers_num = 1;
    pthread_t readers[readres_num], writers[writers_num];
    void *(*reader)(void *) = reader_handler;
    void *(*writer)(void *) = writer_handler;
    pthread_mutex_init(&mutex, NULL);
    pthread_cond_init(&cond, NULL);
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

    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&cond);
    return 0;
}