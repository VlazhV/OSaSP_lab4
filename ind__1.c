#include <signal.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>

#include "../lab5/mytime.c"

#define SIGS 101


int tablenum;
int x = 0, y = 0;
pid_t pidnext;
char *tmbuf;
int to_exit = 0;

typedef void (*sighandler_t)(int);

int kill_fun(pid_t pid_to, int sig)
{
	int res = kill( pid_to, sig);
	if (res == 0)
		printf("#%d %d  %d  send %s %s\n", tablenum, getpid(), getppid(), sig == SIGUSR1 ? "sigusr1" : sig == SIGUSR2 ? "sigusr2" : "sigterm" ,tmbuf != NULL ? tmbuf : "time error");
	else
	{
		fprintf(stderr, "#%d %d : kill() failed", tablenum, getpid());
		perror(" ");
	}
		
	return res;
	
}

void wait_fun(int n_children)
{
	int ws;
	int pid;
	
	for (int i = 0; i < n_children; ++i)
	{
		pid = wait(&ws);
		if (pid != -1)
		{
			if (WIFEXITED(ws))
			{
				if (WEXITSTATUS(ws) != 0)
					fprintf(stderr, "%d terminated unsuccessfully\n", pid);
				else
					printf("%d terminated successfully\n", pid);
			}
			else
				fprintf(stderr, "%d did not terminate\n", pid);
		}
		else
			--i;
		
	}
}


void proc2_su1(int signum)
{
	myTime(tmbuf);
	printf("#%d %d  %d  get sigusr1 %s\n", tablenum, getpid(), getppid(), tmbuf != NULL ? tmbuf : "time error");
	x++;
}

void proc3_su1(int signum)
{
	myTime(tmbuf);
	printf("#%d %d  %d  get sigusr1 %s\n", tablenum, getpid(), getppid(), tmbuf != NULL ? tmbuf : "time error");
	x++;
	
	kill_fun(pidnext, SIGUSR2);
}

void proc4_su2(int signum)
{
	myTime(tmbuf);
	printf("#%d %d  %d  get sigusr1 %s\n", tablenum, getpid(), getppid(), tmbuf != NULL ? tmbuf : "time error");
	y++;
	
	kill_fun(0, SIGUSR1);
}


void proc56_su1(int signum)
{
	myTime(tmbuf);
	printf("#%d %d  %d  get sigusr1 %s\n", tablenum, getpid(), getppid(), tmbuf != NULL ? tmbuf : "time error");
	x++;
}

void proc7_su1(int signum)
{
	myTime(tmbuf);
	printf("#%d %d  %d  get sigusr1 %s\n", tablenum, getpid(), getppid(), tmbuf != NULL ? tmbuf : "time error");
	
	x++;
	kill_fun(pidnext, SIGUSR1);
}

void proc8_su1(int signum)
{
	myTime(tmbuf);
	printf("#%d %d  %d  get sigusr1 %s\n", tablenum, getpid(), getppid(), tmbuf != NULL ? tmbuf : "time error");
	x++;
	
	kill_fun(pidnext, SIGUSR2);
}

void proc1_su2(int signum)
{
	myTime(tmbuf);
	printf("#%d %d  %d  get sigusr2 %s\n", tablenum, getpid(), getppid(), tmbuf != NULL ? tmbuf : "time error");
	y++;
	
	if (y <= SIGS)
		kill_fun(0, SIGUSR1);
	else
		kill_fun(0, SIGTERM);
}

void handle(int signum, sighandler_t handler, int sigign)
{
	struct sigaction sigact;
	sigact.sa_flags = 0;
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
		perror("error parent: cannot alloc mem");
		return -1;
	}
	
	
	pid_t pid1;
	pid_t pid4;
	
	struct sigaction sigact;
	sigact.sa_flags = 0;
	sigemptyset(&sigact.sa_mask);
	sigact.sa_handler = SIG_IGN;
	sigaction(SIGUSR1, &sigact, NULL);
	sigaction(SIGUSR2, &sigact, NULL);

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
			{//child 1
				
				p = fork(); // child 1 --> child 1 + child 3
				if (p > 0)
				{//child 1
					
					handle(SIGUSR2, proc1_su2, SIGUSR1);
					setpgid(0, 0);
					tablenum = 1;
					foo();
					
					
					sleep(1);
					kill_fun(0, SIGUSR1);
					wait_fun(3);
					//while(1);
					exit(0);
				}//--child 1
				else if (p == 0)
				{//child 3
					handle(SIGUSR1, proc3_su1, SIGUSR2);
					
					pidnext = pid4;
					setpgid(0, getppid());
					tablenum = 3;
					foo();
					
					
																							//					sleep(2);
																							//					sigset_t ss;
																							//					sigemptyset(&ss);
																							//					sigaddset(&ss, SIGUSR2);
																							//					
																							//					int pidd;
																							//					sigwait(&ss, &pidd);
					while(!to_exit);
					sleep(3);
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
						
						sleep(3);
						wait_fun(2);
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
								handle(SIGUSR1, proc7_su1, SIGUSR2);
								setpgid(0, pid4);
								pidnext = p;
								tablenum = 7;
								foo();
								
								
								wait_fun(1);
								exit(0);
							}//--child 7
							else if (p == 0)
							{//child 8
								handle(SIGUSR1, proc8_su1, SIGUSR2);
								tablenum = 8;
								pidnext = pid1;
								foo();
								
//								kill(pid1, SIGUSR2);
								while(!to_exit);
								exit(0);
							}//--child 8
							
							
						}//--child 7
						else if (p > 0)
						{//child 6
							handle(SIGUSR1, proc56_su1, SIGUSR2);
							tablenum = 6;
							foo();
							wait_fun(1);
							exit(0);
						}//--child 6
						
						
					}//--child 6
					
					
				}
				else if (p == 0)
				{//child 5
					handle(SIGUSR1, proc56_su1, SIGUSR2);
					tablenum = 5;
					foo();
					
					while(!to_exit);
					exit(0);
				}//--child 5
				
			}//--child 2
			
		}//--child 1
		else if (p == 0)
		{//child 4
			handle(SIGUSR2, proc4_su2, SIGUSR1);
			tablenum = 4;
			foo();
			
			while(!to_exit);
			exit(0);
		}//--child 4
				
	}//--child 1
	else if (p > 0)
	{//parent
		wait_fun(1);
		return 0;
	}//--parent
	
}
