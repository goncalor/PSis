#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

void launch()
{
	pid_t pid;
	pid = fork();
	
	if(pid == 0)
	{
		//child
		if(execlp("./server", "server", NULL) == -1)
		{
			perror("execve");
			exit(-1);
		}
	}
}

int main()
{
	int status;
	
	while(1)
	{
		launch();
		wait(&status);
		printf("wait() status %d\n", status);
	}

	return 0;
}
