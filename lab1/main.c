#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
void exit_handler()
{
    printf("The process %d is died! He was a child of %d!\n", getpid(), getppid());
}
int main()
{
    atexit(exit_handler);
    pid_t pid;
    switch (pid = fork())
    {
    case -1:
        perror("fork"); /* произошла ошибка */
        exit(1);        /*выход из родительского процесса*/
    case 0:
        printf(" CHILD: Это процесс-потомок!\n");
        printf(" CHILD: Мой PID -- %d\n", getpid());
        printf(" CHILD: PID моего родителя -- %d\n", getppid());
        sleep(10);
        exit(1);
    default:
        printf("PARENT: Это процесс-родитель!\n");
        printf("PARENT: Мой PID -- %d\n", getpid());
        printf("PARENT: PID моего потомка %d\n", pid);
        printf("PARENT: Я жду, пока потомок не вызовет exit()...\n");
        int status = -1;
        wait(&status); /*Дождёмся выхода из любого дочернего процесса - он всего один*/
    }
    return 0;
}