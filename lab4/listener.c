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

int main()
{
	key_t k = ftok(mem_name, proj_id);
	if (k == -1)
	{
		printf("Can't create shared memory's id\n");
		exit(-1);
	}
	int shmid = shmget(k, 4096, 0666);
	if (shmid < 0)
	{
		printf("Can't open shared memory\n");
		exit(-1);
	}
	key_t sem_key = ftok(sem_name, proj_id);
	int nsem = 2;
	int semid = semget(sem_key, nsem, 0666);
	if (semid < 0)
	{
		printf("Can\'t get semaphore\n");
		exit(-1);
	}
	void *shared_mem_obj = shmat(shmid, 0, 0);
	char buf[MESSAGE_SIZE + 1];
	struct sembuf get_perm = {1, -1, SEM_UNDO},
				  set_perm = {1, 1, 0};
	while (true)
	{
		int s = semop(semid, &get_perm, 1);
		if (s == -1)
		{
			printf("Can't get permission\n");
			exit(-1);
		}
		memcpy(&buf, shared_mem_obj, MESSAGE_SIZE + 1);
		char *t = get_time(time(NULL));
		printf("My current time is %s and my pid is %d\n", t, getpid());
		printf("Meanwhile recieved inf is: %s\n", buf);
		free(t);
		semop(semid, &set_perm, 1);
		sleep(5);
	}
}
