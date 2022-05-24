//TODO sem_unlink
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h> 
#include <time.h>
#include <signal.h>

#include <semaphore.h>
#include <fcntl.h>
#include <sys/stat.h>

#define SLEEP_TIME 100000
#define SIGNALS 10
#define TIME_MAX 12
#define SEM1 "/sem1"

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

typedef void (*sighandler_t)(int);


void emptyhandlerTstp(int NoSig)
{}

void handlerUsr1(int NoSig)
{
	myTime(tmBuf);	

	printf("#%d  child (%d, %d) RECEIVE signal SIGUSR1 from parent(%d)\t%s\n",
			++NoMessage, getpid(), getppid(), getppid(), tmBuf != NULL ? tmBuf : "time error");
		
	myTime(tmBuf);
	int res = kill(getppid(), SIGUSR2);
	if (res == -1)
	{
		fprintf(stderr, "error : child(%d, %d) COULDN'T SEND signal SIGUSR2 to parent(%d)\t%s\n", getpid(), getppid(), getppid(), tmBuf != NULL ? tmBuf : "time error");
		perror(" ");
	}
	else
		printf("#%d  child(%d, %d) SENT signal SIGUSR2 to parent(%d)\t%s\n",
				NoMessage, getpid(), getppid(), getppid(), tmBuf != NULL ? tmBuf : "time error");

}



int createChildProcesses(int nChild)
{
	if (nChild == 0)
		return 0;
	pid_t p = fork();
	pid_t ppid;
	pid_t pid;
	int res;
	int sig;
	sem_t *sem_id;
	
	struct sigaction sigact;
	sigset_t sigsetTstp;
	
	switch(p)
	{
	case -1:
		perror("fork() failed");
		return -1;
	case 0:
		
		sem_id = sem_open(SEM1, 0);
		printf("\n|--- CHILD [create]: my pid is %d\t my parent's pid is %d\t%s ---|\n\n", getpid(), getppid(), myTime(tmBuf) != NULL ? tmBuf : "time error");

		sigact.sa_handler = handlerUsr1;
		res = sigemptyset(&sigact.sa_mask);
		if (res == -1)
		{
			perror("error cch1 : sigemptyset() failed");
			sem_post(sem_id);
			sem_close(sem_id);
			_exit(-1);
		}
		
		res = sigaddset(&sigact.sa_mask, SIGTSTP);
		if (res == -1)
		{
			perror("error cch2 : sigaddset() failed");
			sem_post(sem_id);
			sem_close(sem_id);
			_exit(-2);
		}
		sigact.sa_flags = 0;
		
		res = sigaction(SIGUSR1, &sigact, NULL);
		if (res == -1)
		{
			perror("error cch3 : sigaction failed()");
			sem_post(sem_id);
			sem_close(sem_id);
			_exit(-3);
		}
		
		sigact.sa_handler = emptyhandlerTstp;
		res = sigemptyset(&sigact.sa_mask);
		if (res == -1)
		{
			perror("error cch1 : sigemptyset() failed");
			sem_post(sem_id);
			sem_close(sem_id);
			_exit(-1);
		}
		sigact.sa_flags = 0;		
		
		res = sigaction(SIGTSTP, &sigact, NULL);
		if (res == -1)
		{
			perror("error cch3 : sigaction failed()");
			sem_post(sem_id);
			sem_close(sem_id);
			_exit(-3);
		}
				
				
		
		res = sigemptyset(&sigsetTstp);
		if (res == -1)
		{
			perror("error cch1 : sigemptyset() failed");
			sem_post(sem_id);
			sem_close(sem_id);
			_exit(-1);
		}
		
		res = sigaddset(&sigsetTstp, SIGTSTP);
		if (res == -1)
		{
			perror("error cch2 : sigaddset() failed");
			sem_post(sem_id);
			sem_close(sem_id);
			_exit(-2);
		}
		
		res = sem_post(sem_id);
		if (res == -1)
		{
			perror("error cch3 : sem_post() failed");
			sem_close(sem_id);
			_exit(-3);
		}
		
		res = sigwait(&sigsetTstp, &sig);
		myTime(tmBuf);
		if (res > 0)
		{
			perror("error cpp5: sigwait() failed");
			sem_close(sem_id);
			_exit(-5);
		}
		
		printf("\n|--- CHILD [terminate]: my pid is %d\t my parent's pid is %d\t%s ---|\n\n", getpid(), getppid(), tmBuf != NULL ? tmBuf : "time error");
		
		res = sem_close(sem_id);
		if (res == -1)
		{
			perror("error cch4: sem_close() failed");
			_exit(-4);
		}
		_exit(0);
	default:

		return createChildProcesses(nChild-1);
	}
	
}





void handlerUsr2(int NoSig, siginfo_t *info, void *context)
{
	myTime(tmBuf);	
	pid_t sender_pid = info->si_pid;
	
	printf("\t\t#%d parent(%d) RECEIVE signal SIGUSR2 from child(%d)\t%s\n",
			NoMessage, getpid(), sender_pid, tmBuf != NULL ? tmBuf : "time error");
	
	int res;

	usleep(SLEEP_TIME);
	
	if (NoMessage <= SIGNALS)		
	{
		myTime(tmBuf);
		res = kill(0, SIGUSR1);
		if (res == -1)
			fprintf(stderr, "\t\terror : #%d parent(%d) COULDN'T SEND signal SIGUSR2 to children\t%s\n",
										NoMessage, getpid(), tmBuf != NULL ? tmBuf : "time error");
		else
			printf("\t\t#%d parent(%d) SENT signal SIGUSR2 to children\t%s\n",
										++NoMessage, getpid(), tmBuf != NULL ? tmBuf : "time error");
	}
	else
	{
		myTime(tmBuf);
		res = kill(0, SIGTSTP);
		if (res == -1)
			perror("error m8: kill() falied : couldn't send SIGTSTP");
		else
			printf("\t\t#%d parent(%d) SENT signal SIGTSTP to children\t%s\n",
								++NoMessage, getpid(), tmBuf != NULL ? tmBuf : "time error");
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
	
	sem_t *sem_id = sem_open(SEM1, O_CREAT, 777, 0);
	
	int res = createChildProcesses(nChildren);
	if (res)
	{
		perror("error m2 : Creating children failed");
		sem_close(sem_id);
		sem_unlink(SEM1);
		return -2;
	}
	

	
	printf("\n|--- PARENT : my pid is %d\t my parent's pid is %d\t%s ---|\n\n", getpid(), getppid(), myTime(tmBuf) != NULL ? tmBuf : "time error");

	//sleep(1);	
	for (int i = 0; i < nChildren; ++i)
		sem_wait(sem_id);

//SIG_HANDLE
	struct sigaction sigact;
	sigact.sa_sigaction = handlerUsr2;
	sigact.sa_flags = SA_SIGINFO;

	res = sigemptyset(&sigact.sa_mask);
	if (res == -1)
	{
		perror("error : sigemptyset() failed");
		sem_close(sem_id);
		sem_unlink(SEM1);
		return -1;
	}
	
	res = sigaction(SIGUSR2, &sigact, NULL);
	if (res == -1)
	{
		perror("error : sigaction() failed");
		sem_close(sem_id);
		sem_unlink(SEM1);
		return -1;
	}
	
	sighandler_t resh = signal(SIGUSR1, SIG_IGN);
	if (resh == SIG_ERR)
	{
		perror("error: signal() failed");
		sem_close(sem_id);
		sem_unlink(SEM1);
		return -1;
	}
	
	resh = signal(SIGTSTP, SIG_IGN);
	if (resh == SIG_ERR)
	{
		perror("error: signal() failed");
		sem_close(sem_id);
		sem_unlink(SEM1);
		return -1;
	}
	
	
	

//SET SIGSET FOR SIGWAIT()
	
	
			
		
//SIGNAL COMMUNICATE START
	myTime(tmBuf);
	res = kill(0, SIGUSR1);
	if (res == -1)
		fprintf(stderr, "\t\terror : #%d parent(%d) COULDN'T SEND signal SIGUSR2 to children\t%s\n",
									NoMessage, getpid(), tmBuf != NULL ? tmBuf : "time error");
	else
		printf("\t\t#%d parent(%d) SENT signal SIGUSR2 to children\t%s\n",
									++NoMessage, getpid(), tmBuf != NULL ? tmBuf : "time error");
				
//SEND STOP SIGNAL


	
	//sleep(2);

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
	res = sem_close(sem_id);
	if (res == -1)
		perror ("error m10 : sem_close() failed");
		
	
	res = sem_unlink(SEM1);
	if (res == -1)
		perror("error m11 : sem_unlink() failed");
	return 0;
}
