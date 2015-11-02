#include<stdio.h>
#include<stdlib.h>
#include<errno.h>
#include<sys/types.h>
#include<sys/sem.h>
#include<sys/ipc.h>

union semun
{
	int val;
	struct semid_ds *buf;
	unsigned short *array;
}arg;
void wait(int semid)
{
	struct sembuf smop={0,-1,0};
	semop(semid,&smop,1); 
}
void signal(int semid)
{
	struct sembuf smop={0,1,0};
	semop(semid,&smop,1); 
}
int creat_init_semph(key_t key,int initial_val)
{
	int semid;
	if((semid=semget(key,1,0666 | IPC_CREAT))==-1)
	{
		perror("semget");
		exit(1);
	}
	arg.val=initial_val;
	//semctl(semid,0,SETVAL,arg);
	return semid;
}
int main()
{
	int full=creat_init_semph(0x20,0);
	int empty=creat_init_semph(0x40,100);
        int mutex=creat_init_semph(0x60,1);
	while(1)
	{
		printf("full is %d and empty is %d\n",semctl(full,0,GETVAL,0),semctl(empty,0,GETVAL,0));
		wait(full);
		wait(mutex);
                //printf("full is %d and empty is %d\n",semctl(full,0,GETVAL,0),semctl(empty,0,GETVAL,0));
		printf("consumer is now consuming a item....\n");
                sleep(5);
		signal(mutex);
		signal(empty);
	}       
	return (0);
}
