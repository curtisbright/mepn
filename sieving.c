#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/wait.h>

int pid = -1;

void sig_handler(int signo)
{	if(signo == SIGINT)
	{	if(pid!=-1)
			kill(pid, SIGINT);
		int status;
		wait(&status);
		system("./merge");
		exit(0);
	}
}

int main(int argc, char** argv)
{	if(argc==1)
	{	printf("Given a base on the command line, this program runs\n");
		printf("srsieve on that base sieve file and then\n");
		printf("merges the results with the LLR search data\n");
		printf("\nNOTE: The program srsieve must be located in the directory srsieve\n");
	}
	else if(access("srsieve/srsieve", F_OK)==-1)
		printf("The program srsieve must be located in the directory srsieve\n");
	else if(argc>1)
	{	signal(SIGINT, sig_handler);

		char filename[25];
		sprintf(filename, "srsieve/sieve.%s.txt", argv[1]);
		if(access(filename, F_OK)==-1)
		{	char filename2[25], copy[100];
			sprintf(filename2, "data/sieve.%s.txt", argv[1]);
			if(access(filename2, F_OK)==-1)
			{	printf("No sieve file for base %s!\n", argv[1]);
				return 0;
			}
			sprintf(copy, "cp %s %s", filename2, filename);
			system(copy);
		}

		for(;;)
		{	pid = fork();
			if(pid==0)
				execlp("timeout", "timeout", "120m", "srsieve/srsieve", filename, "-o", filename, NULL);
			int status;
			wait(&status);
			system("./merge");
		}
	}

	return 0;
}
