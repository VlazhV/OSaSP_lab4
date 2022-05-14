//TODO check all kill sigawait signal

#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h> 
#include <time.h>
#include <signal.h>


#define SIGNALS 2
#define TIME_MAX 12
char *tmBuf;
char *myTime(char *tmBuf)
{

	for (int i = 0; i < 10; ++i)
	if (tmBuf == NULL)
	{
		fprintf(stderr, "error mT1: myTime(null)");
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
char *log_filename;

typedef void (*sighandler_t)(int);


void emptyhandlerTstp(int NoSig)
{}

void handlerUsr1(int NoSig)
{
	

	printf("#%d  child (%d, %d) RECEIVE signal SIGUSR1 from parent(%d)\t%s\n",
			++NoMessage, curr_pid, curr_parent_pid, curr_parent_pid, myTime(tmBuf) != NULL ? tmBuf : "time error");
	
	
	int res = kill(curr_parent_pid, SIGUSR2);
	if (res == -1)
	{
		fprintf(stderr, "error : child(%d, %d) COULDN'T SEND signal SIGUSR2 to parent(%d)\n", curr_pid, curr_parent_pid, curr_parent_pid);
		perror(" ");
	}
	else
		printf("#%d  child(%d, %d) SEND signal SIGUSR2 to parent(%d)\t%s\n",
				NoMessage, curr_pid, curr_parent_pid, curr_parent_pid, myTime(tmBuf) != NULL ? tmBuf : "time error");

}



int createChildProcesses(int nChild)
{
	if (nChild == 0)
		return 0;
	pid_t p = fork();
	pid_t ppid;
	pid_t pid;
	int res;
	sighandler_t res_signal;
	int sig;
	struct sigaction sigact_usr1;
	sigset_t sigset;
	
	switch(p)
	{
	case -1:
		perror("fork() failed");
		return -1;
	case 0:		
	

		curr_pid = getpid();
		curr_parent_pid = getppid();
		
		printf("\n|--- CHILD [create]: my pid is %d\t my parent's pid is %d\t%s ---|\n\n", curr_pid, curr_parent_pid, myTime(tmBuf) != NULL ? tmBuf : "time error");		
		
		res = sigemptyset(&sigset);
		if (res == -1)
		{
			perror("error ccp1: sigemptyset() failed");			
			exit(-1);
		}		
		res = sigaddset(&sigset, SIGTSTP);
		if (res == -1)
		{
			perror("error ccp2: sigaddset() failed");
			exit(-2);
		}
		
		sigact_usr1.sa_handler = handlerUsr1;
		sigemptyset(&sigact_usr1.sa_mask);		
		sigact_usr1.sa_flags = 0;

		res = sigaction(SIGUSR1, &sigact_usr1, NULL);
		if (res == -1)
		{
			perror("error -2: sigaction failed");
			_exit(-2);
		}

//		res_signal = signal(SIGUSR1, handlerUsr1);
//		if (res_signal == SIG_ERR)
//		{
//			perror("error ccp3: signal(SIGUSR1, __ ) failed");
//			exit(-3);
//		}
		res_signal = signal(SIGTSTP, emptyhandlerTstp);
		if (res_signal == SIG_ERR)
		{
			perror("error ccp4: signal(SIGTSTP, __ ) failed");
			exit(-4);
		}
		

		res = sigwait(&sigset, &sig);
		if (res > 0)
		{
			perror("error cpp5: sigwait() failed");
			exit(-5);
		}
		
												
		printf("\n|--- CHILD [terminate]: my pid is %d\t my parent's pid is %d\t%s ---|\n\n", curr_pid, curr_parent_pid, myTime(tmBuf) != NULL ? tmBuf : "time error");		
		exit(0);
	default:
		curr_pid = getpid();
		curr_parent_pid = getppid();
		Children[nChild - 1] = p;
		return createChildProcesses(nChild-1);
	}
	
}





void handlerUsr2(int NoSig, siginfo_t *info, void *context)
{	
	pid_t sender_pid = info->si_pid;
	
	printf("\t\t#%d parent(%d) RECEIVE signal SIGUSR2 from child(%d)\t%s\n",
			++NoMessage, curr_pid, sender_pid, myTime(tmBuf) != NULL ? tmBuf : "time error");
	
	sleep(1);
	int res;
	if (NoMessage <= SIGNALS)		
	{
		res = kill(0, SIGUSR1);
		if (res == -1)
			fprintf(stderr, "\t\terror : #%d parent(%d) COULDN'T SEND signal SIGUSR2 to children\t%s\n",
										NoMessage, curr_pid, myTime(tmBuf) != NULL ? tmBuf : "time error");
		else
			printf("\t\t#%d parent(%d) SENT signal SIGUSR2 to children\t%s\n",
										NoMessage, curr_pid, myTime(tmBuf) != NULL ? tmBuf : "time error");
	}
	else
	{
		res = kill(0, SIGTSTP);
		if (res == -1)
			perror("error m8: kill() falied : couldn't send SIGTSTP");		
	}
	
	

}


int main()
{
	int nChildren = 2;
	
	
	tmBuf = (char*)calloc(TIME_MAX, 1);
	if (!tmBuf)
	{
		perror("error m1 : calloc() failed");
		return -1;
	}
	
	int res = createChildProcesses(nChildren);
	if (res)
	{
		perror("error m2 : Creating children failed");
		return -2;
	}
	

	
	printf("\n|--- PARENT : my pid is %d\t my parent's pid is %d\t%s ---|\n\n", curr_pid, curr_parent_pid, myTime(tmBuf) != NULL ? tmBuf : "time error");

//DEBUG
	sleep(1);	
	putchar('\n');
	for (int i = 0; i < nChildren; ++i)
		printf("%d ", Children[i]);
	putchar('\n');
	

//SIG_HANDLE		TODO nee checks
	struct sigaction sigact;
	sigact.sa_sigaction = handlerUsr2;
	sigact.sa_flags = SA_SIGINFO;
	sigemptyset(&sigact.sa_mask);
	
	sigaction(SIGUSR2, &sigact, NULL);
	
	signal(SIGUSR1, SIG_IGN);
	signal(SIGTSTP, SIG_IGN);
	


//SET SIGSET FOR SIGWAIT()
	
	
			
		
//SIGNALS SEND
	res = kill(0, SIGUSR1);
	if (res == -1)
		perror("error m8: kill() failed");
				
//SEND STOP SIGNAL


	
	sleep(2);


//WAIT CHILDREN ENDING
	int ws;
	int pid;
	for (int i = 0; i < nChildren; ++i)
	{
		pid = wait(&ws);
		if (pid > 0) 
		{

			if (WIFEXITED(ws))	
			{		
				if (0 == WEXITSTATUS(ws))
					printf("%d terminated successfully\n", pid);
				else
				{
					fprintf(stderr, "%d terminated Unsuccessfully\n", pid);
					perror(" ");
				}
			}
			else
			{
				fprintf(stderr, "%d did not terminate)\n", pid);
				perror(" ");
			}				
		}
		else
			perror ("error m9: wait() failed");
	}	
	
	
	printf("\n\nPARENT EXIT\n\n");
	return 0;
}
