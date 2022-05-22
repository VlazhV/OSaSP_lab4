#include <signal.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#include "../lab5/mytime.c"
int tablenum;
int numMessage = 0, x = 0, y = 0;
pid_t pidnext;
char *tmbuf;
typedef void (*sighandler_t)(int);

void proc2_su1(int signum)
{
	
	myTime(tmbuf);
	printf("#%d %d  %d  get sigusr1 %s", tablenum, getpid(), getppid(), tmbuf != NULL ? tmbuf : "time error");
	x++;
}

void proc3_su1(int signum)
{
	myTime(tmbuf);
	printf("#%d %d  %d  get sigusr1 %s", tablenum, getpid(), getppid(), tmbuf != NULL ? tmbuf : "time error");
	x++;
	
	kill(pidnext, SIGUSR2);
}

void proc4_su2(int signum)
{
	myTime(tmbuf);
	printf("#%d %d  %d  get sigusr2 %s", tablenum, getpid(), getppid(), tmbuf != NULL ? tmbuf : "time error");
	y++;
	
	kill(0, SIGUSR1);
}


void proc56_su1(int signum)
{
	myTime(tmbuf);
	printf("#%d %d  %d  get sigusr1 %s", tablenum, getpid(), getppid(), tmbuf != NULL ? tmbuf : "time error");
	x++;
}

void proc7_su1(int signum)
{
	myTime(tmbuf);
	printf("#%d %d  %d  get sigusr1 %s", tablenum, getpid(), getppid(), tmbuf != NULL ? tmbuf : "time error");
	
	x++;
	kill(pidnext, SIGUSR1);
}

void proc8_su1(int signum)
{
	myTime(tmbuf);
	printf("#%d %d  %d  get sigusr1 %s", tablenum, getpid(), getppid(), tmbuf != NULL ? tmbuf : "time error");
	x++;
	
	kill(pidnext, SIGUSR2);
}

void proc1_su2(int signum)
{
	myTime(tmbuf);
	printf("#%d %d  %d  get sigusr1 %s", tablenum, getpid(), getppid(), tmbuf != NULL ? tmbuf : "time error");
	y++;
	
	kill(0, SIGUSR1);
}

void handle(int signum, sighandler_t handler, int sigign)
{
	struct sigaction sigact;
	sigemptyset(&sigact.sa_mask);
	sigact.sa_handler = handler;
	sigaction(signum, &sigact, NULL);
	
	sigact.sa_handler = SIG_IGN;
	sigaction(sigign, &sigact, NULL);
}


//void foo(int sig, int sig_ign, sighandler_t handler)
void foo()
{
	printf("#%d %d  %d\t  pgid=%d\n", tablenum, getpid(), getppid(), getpgid(0));
}

int main()
{
	tablenum = 0;
	foo();
	
	tmbuf = (char*)calloc(TIME_MAX, 1);
	if (tmbuf == NULL)
	{
		perror("error: cannot alloc mem");
		return -1;
	}
	
	
	pid_t pid1;
	pid_t pid4;

	pid_t p;
	p = fork();
	if (p == 0) //parent 0  --> parent 0 + child 1
	{//child 1
	
		pid1 = getpid();
		p = fork();  // child 1 --> child 1 + child 4
		if (p > 0)
		{//child 1
			pid4 = p;
			
			p = fork(); // child 1 --> child 1 + child 2
			if (p > 0)
			{
				
				p = fork(); // child 1 --> child 1 + child 3
				if (p > 0)
				{//child 1
					
					handle(SIGUSR2, proc1_su2, SIGUSR1);
					setpgid(0, 0);
					tablenum = 1;
					foo();
					sleep(1);
					
					
					kill(0, SIGUSR1);
					while(1);
					wait(NULL);wait(NULL);wait(NULL);
					exit(0);
				}//--child 1
				else if (p == 0)
				{//child 3
					handle(SIGUSR1, proc3_su1, SIGUSR2);
					
					pidnext = pid4;
					setpgid(0, getppid());
					tablenum = 3;
					foo();
					
					while(1);
					exit(0);
					
				}//--child 3
				
			}
			else if (p == 0)
			{//child 2
				
				
				p = fork(); //child 2 --> child 2 + child 5
				if ( p > 0)
				{
					
					p = fork(); //child 2 -- > child 2 + child 6
					if (p > 0)
					{//child 2
						handle(SIGUSR1, proc2_su1, SIGUSR2);
						setpgid(0, getppid());
						tablenum = 2;
						foo();
					
						while(1);
						wait(NULL);wait(NULL);
						exit(0);
					}//--child 2
					else if (p == 0)
					{//child 6
						
						p = fork(); //child 6 --> child 6 + child 7
						if (p == 0)
						{//child 7
							
							p = fork(); //child 7 --> child 7 + child 8
							if (p > 0)
							{//child 7
								setpgid(0, 0);
								tablenum = 7;
								foo();
								while(1);
								wait(NULL);
								exit(0);
							}//--child 7
							else if (p == 0)
							{//child 8
								setpgid(0, getppid());
								tablenum = 8;
								pidnext = pid1;
								foo();
								while(1);
								exit(0);
							}//--child 8
							
							
						}//--child 7
						else if (p > 0)
						{//child 6
							
							tablenum = 6;
							foo();
							while(1);
							wait(NULL);
							exit(0);
						}//--child 6
						
						
					}//--child 6
					
					
				}
				else if (p == 0)
				{//child 5
					handle(SIGUSR1, proc56_su1, SIGUSR2);
					tablenum = 5;
					foo();
					
					while(1);
					exit(0);
				}//--child 5
				
			}//--child 2
			
		}//--child 1
		else if (p == 0)
		{//child 4
			handle(SIGUSR2, proc4_su2, SIGUSR1);
			tablenum = 4;
			foo();
			
			while(1);
			exit(0);
		}//--child 4
				
	}//--child 1
	else if (p == 0)
	{//parent 0
		sleep(10);
//		wait(NULL);wait(NULL);wait(NULL);wait(NULL);wait(NULL);wait(NULL);wait(NULL);wait(NULL);wait(NULL);
//		wait(NULL);wait(NULL);wait(NULL);wait(NULL);wait(NULL);wait(NULL);wait(NULL);wait(NULL);wait(NULL);
//		wait(NULL);wait(NULL);wait(NULL);wait(NULL);wait(NULL);wait(NULL);wait(NULL);wait(NULL);wait(NULL);
//		wait(NULL);wait(NULL);wait(NULL);wait(NULL);wait(NULL);wait(NULL);wait(NULL);wait(NULL);wait(NULL);
		while(1);
		return 0;
	
	}//--parent 0
}