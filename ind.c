#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main()
{

	pid_t p0, p1, p2, p3, p4, p5, p6, p7;
	
	pid_t p;
	
	p0 = fork();
	
	switch (p0)
	{
		case -1: return -1;
		case 0:
			//proc 1;
			printf("proc #%d, pid = %d, ppid = %d\n\n", 1, getpid(), getppid());
			
			p1 = fork();
			switch (p1)
			{
				case -1: return -1;
				case 0: 
					//proc 2:
					printf("proc #%d, pid = %d, ppid = %d\n\n", 2, getpid(), getppid());
					
					p4 = fork();
					switch (p4)
					{
						case -1: return -1;
						case 0:
							//proc 5
							printf("proc #%d, pid = %d, ppid = %d\n\n", 5, getpid(), getppid());
							
							exit(0);
						default:
							//proc 2
							p5 = fork();
							switch (p5)
							{
								case -1: return -1;
								case 0: 
									//proc 6
									printf("proc #%d, pid = %d, ppid = %d\n\n", 6, getpid(), getppid());
									
									p6 = fork();
									switch(p6)
									{
										case -1: return -1;
										case 0:
											//proc7
											printf("proc #%d, pid = %d, ppid = %d\n\n", 7, getpid(), getppid());
											
											p7 = fork();
											switch(p7)
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
					p2 = fork();
					switch (p2)
					{
						case -1: return -1;
						case 0:
							//proc 3
							printf("proc #%d, pid = %d, ppid = %d\n\n", 3, getpid(), getppid());
							
							
							exit(0);
						default:
						//proc 1
						
							p3 = fork();
							switch(p3)
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
