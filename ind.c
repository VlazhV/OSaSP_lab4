#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>



pid_t curr_pid, curr_parent_pid;
int tableNum;


void handler_proc1_su2 (int signum)
{
	
}

void handler_proc2_3_su1 (int signum)
{}

void handler_proc4_su2 (int signum)
{}

void handler_proc5_6_7_su1 (int signum)
{}

void handler_proc8_su1(int signum)
{}


int main()
{
	
	pid_t p = fork();
	struct sigaction sigact;
	
	switch (p)
	{
		case -1: return -1;
		case 0:
			//proc 1;			
			tableNum = 1;

			
			sigact.sa_handler = handler_proc1_su2;
			sigact.sa_flags = 0;			
			sigemptyset(&sigact.sa_mask);
			sigaddset(&sigact.sa_mask, SIGUSR1);
			sigaction (SIGUSR2, &sigact, NULL);
			
			
			printf("proc #%d, pid = %d, ppid = %d\n\n", tableNum, getpid(), getppid());
			
			p = fork();
			switch (p)
			{
				case -1: return -1;
				case 0: 
					//proc 2:					
					tableNum = 2;
					
					sigact.sa_handler = handler_proc2_3_su1;
					sigact.sa_flags = 0;
					sigemptyset(&sigact.sa_mask);
					sigaddset(&sigact.sa_mask, SIGUSR2);
					sigaction (SIGUSR1, &sigact, NULL);
					
					
					printf("proc #%d, pid = %d, ppid = %d\n\n", 2, getpid(), getppid());
					
					p = fork();
					switch (p)
					{
						case -1: return -1;
						case 0:
							//proc 5
							tableNum = 5;
							
							sigact.sa_handler = handler_proc5_6_7_su1;
							sigact.sa_flags = 0;
							sigemptyset(&sigact.sa_mask);
							sigaddset(&sigact.sa_mask, SIGUSR2);
							sigaction (SIGUSR1, &sigact, NULL);
							
							
							printf("proc #%d, pid = %d, ppid = %d\n\n", 5, getpid(), getppid());
							
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
									
									sigact.sa_handler = handler_proc5_6_7_su1;
									sigact.sa_flags = 0;
									sigemptyset(&sigact.sa_mask);
									sigaddset(&sigact.sa_mask, SIGUSR2);
									sigaction (SIGUSR1, &sigact, NULL);
									
									
									printf("proc #%d, pid = %d, ppid = %d\n\n", 6, getpid(), getppid());
									
									p = fork();
									switch(p)
									{
										case -1: return -1;
										case 0:
											//proc7
											tableNum = 7;
											
											sigact.sa_handler = handler_proc5_6_7_su1;
											sigact.sa_flags = 0;
											sigemptyset(&sigact.sa_mask);
											sigaddset(&sigact.sa_mask, SIGUSR2);
											sigaction (SIGUSR1, &sigact, NULL);
											
											
											printf("proc #%d, pid = %d, ppid = %d\n\n", 7, getpid(), getppid());
											
											p = fork();
											switch(p)
											{
												case -1: return -1;
												case 0:
													//proc 8
													tableNum = 8
													
													sigact.sa_handler = handler_proc8_su1;
													sigact.sa_flags = 0;
													sigemptyset(&sigact.sa_mask);
													sigaddset(&sigact.sa_mask, SIGUSR2);
													sigaction (SIGUSR1, &sigact, NULL);
													
													
													printf("proc #%d, pid = %d, ppid = %d\n\n", 8, getpid(), getppid());
													
													exit(0);
													
												default:
													//proc 7
													wait(NULL);
												
											}
											
											exit(0);
										default:
											//proc6
											wait(NULL);
									}
									
									exit(0);
								default:
									//proc 2
									wait(NULL);
									wait(NULL);
							}
					}
					
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
							
							sigact.sa_handler = handler_proc2_3_su1;
							sigact.sa_flags = 0;
							sigemptyset(&sigact.sa_mask);
							sigaddset(&sigact.sa_mask, SIGUSR2);
							sigaction (SIGUSR1, &sigact, NULL);
							
							
							printf("proc #%d, pid = %d, ppid = %d\n\n", 3, getpid(), getppid());
							
							
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
									
									sigact.sa_handler = handler_proc4_su2;
									sigact.sa_flags = 0;
									sigemptyset(&sigact.sa_mask);
									sigaddset(&sigact.sa_mask, SIGUSR1);
									sigaction (SIGUSR2, &sigact, NULL);
									
									
									printf("proc #%d, pid = %d, ppid = %d\n\n", 4, getpid(), getppid());
									
									exit(0);
									
								default:
									//proc 1
									wait(NULL);
									wait(NULL);
									wait(NULL);
							}
					}
			}
			
			exit(0);
		default:
			//proc 0
			printf("proc #%d, pid = %d, ppid = %d\n\n", 0, getpid(), getppid());
			wait(NULL);
	}
	
	return 0;
}
