#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<errno.h>
#include<sys/sem.h>
#include<sys/ipc.h>
#include<sys/types.h>

#define right(i) (((i)+1)%5)
#define left(i) (((i)+4)%5)
#define shmsize 10
          //states in shared memory------>0:thinking...1:hungry.....2:eating
	int self[5],mutex,i;
int shmid[5];
char* shmpointer[5];

union semun
{
	int val;
	struct semid_ds *buf;
	unsigned short *array;	
}arg;

void wait(int semid)                            //wait(i.e.,p)
{
	struct sembuf sem={0,-1,0};
	semop(semid,&sem,1);
}

void signal(int semid)                                 //signal(i.e.,v)
{
	struct sembuf sem={0,1,0};
	semop(semid,&sem,1);
}

int creat_init_semph(int key,int initial_val)          //function to create and initiate a semaphor with some intial value
{
	int semid;
	if((semid=semget(key,1, 0666))==-1)
	{
		perror("semget\n");
		exit(1);
	}
	//arg.val=initial_val;
	//semctl(semid,0,SETVAL,arg);
	return semid;
}
int get(int i)
{
	return *shmpointer[i]-'0';
}
void set(int i,int j)
{
	//printf("ging fine");
	*shmpointer[i]=j+'0';
	//printf("segmentation faluitsff");
}
void test(int i)
{
	if((get(right(i))!=2)&&(get(left(i))!=2)&&(get(i)==1))
	{
		set(i,2);
		signal(self[i]);
	}	
}
void pickup(int i) {
    wait(mutex);
    set(i,1);
    test(i);
    signal(mutex);
	if(get(i)!=2)
    wait(self[i]);
}
void putdown(int i) {
    wait(mutex);
    set(i,0);
    test(left(i));
    test(right(i));
    signal(mutex);
}
void shmemory()
{
	if((shmid[0]=shmget(100,shmsize,0666))==-1)
	{
		perror("shmget\n");
		exit(1);
	}
	if((shmid[1]=shmget(200,shmsize,0666))==-1)
	{
		perror("shmget\n");
		exit(1);
	}
	if((shmid[2]=shmget(300,shmsize,0666))==-1)
	{
		perror("shmget\n");
		exit(1);
	}
	if((shmid[3]=shmget(400,shmsize,0666))==-1)
	{
		perror("shmget\n");
		exit(1);
	}
	if((shmid[4]=shmget(500,shmsize,0666))==-1)
	{
		perror("shmget\n");
		exit(1);
	}
	
	if((shmpointer[0]=shmat(shmid[0],NULL,0))==(char*)-1)
	{
		perror("shmat\n");
		exit(1);
	}
	if((shmpointer[1]=shmat(shmid[1],NULL,0))==(char*)-1)
	{
		perror("shmat\n");
		exit(1);
	}
	if((shmpointer[2]=shmat(shmid[2],NULL,0))==(char*)-1)
	{
		perror("shmat\n");
		exit(1);
	}
	if((shmpointer[3]=shmat(shmid[3],NULL,0))==(char*)-1)
	{
		perror("shmat\n");
		exit(1);
	}
	if((shmpointer[4]=shmat(shmid[4],NULL,0))==(char*)-1)
	{
		perror("shmat\n");
		exit(1);
	}
}
int main(int argc,char **argv)
{
	
	
	i=atoi(argv[1]);
	shmemory();
	int j=0;
	
	self[0]=creat_init_semph(0x10,0);
	self[1]=creat_init_semph(0x20,0);
	self[2]=creat_init_semph(0x30,0);
	self[3]=creat_init_semph(0x40,0);
	self[4]=creat_init_semph(0x50,0);
	mutex=creat_init_semph(0x60,1);
	while(1)
	{
		printf("%d is thinking....\n",getpid());
		pickup(i);
		printf("%d is eating.....\n",getpid());
		sleep(5);
		putdown(i);
	}
	
	return 0;
}




