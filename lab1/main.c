#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
void exit_handler()
{
    printf("The process %d is died!\n", getpid());
}
int main()
{
    atexit(exit_handler);
    printf("I'm living in my own!\n");
    sleep(10);
    return 0;
}