#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/shm.h>
#include <sys/mman.h>
#include <unistd.h>
#include <fcntl.h>

#define SIZE 100

struct memory {
	char buff[SIZE];
	int status, pid1, pid2;
};

struct memory* shmptr;

void handler(int signum)
{
	if(signum == SIGUSR1)
	{
		printf("Received from User2: ");
		puts(shmptr->buff);
	}
}

int main()
{
	int pid = getpid();
	int shmid;
	const char* shm_name = "Memory";
	shmid = shm_open(shm_name, O_CREAT | O_RDWR, 0666);
	ftruncate(shmid, SIZE);
	shmptr = (struct memory*)mmap(0, SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shmid, 0);
	shmptr->pid1 = pid;
	shmptr->status = -1;
	signal(SIGUSR1, handler);

	while(1)
	{
		do
		{
			sleep(1);
			fgets(shmptr->buff, SIZE, stdin);
			shmptr->status = 0;
			kill(shmptr->pid2, SIGUSR2);
		}while(shmptr->status == 0);
	}
	shm_unlink(shm_name);
	shmctl(shmid, IPC_RMID, NULL);
	return 0;
}
