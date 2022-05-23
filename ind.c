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

int res;

typedef void (*sighandler_t)(int);

int handle(int signum, sighandler_t handler, int sigign, sighandler_t handler_term)
{
	struct sigaction sigact;
	sigact.sa_flags = 0;
	int res = sigemptyset(&sigact.sa_mask);
	if (res == -1 )
	{
		fprintf(stderr, "#%d %d : sigemptyset() failed\n", tablenum, getpid());
		perror(" ");
		return -1;
	}
	
	sigact.sa_handler = handler;
	res = sigaction(signum, &sigact, NULL);
	if (res == -1)
	{
		fprintf(stderr, "#%d %d : sigaction() failed\n", tablenum, getpid());
		perror(" ");
		return -1;
	}
	
	if (sigign != -1 )
	{
		sigact.sa_handler = SIG_IGN;
		res = sigaction(sigign, &sigact, NULL);
		if (res == -1)
		{
			fprintf(stderr, "#%d %d : sigaction() failed\n", tablenum, getpid());
			perror(" ");
			return -1;
		}
	}

	if (handler_term != NULL)
	{
		sigact.sa_handler = handler_term;
		sigaddset(&sigact.sa_mask, SIGUSR1);
		sigaddset(&sigact.sa_mask, SIGUSR2);
		res = sigaction(SIGTERM, &sigact, NULL);
		if (res == -1)
		{
			fprintf(stderr, "#%d %d : sigaction() failed\n", tablenum, getpid());
			perror(" ");
			return -1;
		}
	}
	return 0;
}



int kill_fun(pid_t pid_to, int sig)
{	
	int res = kill( pid_to, sig);
	if (res == 0)
		printf("#%d %d %d  send %s %s\n", tablenum, getpid(), getppid(), sig == SIGUSR1 ? "sigusr1" : sig == SIGUSR2 ? "sigusr2" : "sigterm" ,tmbuf != NULL ? tmbuf : "time error");
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
			if (WIFSIGNALED(ws))
				printf("signaled");
			if (WIFEXITED(ws))
			{
				if (WEXITSTATUS(ws) != 0)
					fprintf(stderr, "%d terminated unsuccessfully\n", pid);
//				else
	//				printf("%d terminated successfully\n", pid);
			}
			else
			{
				fprintf(stderr, "%d did not terminate\n", pid);
				--i;
			}
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
	
	kill_fun(pidnext, SIGUSR1);
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
	
	if (y < SIGS)
		kill_fun(pidnext, SIGUSR1);
	else
	{
		kill_fun(pidnext, SIGTERM);
		handle(SIGUSR2, SIG_IGN, SIGUSR1, SIG_IGN);
	}
}




void proc34_term(int sig)
{
	handle(SIGUSR1, SIG_IGN, SIGUSR2, SIG_IGN);
	myTime(tmbuf);
	kill_fun(pidnext, sig);
	printf("#%d %d %d TERMINATED after %d %s %s\n", tablenum, getpid(), getppid(), x > 0 ? x : y, x > 0 ? "sigusr1" : "sigusr2", tmbuf != NULL ? tmbuf : "time error");
	to_exit = 1;
}

void proc58_term(int sig)
{
	handle(SIGUSR1, SIG_IGN, SIGUSR2, SIG_IGN);
	myTime(tmbuf);
	printf("#%d %d %d TERMINATED after %d %s %s\n", tablenum, getpid(), getppid(), x > 0 ? x : y, x > 0 ? "sigusr1" : "sigusr2", tmbuf != NULL ? tmbuf : "time error");
	to_exit = 1;
}

void proc2_term(int sig)
{
	handle(SIGUSR1, SIG_IGN, SIGUSR2, SIG_IGN);
	myTime(tmbuf);
	wait_fun(2);
	to_exit = 1;
	printf("#%d %d %d TERMINATED after %d %s %s\n", tablenum, getpid(), getppid(), x, "sigusr1", tmbuf != NULL ? tmbuf : "time error");
	
}

void proc6_term(int sig)
{
	handle(SIGUSR1, SIG_IGN, SIGUSR2, SIG_IGN);
	myTime(tmbuf);
	wait_fun(1);
	to_exit = 1;
	printf("#%d %d %d TERMINATED after %d %s %s\n", tablenum, getpid(), getppid(), x, "sigusr1", tmbuf != NULL ? tmbuf : "time error");
}

void proc7_term(int sig)
{
	handle(SIGUSR1, SIG_IGN, SIGUSR2, SIG_IGN);
	myTime(tmbuf);
	kill_fun(pidnext, SIGTERM);
	wait_fun(1);
	to_exit = 1;
	printf("#%d %d %d TERMINATED after %d %s %s\n", tablenum, getpid(), getppid(), x, "sigusr1", tmbuf != NULL ? tmbuf : "time error");
}


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
		perror("parent: cannot alloc mem");
		return -1;
	}
	
	
	pid_t pid1;
	pid_t pid4;
	
	res = handle(SIGUSR1, SIG_IGN, SIGUSR2, SIG_IGN);
	if (res == -1)
		return -1;

	pid_t p;
	p = fork();
	if (p == 0) //parent 0  --> parent 0 + child 1
	{//child 1
		pidnext = 0;
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
					
					handle(SIGUSR2, proc1_su2, SIGUSR1, SIG_IGN);
					setpgid(0, 0);
					tablenum = 1;
					foo();
					
					sleep(1);
					kill_fun(0, SIGUSR1);
					wait_fun(3);
					myTime(tmbuf);
					printf("#%d %d %d TERMINATED after %d sigusr2 %s\n", tablenum, getpid(), getppid(), y, tmbuf != NULL ? tmbuf : "time error");
					exit(0);
				}//--child 1
				else if (p == 0)
				{//child 3
					handle(SIGUSR1, proc3_su1, SIGUSR2, proc34_term);
					
					pidnext = pid4;
					setpgid(0, getppid());
					tablenum = 3;
					foo();
					
					while(!to_exit);
//					puts("3 : exit");
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
						handle(SIGUSR1, proc2_su1, SIGUSR2, proc2_term);
						setpgid(0, getppid());
						tablenum = 2;
						foo();
						
						sleep(3);
						while(!to_exit);
//						puts("2 : exit");
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
								handle(SIGUSR1, proc7_su1, SIGUSR2, proc7_term);
								setpgid(0, pid4);
								pidnext = p;
								tablenum = 7;
								foo();
								
								
								while(!to_exit);
//								puts("7 : exit");
								exit(0);
							}//--child 7
							else if (p == 0)
							{//child 8
								handle(SIGUSR1, proc8_su1, SIGUSR2, proc58_term);
								tablenum = 8;
								pidnext = pid1;
								foo();
								
								while(!to_exit);
//								puts("8 : exit");
								exit(0);
							}//--child 8
							
							
						}//--child 7
						else if (p > 0)
						{//child 6
							handle(SIGUSR1, proc56_su1, SIGUSR2, proc6_term);
							tablenum = 6;
							foo();
							
							while(!to_exit);
//							puts("6 : exit");
							exit(0);
						}//--child 6
						
						
					}//--child 6
					
					
				}
				else if (p == 0)
				{//child 5
					handle(SIGUSR1, proc56_su1, SIGUSR2, proc58_term);
					tablenum = 5;
					foo();
					
					while(!to_exit);
//					puts("5 : exit");
					exit(0);
				}//--child 5
				
			}//--child 2
			
		}//--child 1
		else if (p == 0)
		{//child 4
			handle(SIGUSR2, proc4_su2, SIGUSR1, proc34_term);
			pidnext = 0;
			tablenum = 4;
			foo();
			
			while(!to_exit);
//			puts("4 : exit");
			exit(0);
		}//--child 4
				
	}//--child 1
	else if (p > 0)
	{//parent
		wait_fun(1);
		return 0;
	}//--parent
	
}
