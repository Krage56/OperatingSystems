#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <sys/sem.h>
#include <time.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <errno.h>
#define true 1
#define MESSAGE_SIZE 64

char mem_name[] = "./slava_ukraine";
char sem_name[] = "./wonderful_seal";
int proj_id = 545;
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

void write_message(void *shared_mem_obj)
{
    char *shared_data;
    shared_data = get_inform_string();
    memcpy(shared_mem_obj, shared_data, MESSAGE_SIZE + 1);
    free(shared_data);
    // sleep(2);
}

void writing_process(int semid)
{
    struct sembuf mutex = {0, -1, IPC_NOWAIT | SEM_UNDO},
                  perm_get = {1, -2, SEM_UNDO},
                  perm_set = {1, 2, 0};
    if (semop(semid, &mutex, 1) < 0)
    {
        if (errno == EAGAIN)
        {
            printf("Some writer is already working\n");
        }
        printf("%d\n", errno);
        exit(-1);
    }
    //Если можно писать, захватим разделяемую память
    int f = open(mem_name, O_CREAT | 0666 | O_EXCL);
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

    key_t k = ftok(mem_name, proj_id);
    //Монтирование памяти
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
            if ((shmid = shmget(k, 4096, 0)) < 0)
            {
                printf("Can\'t find shared memory\n");
                exit(-1);
            }
        }
    }
    printf("I'm going to write!\n");
    void *shared_mem_obj = shmat(shmid, 0, 0);
    while (true)
    {
        semop(semid, &perm_get, 1);
        printf("Sender is writing\n");
        write_message(shared_mem_obj);
        semop(semid, &perm_set, 1);
        printf("Sender finished\n");
        sleep(2);
    }
}

//init values
struct sembuf writer = {0, 1, IPC_NOWAIT}, reader = {1, 2, 0};
int main()
{
    int f = open(sem_name, O_CREAT | 0666 | O_EXCL);
    //Проверка на существование файла sem_name
    if (f != -1)
    {
        close(f);
    }
    else
    {
        if (errno != EEXIST)
        {
            printf("Can\'t open file for semaphore\n");
            exit(-1);
        }
    }

    key_t sem_key = ftok(sem_name, proj_id);
    int nsem = 2;
    int semid = semget(sem_key, nsem, IPC_CREAT | IPC_EXCL | S_IRWXU);
    if (semid < 0)
    {
        if (errno != EEXIST)
        {
            printf("Can\'t create semaphore\n");
            exit(-1);
        }
        else
        {
            if ((semid = semget(sem_key, nsem, S_IRWXU)) < 0)
            {
                printf("Can\'t find semaphore\n");
                exit(-1);
            }
        }
    }
    else
    {
        //init the scope of semaphores
        struct sembuf sops[2];
        sops[0] = writer;
        sops[1] = reader;
        int s = semop(semid, sops, 2);
    }
    writing_process(semid);
    return 0;
}