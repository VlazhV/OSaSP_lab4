#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main()
{
	
	pid_t p = fork();
	switch (p)
	{
		case -1: return -1;
		case 0:
			//proc 1;
			printf("proc #%d, pid = %d, ppid = %d\n\n", 1, getpid(), getppid());
			
			p = fork();
			switch (p)
			{
				case -1: return -1;
				case 0: 
					//proc 2:
					printf("proc #%d, pid = %d, ppid = %d\n\n", 2, getpid(), getppid());
					
					p = fork();
					switch (p)
					{
						case -1: return -1;
						case 0:
							//proc 5
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
									printf("proc #%d, pid = %d, ppid = %d\n\n", 6, getpid(), getppid());
									
									p = fork();
									switch(p)
									{
										case -1: return -1;
										case 0:
											//proc7
											printf("proc #%d, pid = %d, ppid = %d\n\n", 7, getpid(), getppid());
											
											p = fork();
											switch(p)
											{
												case -1: return -1;
												case 0:
													//proc 8
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
