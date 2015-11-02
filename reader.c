#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<errno.h>
#include<sys/sem.h>
#include<sys/ipc.h>
#include<sys/types.h>
#include<sys/shm.h>

#define shmsize 10           //shared memory size(to store varible readcount in this case)
#define shmkey 222           //key used for shared memory creation
char* shmpointer;
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
}int creat_init_semph(int key,int initial_val)          //function to create and initiate a semaphor with some intial value
{
	int semid;
	if((semid=semget(key,1, 0666))==-1)
	{
		perror("semget\n");
		exit(1);
	}
	arg.val=initial_val;
	//semctl(semid,0,SETVAL,arg);
	return semid;
}
int creat_shmemory(int key)                           //function to create shared memory
{
	int shmid;
	if((shmid=shmget(key,shmsize,0666))==-1)
	{
		perror("shmget\n");
		exit(1);
	}
	return shmid;
}
char* attach_shmemory(int shmid)                      //function to attach shared memory to the process area
{
	char* shmpointer;
	if((shmpointer=shmat(shmid,NULL,0))==(char*)-1)
	{
		perror("shmat\n");
		exit(1);
	}
	return shmpointer;
}
int getshm()                          //funtion to return the value stored in shared memory in integer form
{
	char* s=shmpointer;
	int rcount=0;
	while(*s!='\0')
	{
		rcount=rcount*10+(*s-'0');
		s++;
	}
	return rcount;
}
void setshm(int rcount)            //function to store readercount in shared memory in string format
{
	char *s=shmpointer;
	char buf[10];
	int i=0;
	while(rcount)
	{
		buf[i++]=(rcount%10)+'0';
		rcount/=10;
	}
	i--;
	while(i>=0)
		*s++=buf[i--];
	*s='\0';
}
int main()
{
	int mutex=creat_init_semph(0x20,1);       //synchronises shared memory between readers(readcount)	
	int writer=creat_init_semph(0x40,1);      //synchronises writer and reader(mutual exclusion between writer and reader)
	int readcount=0;                    //common varible to be stored in shared memory
	shmpointer=attach_shmemory(creat_shmemory(shmkey));              //will point to shared memory area
	//setshm(0);
	while(1)
	{
		printf("waiting for mutex control\n");
		wait(mutex);
		printf("mutex under my control\n");
		readcount=getshm()+1;
		setshm(readcount);     //increment readcount by 1
		printf("readcount: %d\n",readcount);
		if(readcount==1)
		{printf("waiting for writer\n");wait(writer);printf("got writer control\n");}	//if first reader then wait for writer
                printf("writer is %d\n",semctl(writer,0,GETVAL,0));
		printf("leaving mutex control..\n");		
		sleep(10);
		
		signal(mutex);
		printf("reader %d is now reading\n",getpid());
		sleep(20);
		printf("reading over.....");
		printf("waiting for mutex again\n");
		wait(mutex);
		printf("mutex control again\n");
		readcount=getshm()-1;
		setshm(readcount);
		printf("readcount now is%d\n",getshm());
		if(readcount==0)
		{printf("releasing writer control//////////////\n");signal(writer);}
		printf("releasing mutex again\n");
               	
	        signal(mutex);
		printf("mutex released\n");	
				
	}
	return 0;
}



