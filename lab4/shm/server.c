#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>	/* For O_* constants */
#include <unistd.h>
#include <sys/types.h>
#include "heartbeat.h"

int main(int argc, char * argv[]){
	int task_id, fd_mem, dead_count, times_dead=0;
	shm *shm_addr;

	fd_mem = shm_open("/heartbeat", O_RDWR | O_CREAT, 0600);
	if(fd_mem == -1)
		puts("Failed to open shared memory");

	ftruncate(fd_mem, sizeof(shm));
	if(fd_mem == -1)
		puts("Failed to truncate shared memory");

	shm_addr = mmap(NULL, sizeof(shm), PROT_WRITE, MAP_SHARED, fd_mem, 0);
	if(shm_addr == MAP_FAILED)
		puts("nmap failed");

	while(1)
	{
		dead_count = 0;
		for(task_id = 0; task_id < MAX_TASKS; task_id++)
		{
			printf("Checking task %d... ", task_id);

			if(shm_addr->task[task_id] > 0)
			{
				puts("OK.");
				shm_addr->task[task_id] -= 1;
			}
			else
			{
				puts("dead.");
				dead_count++;
			}
		}
		puts("");
		if(dead_count == MAX_TASKS)
		{
			puts("All tasks are dead.\n\n");
			times_dead++;
			if(times_dead == 4)
			{
				puts("EXITING NOW!!!\nBYE BYE NIGAAAA.\n\n");
				return EXIT_SUCCESS;
			}
		}
		else
			times_dead = 0;

		sleep(TICK_TIME);
	}

	exit(EXIT_SUCCESS);
}
