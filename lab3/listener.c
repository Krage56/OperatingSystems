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
	char *buff = malloc(MESSAGE_SIZE / 2 + 1) /*= ctime(&t)*/;
	ctime_r(&t, buff);
	return buff;
}

int main()
{
	key_t k = ftok(mem_name, 545);
	if (k == -1)
	{
		printf("Can't create shared memory's id\n");
		exit(-1);
	}
	int shmid = shmget(k, 4096, O_RDONLY);
	if (shmid < 0)
	{
		printf("Can't open shared memory\n");
		exit(-1);
	}
	sleep(2);
	void *shared_mem_obj = shmat(shmid, 0, O_RDONLY);
	char buf[MESSAGE_SIZE + 1];
	while (true)
	{
		memcpy(&buf, shared_mem_obj, MESSAGE_SIZE + 1);
		char *t = get_time(time(NULL));
		printf("My current time is %s and my pid is %d\n", t, getpid());
		printf("Meanwhile recieved inf is: %s\n", buf);
		free(t);
		sleep(2);
	}
}
