//TODO chech all kill sigawait signal



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
	for (int i = 0; i < 10; ++i)
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
		
	if (sprintf(tmBuf, "%d:%d:%d:%d", hms->tm_hour, hms->tm_min, hms->tm_sec, (int)mtime.tv_nsec) < 0)
	{
		perror("error mT2 : sprintf() failed");
		return NULL;
	}
	return tmBuf;
}


static int NoMessage = 0;
static pid_t curr_pid;
static pid_t curr_parent_pid;
static pid_t Children[2]; 


void handlerTstp(int NoSig)
{
	return;
}

void handlerUsr1(int NoSig)
{
//	struct timespec mtime;
//	mtime.tv_sec = 0;
//	mtime.tv_nsec = 100000000;
//	nanosleep(&mtime, NULL);//100ms
//	puts("Signal received from parent\n");
	printf("child (%d, %d) receive signal SIGUSR1 from parent(%d)\n", curr_pid, curr_parent_pid, curr_parent_pid);	
	sleep(1);
	kill(curr_parent_pid, SIGUSR2);
//	kill(curr_pid, SIGUSR2);


}



void processCommunicate(char *tmBuf)
{
	int res_kill;
	int ws;
	for (int i = 0; i < 1; ++i)
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
					
		res_kill = kill(curr_parent_pid, SIGUSR2);
		if (res_kill == -1)
		{}
		else
		{
			printf("#%d child(%d, %d) send to parent(%d)\t%s\n\n", ++NoMessage, curr_pid, curr_parent_pid, curr_parent_pid, myTime(tmBuf) != NULL ? tmBuf : "time error");
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
	int res_sigact;
	int sig;
	struct sigaction sigact_usr1;
	sigset_t sigset;
	int i;
	
	switch(p)
	{
	case -1:
		perror("fork() failed");
		return -1;
	case 0:		

		curr_pid = getpid();
		curr_parent_pid = getppid();
		
		printf("\n|---\tCHILD : my pid is %d\t my parent's pid is %d\t%s\t---|\n\n", curr_pid, curr_parent_pid, myTime(tmBuf) != NULL ? tmBuf : "time error");		
//		processCommunicate(tmBuf);	
		
		sigemptyset(&sigset);
		sigaddset(&sigset, SIGTSTP);
		
//		sigact_usr1.sa_handler = handlerUsr1;
//		sigemptyset(&sigact_usr1.sa_mask);
																							//		sigaddset(&sigact_usr1.sa_mask, SIGUSR1);
//		sigact_usr1.sa_flags = 0;

//		res_sigact = sigaction(SIGUSR1, &sigact_usr1, NULL);
//		if (res_sigact == -1)
//		{
//			perror("error -2: sigaction failed");
//			_exit(-2);
//		}
		signal(SIGUSR1, handlerUsr1);
		signal(SIGTSTP, SIG_IGN);


		sigwait(&sigset, &sig);

		
		_exit(0);
	default:
		curr_pid = getpid();
		curr_parent_pid = getppid();
		Children[nChild - 1] = p;
		return createChildProcesses(nChild-1, tmBuf);
	}
	
}





void handlerUsr2(int NoSig)
{	
	printf("\t\t\t\tparent(%d) receive signal SIGUSR2 from children\n", curr_pid);
}


int main()
{
	int nChildren = 2;
		
//	signal(SIGUSR1, handlerUsr1);
	signal(SIGUSR2, handlerUsr2);
	signal(SIGUSR1, SIG_IGN);
	signal(SIGTSTP, handlerTstp);
	
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
	

	
	printf("\n|---\tPARENT : my pid is %d\t my parent's pid is %d\t%s\t---|\n\n", curr_pid, curr_parent_pid, myTime(tmBuf) != NULL ? tmBuf : "time error");
	
	
	putchar('\n');
	for (int i = 0; i < nChildren; ++i)
		printf("%d ", Children[i]);
	putchar('\n');
	
/*
	
	for (int i = 0; i < 5; ++i)
	{
		res_kill = kill(Children[0], SIGUSR1);		
		if (res_kill == -1)
		{}
		else
		{
			printf("#%d parent(%d) send to children\t%s\n", ++NoMessage, curr_pid, myTime(tmBuf) != NULL ? tmBuf : "time error");
		}
	}

*/

	int pid;
	int ws;	
	int res_kill;
	int sig;
	sigset_t sigset;
	
	
	sigemptyset(&sigset);
	sigaddset(&sigset, SIGUSR2);
	
	for (int i = 0; i < 2; ++i)
	{
			sleep(2);
			printf("%d[poslan]\n", i);
			kill(0, SIGUSR1);
			sigwait(&sigset, &sig);
	}

	kill(0, SIGTSTP);
			

	for (int i = 0; i < 2; ++i)
	{
		pid = wait(&ws);
		if (pid > 0) 
		{
			if (WIFEXITED(ws))
				if (!WEXITSTATUS(ws))
					printf("%d terminated successfully\n", pid);				
		}
	}	
	
	
	printf("\n\nPARENT EXIT\n\n");
	return 0;
}
