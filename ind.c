#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include "../lab5/mytime.c"



pid_t curr_pid, curr_ppid;
int tableNum;
char *tmBuf;
int numMessage;


void handler_proc1_su2 (int signum)
{
	myTime(tmBuf);
	printf("#%d   %d  %d  get SIGUSR2\t%s\n", tableNum, curr_pid, curr_ppid, tmBuf != NULL ? tmBuf : "time error");	
	
//	kill(0, 
	myTime(tmBuf);
	printf("#%d   %d  %d  send SIGUSR1\t%s\n", tableNum, curr_pid, curr_ppid, tmBuf != NULL ? tmBuf : "time error");	
}



void handler_proc2_3_su1 (int signum)
{
	myTime(tmBuf);
	printf("#%d   %d  %d  get SIGUSR2\t%s\n", tableNum, curr_pid, curr_ppid, tmBuf != NULL ? tmBuf : "time error");	
	
//	kill(0, 
	myTime(tmBuf);
	printf("#%d   %d  %d  send SIGUSR1\t%s\n", tableNum, curr_pid, curr_ppid, tmBuf != NULL ? tmBuf : "time error");	
}



void handler_proc4_su2 (int signum)
{
	myTime(tmBuf);
	printf("#%d   %d  %d  get SIGUSR2\t%s\n", tableNum, curr_pid, curr_ppid, tmBuf != NULL ? tmBuf : "time error");	
	
//	kill(0, 
	myTime(tmBuf);
	printf("#%d   %d  %d  send SIGUSR1\t%s\n", tableNum, curr_pid, curr_ppid, tmBuf != NULL ? tmBuf : "time error");	
}



void handler_proc5_6_7_su1 (int signum)
{
	myTime(tmBuf);
	printf("#%d   %d  %d  get SIGUSR2\t%s\n", tableNum, curr_pid, curr_ppid, tmBuf != NULL ? tmBuf : "time error");	
	
//	kill(0, 
	myTime(tmBuf);
	printf("#%d   %d  %d  send SIGUSR1\t%s\n", tableNum, curr_pid, curr_ppid, tmBuf != NULL ? tmBuf : "time error");	
}



void handler_proc8_su1(int signum)
{
	myTime(tmBuf);
	printf("#%d   %d  %d  get SIGUSR2\t%s\n", tableNum, curr_pid, curr_ppid, tmBuf != NULL ? tmBuf : "time error");	
	
//	kill(0, 
	myTime(tmBuf);
	printf("#%d   %d  %d  send SIGUSR1\t%s\n", tableNum, curr_pid, curr_ppid, tmBuf != NULL ? tmBuf : "time error");	
}


int main()
{

	tableNum = 0;
	curr_pid = getpid();
	curr_ppid = getppid();
	
	pid_t p = fork();
	struct sigaction sigact;
	
	switch (p)
	{
		case -1: return -1;
		case 0:
			//proc 1;			
			tableNum = 1;
			curr_pid = getpid();
			curr_ppid = getppid();
			

			tmBuf = (char*)calloc(TIME_MAX, 1);
			if (tmBuf == NULL)
			{
				perror("error 1: can't alloc mem");
				exit (1);
			}
			
			sigact.sa_handler = handler_proc1_su2;
			sigact.sa_flags = 0;			
			sigemptyset(&sigact.sa_mask);
			sigaddset(&sigact.sa_mask, SIGUSR1);
			sigaction (SIGUSR2, &sigact, NULL);
			
			
			printf("proc #%d, pid = %d, ppid = %d\n\n", tableNum, curr_pid, curr_ppid);
			
			p = fork();
			switch (p)
			{
				case -1: return -1;
				case 0: 
					//proc 2:					
					tableNum = 2;
					curr_pid = getpid();
					curr_ppid = getppid();
					
					tmBuf = (char*)calloc(TIME_MAX, 1);
					if (tmBuf == NULL)
					{
						perror("error 1: can't alloc mem");
						exit (1);
					}
					
					sigact.sa_handler = handler_proc2_3_su1;
					sigact.sa_flags = 0;
					sigemptyset(&sigact.sa_mask);
					sigaddset(&sigact.sa_mask, SIGUSR2);
					sigaction (SIGUSR1, &sigact, NULL);
					
					
					printf("proc #%d, pid = %d, ppid = %d\n\n", tableNum, curr_pid, curr_ppid);
					
					p = fork();
					switch (p)
					{
						case -1: return -1;
						case 0:
							//proc 5
							tableNum = 5;
							curr_pid = getpid();
							curr_ppid = getppid();
			
							
							tmBuf = (char*)calloc(TIME_MAX, 1);
							if (tmBuf == NULL)
							{
								perror("error 1: can't alloc mem");
								exit (1);
							}
							
							sigact.sa_handler = handler_proc5_6_7_su1;
							sigact.sa_flags = 0;
							sigemptyset(&sigact.sa_mask);
							sigaddset(&sigact.sa_mask, SIGUSR2);
							sigaction (SIGUSR1, &sigact, NULL);
							
							
							printf("proc #%d, pid = %d, ppid = %d\n\n", tableNum, curr_pid, curr_ppid);
							
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
									curr_pid = getpid();
									curr_ppid = getppid();
									
									tmBuf = (char*)calloc(TIME_MAX, 1);
									if (tmBuf == NULL)
									{
										perror("error 1: can't alloc mem");
										exit (1);
									}
									
									sigact.sa_handler = handler_proc5_6_7_su1;
									sigact.sa_flags = 0;
									sigemptyset(&sigact.sa_mask);
									sigaddset(&sigact.sa_mask, SIGUSR2);
									sigaction (SIGUSR1, &sigact, NULL);
									
									
									printf("proc #%d, pid = %d, ppid = %d\n\n", tableNum, curr_pid, curr_ppid);
									
									p = fork();
									switch(p)
									{
										case -1: return -1;
										case 0:
											//proc7
											tableNum = 7;
											curr_pid = getpid();
											curr_ppid = getppid();
			
											tmBuf = (char*)calloc(TIME_MAX, 1);
											if (tmBuf == NULL)
											{
												perror("error 1: can't alloc mem");
												exit (1);
											}
											
											sigact.sa_handler = handler_proc5_6_7_su1;
											sigact.sa_flags = 0;
											sigemptyset(&sigact.sa_mask);
											sigaddset(&sigact.sa_mask, SIGUSR2);
											sigaction (SIGUSR1, &sigact, NULL);
											
											
											printf("proc #%d, pid = %d, ppid = %d\n\n", tableNum, curr_pid, curr_ppid);
											
											p = fork();
											switch(p)
											{
												case -1: return -1;
												case 0:
													//proc 8
													tableNum = 8;
													curr_pid = getpid();
													curr_ppid = getppid();
													
											
													tmBuf = (char*)calloc(TIME_MAX, 1);
													if (tmBuf == NULL)
													{
														perror("error 1: can't alloc mem");
														exit (1);
													}
													
													sigact.sa_handler = handler_proc8_su1;
													sigact.sa_flags = 0;
													sigemptyset(&sigact.sa_mask);
													sigaddset(&sigact.sa_mask, SIGUSR2);
													sigaction (SIGUSR1, &sigact, NULL);
													
													
													printf("proc #%d, pid = %d, ppid = %d\n\n", tableNum, curr_pid, curr_ppid);
													
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
							curr_pid = getpid();
							curr_ppid = getppid();
			
							tmBuf = (char*)calloc(TIME_MAX, 1);
							if (tmBuf == NULL)
							{
								perror("error 1: can't alloc mem");
								exit (1);
							}
							
							sigact.sa_handler = handler_proc2_3_su1;
							sigact.sa_flags = 0;
							sigemptyset(&sigact.sa_mask);
							sigaddset(&sigact.sa_mask, SIGUSR2);
							sigaction (SIGUSR1, &sigact, NULL);
							
							
							printf("proc #%d, pid = %d, ppid = %d\n\n", tableNum, curr_pid, curr_ppid);
							
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
									curr_pid = getpid();
									curr_ppid = getppid();
									
									tmBuf = (char*)calloc(TIME_MAX, 1);
									if (tmBuf == NULL)
									{
										perror("error 1: can't alloc mem");
										exit (1);
									}
									
									sigact.sa_handler = handler_proc4_su2;
									sigact.sa_flags = 0;
									sigemptyset(&sigact.sa_mask);
									sigaddset(&sigact.sa_mask, SIGUSR1);
									sigaction (SIGUSR2, &sigact, NULL);
									
									
									printf("proc #%d, pid = %d, ppid = %d\n\n", tableNum, curr_pid, curr_ppid);
									
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
			printf("proc #%d, pid = %d, ppid = %d\n\n", tableNum, curr_pid, curr_ppid);
			wait(NULL);
	}
	
	return 0;
}
