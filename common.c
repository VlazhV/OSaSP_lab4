//TODO check all kill sigawait signal

#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h> 
#include <time.h>
#include <signal.h>


#define SIGNALS 19
#define TIME_MAX 12

int sleep100ms()
{
	struct timespec ts;
	ts.tv_sec = 0;
	ts.tv_nsec = 100 * 1000 * 1000;
	return nanosleep(&ts, NULL);
}

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
		
	if (sprintf(tmBuf, "%d:%d:%d:%d", hms->tm_hour, hms->tm_min, hms->tm_sec, (int)mtime.tv_nsec/1000) < 0)
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
	myTime(tmBuf);	

	printf("#%d  child (%d, %d) RECEIVE signal SIGUSR1 from parent(%d)\t%s\n",
			++NoMessage, curr_pid, curr_parent_pid, curr_parent_pid, tmBuf != NULL ? tmBuf : "time error");
		
	myTime(tmBuf);
	int res = kill(curr_parent_pid, SIGUSR2);
	if (res == -1)
	{
		fprintf(stderr, "error : child(%d, %d) COULDN'T SEND signal SIGUSR2 to parent(%d)\t%s\n", curr_pid, curr_parent_pid, curr_parent_pid, tmBuf != NULL ? tmBuf : "time error");
		perror(" ");
	}
	else
		printf("#%d  child(%d, %d) SENT signal SIGUSR2 to parent(%d)\t%s\n",
				NoMessage, curr_pid, curr_parent_pid, curr_parent_pid, tmBuf != NULL ? tmBuf : "time error");

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
	
	struct sigaction sigact;
	sigset_t sigsetTstp;
	
	switch(p)
	{
	case -1:
		perror("fork() failed");
		return -1;
	case 0:		
	

		curr_pid = getpid();
		curr_parent_pid = getppid();
		
		printf("\n|--- CHILD [create]: my pid is %d\t my parent's pid is %d\t%s ---|\n\n", curr_pid, curr_parent_pid, myTime(tmBuf) != NULL ? tmBuf : "time error");		
		

		sigact.sa_handler = handlerUsr1;
		res = sigemptyset(&sigact.sa_mask);
		if (res == -1)
		{
			perror("error cch1 : sigemptyset() failed");
			_exit(-1);
		}
		
		res = sigaddset(&sigact.sa_mask, SIGTSTP);
		if (res == -1)
		{
			perror("error cch2 : sigaddset() failed");
			_exit(-2);
		}
		sigact.sa_flags = 0;
		
		res = sigaction(SIGUSR1, &sigact, NULL);
		if (res == -1)
		{
			perror("error cch3 : sigaction failed()");
			_exit(-3);
		}
		
		sigact.sa_handler = emptyhandlerTstp;
		res = sigemptyset(&sigact.sa_mask);
		if (res == -1)
		{
			perror("error cch1 : sigemptyset() failed");
			_exit(-1);
		}
		sigact.sa_flags = 0;		
		
		res = sigaction(SIGTSTP, &sigact, NULL);
		if (res == -1)
		{
			perror("error cch3 : sigaction failed()");
			_exit(-3);
		}
				
				
		
		res = sigemptyset(&sigsetTstp);
		if (res == -1)
		{
			perror("error cch1 : sigemptyset() failed");
			_exit(-1);
		}
		
		res = sigaddset(&sigsetTstp, SIGTSTP);
		if (res == -1)
		{
			perror("error cch2 : sigaddset() failed");
			_exit(-2);
		}
		
		res = sigwait(&sigsetTstp, &sig);
		myTime(tmBuf);
		if (res > 0)
		{
			perror("error cpp5: sigwait() failed");
			exit(-5);
		}
		
												
		printf("\n|--- CHILD [terminate]: my pid is %d\t my parent's pid is %d\t%s ---|\n\n", curr_pid, curr_parent_pid, tmBuf != NULL ? tmBuf : "time error");				
		_exit(0);
	default:
		curr_pid = getpid();
		curr_parent_pid = getppid();
		Children[nChild - 1] = p;
		return createChildProcesses(nChild-1);
	}
	
}





void handlerUsr2(int NoSig, siginfo_t *info, void *context)
{
	myTime(tmBuf);	
	pid_t sender_pid = info->si_pid;
	
	printf("\t\t#%d parent(%d) RECEIVE signal SIGUSR2 from child(%d)\t%s\n",
			NoMessage, curr_pid, sender_pid, tmBuf != NULL ? tmBuf : "time error");
	
	
	int res = sleep100ms();
	if (res == -1)	
		perror("error : sleep100ms() failed");
	
	if (NoMessage <= SIGNALS)		
	{
		myTime(tmBuf);
		res = kill(0, SIGUSR1);
		if (res == -1)
			fprintf(stderr, "\t\terror : #%d parent(%d) COULDN'T SEND signal SIGUSR2 to children\t%s\n",
										NoMessage, curr_pid, tmBuf != NULL ? tmBuf : "time error");
		else
			printf("\t\t#%d parent(%d) SENT signal SIGUSR2 to children\t%s\n",
										++NoMessage, curr_pid, tmBuf != NULL ? tmBuf : "time error");
	}
	else
	{
		myTime(tmBuf);
		res = kill(0, SIGTSTP);
		if (res == -1)
			perror("error m8: kill() falied : couldn't send SIGTSTP");
		else
			printf("\t\t#%d parent(%d) SENT signal SIGTSTP to children\t%s\n",
								++NoMessage, curr_pid, tmBuf != NULL ? tmBuf : "time error");
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
	
	
			
		
//SIGNAL COMMUNICATE START
	myTime(tmBuf);
	res = kill(0, SIGUSR1);
	if (res == -1)
		fprintf(stderr, "\t\terror : #%d parent(%d) COULDN'T SEND signal SIGUSR2 to children\t%s\n",
									NoMessage, curr_pid, tmBuf != NULL ? tmBuf : "time error");
	else
		printf("\t\t#%d parent(%d) SENT signal SIGUSR2 to children\t%s\n",
									++NoMessage, curr_pid, tmBuf != NULL ? tmBuf : "time error");
				
//SEND STOP SIGNAL


	
	sleep(2);


//WAIT CHILDREN ENDING
	int ws;
	int pid;
	for (int i = 0; i < nChildren; ++i)
	{
		pid = wait(&ws);
//		printf("pid = %d wie = %d wes = %d stat = %d\n", pid, WIFEXITED(ws), WEXITSTATUS(ws), ws);
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
				fprintf(stderr, "%d did not terminate\n", pid);
		}
		else
		{
			perror ("error m9: wait() failed");
			i--;
		}
	}	
	
	
	printf("\n\nPARENT EXIT\n\n");
	return 0;
}
