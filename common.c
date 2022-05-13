#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h> 
#include <time.h>
#include <signal.h>

#define TIME_MAX 12
char *myTime(char *tmBuf)
{
	if (!tmBuf)
	{
		perror("error mT1: myTime(null)");
		return NULL;
	}
	
	struct timespec mtime;
	if (clock_gettime(CLOCK_REALTIME, &mtime) < 0)
	{
		perror("error mT3 : clock_gettime() failed");
		return NULL;
	}
	
	struct tm *hms = localtime(&(mtime.tv_sec));
	if (!hms)
	{
		perror("error mT4: localtime() failed");
		return NULL;
	}
		
	if (sprintf(tmBuf, "%d:%d:%d:%d", hms->tm_hour, hms->tm_min, hms->tm_sec, (int)mtime.tv_nsec/1000000) < 0)
	{
		perror("error mT2 : sprintf() failed");
		return NULL;
	}
	return tmBuf;
}

static int NoMessage = 0;
int pid1;
static pid_t Children[2]; 

void processCommunicate(pid_t pid, pid_t ppid, char *tmBuf)
{
	int res_kill;
	int ws;
	for (int i = 0; i < 5; ++i)
	{ 
/*		int wait_pid = wait(&ws);
		if (wait_pid == ppid)
		{
			if (!WIFEXITED(ws))
			{
				perror("%d get signal");
			}
		} 
		else if (wait_pid == -1)
			perror("error -1");
	*/	
					
		res_kill = kill(ppid, SIGUSR2);
		if (res_kill == -1)
		{}
		else
		{
			printf("#%d child(%d, %d) send to parent(%d)\t%s\n", ++NoMessage, pid, ppid, ppid, myTime(tmBuf) != NULL ? tmBuf : "time error");
		}
		
	}
}

int createChildProcesses(int nChild, char *tmBuf)
{
	if (nChild == 0)
		return 0;
	pid_t p = fork();
	pid_t ppid;
	pid_t pid;
	
	switch(p)
	{
	case -1:
		perror("fork() failed");
		return -1;
	case 0:
		ppid = getppid();
		pid = getpid();

		printf("CHILD : my pid is %d\t my parent's pid is %d\t%s\n", pid, ppid, myTime(tmBuf) != NULL ? tmBuf : "time error");		
		processCommunicate(pid, ppid, tmBuf);	
		
		
		_exit(0);
	default:
		Children[nChild - 1] = p;
		return createChildProcesses(nChild - 1, tmBuf);
	}
	
}



void handlerUsr1(int NoSig)
{	
	puts("Signal received from parent");			
	nanosleep(100000000);//100ms
}

void handlerUsr2(int NoSig)
{
	puts("Signal received from child");
}


int main()
{
	int nChildren = 2;
		
	
	signal(SIGUSR1, handlerUsr1);
	signal(SIGUSR2, handlerUsr2);
	
	char *tmBuf = (char*)calloc(TIME_MAX, 1);
	if (!tmBuf)
	{
		perror("error m1 : Cannot allocate mem");
		return -1;
	}
	
	int res = createChildProcesses(nChildren, tmBuf);
	if (res)
	{
		perror("error m2 : Creating children failed");
		return -2;
	}
	

	
	printf("PARENT : my pid is %d\t my parent's pid is %d\t%s\n",getpid(), getppid(), myTime(tmBuf) != NULL ? tmBuf : "time error");
	
	
	
		
	putchar('\n');			
	for (int i = 0; i < nChildren; ++i)
		printf("%d ", Children[i]);
	putchar('\n');			


	int pid;
	int ws;	
	int res_kill;

	
	for (int i = 0; i < 2; ++i)
	{
		res_kill = kill(Children[0], SIGUSR1);		
		if (res_kill == -1)
		{}
		else
		{
			printf("#%d parent(%d) send to children\t%s\n", ++NoMessage, pid, myTime(tmBuf) != NULL ? tmBuf : "time error");
		}
	}




	for (int i = 0; i < 2; ++i)
	{
		pid = wait(&ws);
		printf("main wait:: pid = %d ws = %d", pid, ws);
//		if (pid > 0) 
	//	{
		//	if (WIFEXITED(ws))
			//	if (!WEXITSTATUS(ws))
				//	printf("%d terminated successfully\n", pid);				
		//}
		
	}	
	
	

	return 0;
}
