#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/file.h>
#include <sys/stat.h>
#include <time.h>

char *get_time(time_t t)
{
    char *buff = ctime(&t);
    return buff;
}

char *str_from_pid_t(pid_t num)
{
    char *str = malloc(40 + 1);
    snprintf(str, 40, "%d", num);
    return str;
}

int main()
{
    pid_t pid;
    //массив для дескрипторов
    int pipes[2];
    pipe(pipes);
    pid = fork();
    char *currentTime;
    //если родительский процесс
    if (pid)
    {
        //закрыть канал на чтение
        close(pipes[0]);
        //Создать строку-сообщение
        currentTime = get_time(time(NULL));
        char *ppid = str_from_pid_t(getpid());
        char *message = malloc(20 + 40 + 1);
        memcpy(message, currentTime, 20);
        memcpy(message + 20, ppid, 40 + 1);
        sleep(5);
        //Записать в поток
        write(pipes[1], message, 20 + 40 + 1);
        close(pipes[1]);
        //Очистить память
        free(message);
        //Дождёмся выхода из дочернего процесса
        waitpid(pid, 0, 0);
        printf("Pipe-part is finished\n");
    }
    //если процесс дочерний
    else
    {
        close(pipes[1]);
        char message[61];
        size_t r = read(pipes[0], &message, 61);
        message[60] = '\0';
        printf("%s\n", message);
        time(NULL);
        currentTime = get_time(time(NULL));
        printf("%s here 2021 - current year\n", currentTime);
        close(pipes[0]);
        exit(0);
    }

    pid = fork();
    int fd;
    if (pid)
    {
        mkfifo("fifo", S_IRWXO | S_IRWXG | S_IRWXU);
        fd = open("fifo", O_RDWR);
        currentTime = get_time(time(NULL));
        char *ppid = str_from_pid_t(getpid());
        char *message = malloc(20 + 40 + 1);
        memcpy(message, currentTime, 20);
        memcpy(message + 20, ppid, 40 + 1);
        sleep(5);
        write(fd, message, 60 + 1);
        close(fd);
        waitpid(pid, 0, 0);
        printf("FIFO-part is finished\n");
    }
    else
    {
        fd = open("fifo", O_RDONLY);
        char message[61];
        size_t r = read(fd, &message, 61);
        message[60] = '\0';
        printf("%s\n", message);
        time(NULL);
        currentTime = get_time(time(NULL));
        printf("%s here 2021 - current year\n", currentTime);
        close(fd);
    }
    return 0;
}