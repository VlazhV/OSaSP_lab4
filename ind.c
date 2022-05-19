//TODO maybe tmBuf errors
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include "../lab5/mytime.c"

#define MAX_SIGNALS 101
#define PROC 		9

int tableNum;
char *tmBuf;
int numMessage = 0;
pid_t root_pid;

typedef void (*sighandler_t)(int);


int ini_routine(int sig, int sig_ign, sighandler_t handler)
{
	struct sigaction sigact;

	sigact.sa_handler = handler;
	sigact.sa_flags = 0;			
	sigemptyset(&sigact.sa_mask);
	sigaddset(&sigact.sa_mask, sig_ign);
	sigaction (sig, &sigact, NULL);
	
				
	printf("proc #%d, pid = %d, ppid = %d\n\n", tableNum, getpid(), getppid());
}


//void handler_to_connect_		

void handler_proc1_su2 (int signum)
{
	myTime(tmBuf);
	printf("#%d   %d  %d  get SIGUSR2\t%s\n", tableNum, getpid(), getppid(), tmBuf != NULL ? tmBuf : "time error");	
	++numMessage;
	
	if (numMessage <= MAX_SIGNALS)
	{
	//	kill(0, SIGUSR1);
		myTime(tmBuf);
		printf("#%d   %d  %d  send SIGUSR1\t%s\n", tableNum, getpid(), getppid(), tmBuf != NULL ? tmBuf : "time error");	
	}
	else
	{
		//kill SIGTERM
		myTime(tmBuf);
		printf("#%d   %d  %d  send SIGTERM\t%s\n", tableNum, getpid(), getppid(), tmBuf != NULL ? tmBuf : "time error");	
	}
}



void handler_proc2_3_su1 (int signum)
{
	myTime(tmBuf);
	printf("#%d   %d  %d  get SIGUSR2\t%s\n", tableNum, getpid(), getppid(), tmBuf != NULL ? tmBuf : "time error");	
	
//	kill(0, 
	myTime(tmBuf);
	printf("#%d   %d  %d  send SIGUSR1\t%s\n", tableNum, getpid(), getppid(), tmBuf != NULL ? tmBuf : "time error");	
}



void handler_proc4_su2 (int signum)
{
	myTime(tmBuf);
	printf("#%d   %d  %d  get SIGUSR2\t%s\n", tableNum, getpid(), getppid(), tmBuf != NULL ? tmBuf : "time error");	
	
//	kill(0, 
	myTime(tmBuf);
	printf("#%d   %d  %d  send SIGUSR1\t%s\n", tableNum, getpid(), getppid(), tmBuf != NULL ? tmBuf : "time error");	
}



void handler_proc5_6_7_su1 (int signum)
{
	myTime(tmBuf);
	printf("#%d   %d  %d  get SIGUSR2\t%s\n", tableNum, getpid(), getppid(), tmBuf != NULL ? tmBuf : "time error");	
	
//	kill(0, 
	myTime(tmBuf);
	printf("#%d   %d  %d  send SIGUSR1\t%s\n", tableNum, getpid(), getppid(), tmBuf != NULL ? tmBuf : "time error");	
}



void handler_proc8_su1(int signum)
{
	myTime(tmBuf);
	printf("#%d   %d  %d  get SIGUSR2\t%s\n", tableNum, getpid(), getppid(), tmBuf != NULL ? tmBuf : "time error");	
	
//	kill(0, 
	myTime(tmBuf);
	printf("#%d   %d  %d  send SIGUSR1\t%s\n", tableNum, getpid(), getppid(), tmBuf != NULL ? tmBuf : "time error");	
}


int main()
{

	tableNum = 0;
	
	tmBuf = (char*)calloc(TIME_MAX, 1);
	pid_t p = fork();
	struct sigaction sigact;
	
	switch (p)
	{
		case -1: return -1;
		case 0:
			//proc 1;			
			tableNum = 1;
			
			
			ini_routine(SIGUSR2, SIGUSR1, handler_proc1_su2);
								
			p = fork();
			switch (p)
			{
				case -1: return -1;
				case 0: 
					//proc 2:					
					tableNum = 2;
													
					ini_routine(SIGUSR1, SIGUSR2, handler_proc2_3_su1);

					
					p = fork();
					switch (p)
					{
						case -1: return -1;
						case 0:
							//proc 5
							tableNum = 5;
							
							//SET P GID (4)

			
							ini_routine(SIGUSR1, SIGUSR2, handler_proc5_6_7_su1);
							
							
							free(tmBuf);							
							exit(0);
						default:
							//proc 2
							p = fork();
							switch (p)
							{
								case -1: return -1;
								case 0: 
									//proc 6
									tableNum = 6;
									
																										
									ini_routine(SIGUSR1, SIGUSR2, handler_proc5_6_7_su1);
									
									p = fork();
									switch(p)
									{
										case -1: return -1;
										case 0:
											//proc7
											tableNum = 7;
				
											
											ini_routine(SIGUSR1, SIGUSR2, handler_proc5_6_7_su1);
											
																						
											p = fork();
											switch(p)
											{
												case -1: return -1;
												case 0:
													//proc 8
													tableNum = 8;
											

													ini_routine (SIGUSR1, SIGUSR2, handler_proc8_su1);
													
													free(tmBuf);			
													exit(0);
													
												default:
													//proc 7
													wait(NULL);
												
											}
											
											free(tmBuf);	
											exit(0);
										default:
											//proc6
											wait(NULL);
									}
									
									free(tmBuf);
									exit(0);
								default:
									//proc 2
									wait(NULL);
									wait(NULL);
							}
					}
					
					free(tmBuf);
					exit(0);
				default:
				//proc 1;
					p = fork();
					switch (p)
					{
						case -1: return -1;
						case 0:
							//proc 3
							tableNum = 3;							
							
							//SET P GID (1)  ----> SET P GID (4)	
										
							ini_routine(SIGUSR1, SIGUSR2, handler_proc2_3_su1);

							
							free(tmBuf);
							exit(0);
						default:
						//proc 1
						
							p = fork();
							switch(p)
							{
								case -1: return -1;
								case 0:
									//proc 4
									tableNum = 4;
									
									ini_routine(SIGUSR2, SIGUSR1, handler_proc4_su2);
																		
									free(tmBuf);
									exit(0);
								default:
									//proc 1
									wait(NULL);
									wait(NULL);
									wait(NULL);
							}
					}
			}
			
			free(tmBuf);
			exit(0);
		default:
			//proc 0
			printf("proc #%d, pid = %d, ppid = %d\n\n", tableNum, getpid(), getppid());
			wait(NULL);
	}


	return 0;
}












