#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <signal.h>
#include <time.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <errno.h>
#define true 1
#define MESSAGE_SIZE 64

char mem_name[] = "./slava_ukraine";

char *get_time(time_t t)
{
    char *buff = malloc(MESSAGE_SIZE / 2 + 1);
    ctime_r(&t, buff);
    return buff;
}

char *str_from_pid_t(pid_t num)
{
    char *str = malloc((MESSAGE_SIZE) / 2 + 1);
    /*A terminating null character is automatically 
    appended after the content written*/
    snprintf(str, (MESSAGE_SIZE) / 2 + 1, "%d", num);
    return str;
}
char *get_inform_string()
{
    char *message = malloc((MESSAGE_SIZE + 1));
    char *currentTime = get_time(time(NULL));
    pid_t current_pid = getpid();
    char *ppid = str_from_pid_t(current_pid);
    memcpy(message, currentTime, (MESSAGE_SIZE) / 2);
    memcpy(
        message + (MESSAGE_SIZE) / 2,
        ppid,
        (MESSAGE_SIZE) / 2 + 1);
    free(currentTime);
    return message;
}

void write_message(int shmid)
{
    char *shared_data;
    void *shared_mem_obj;
    shared_data = get_inform_string();
    shared_mem_obj = shmat(shmid, 0, O_RDWR);
    memcpy(shared_mem_obj, shared_data, MESSAGE_SIZE + 1);
    while (true)
    {
        free(shared_data);
        sleep(2);
        shared_data = get_inform_string();
        memcpy(shared_mem_obj, shared_data, MESSAGE_SIZE + 1);
    }
}

int main()
{
    int f = open(mem_name, O_CREAT | O_RDWR | O_EXCL);
    //Проверка на существование файла mem_name
    if (f != -1)
    {
        close(f);
    }
    else
    {
        if (errno != EEXIST)
        {
            printf("Can\'t open file for shared memory\n");
            exit(-1);
        }
    }

    key_t k = ftok(mem_name, 545);

    int shmid = shmget(k, 4096, IPC_CREAT | IPC_EXCL | 0666);
    if (shmid < 0)
    {
        if (errno != EEXIST)
        {
            printf("Can\'t create shared memory\n");
            exit(-1);
        }
        else
        {
            if ((shmid = shmget(k, 4096, 0666)) < 0)
            {
                printf("Can\'t find shared memory\n");
                exit(-1);
            }

            struct shmid_ds ds;
            if (shmctl(shmid, IPC_STAT, &ds) == -1)
            {
                printf("Can\'t get information from shared memory\n");
                exit(-1);
            }
            if (ds.shm_nattch > 1)
            {
                printf("Some process is a writer already!\n");
                exit(-1);
            }
            write_message(shmid);
        }
    }
    else
    {
        write_message(shmid);
    }
    return 0;
}