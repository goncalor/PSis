#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>           /* For O_* constants */
#include "heartbeat.h"

int main(int argc, char * argv[]){
	int task_id = -1, fd_mem;
	shm *shm_addr;

	if(argc > 1){
		sscanf(argv[1], "%d", &task_id);
	}

	if (task_id ==-1){
		printf("Task identifier is missing\n");
		exit(1);
	}else{
		if(task_id >= MAX_TASKS)
		{
			printf("Task_id must be less than %d", MAX_TASKS);
			exit(EXIT_FAILURE);
		}
		printf("Taks_id %d\n", task_id);
	}


	fd_mem = shm_open("/heartbeat", O_RDWR, 0600);
	if(fd_mem == -1)
		puts("Failed to open shared memory");

	shm_addr = mmap(NULL, sizeof(shm), PROT_WRITE, MAP_SHARED, fd_mem, 0);
	if(shm_addr == MAP_FAILED)
		puts("nmap failed");

	while(1){
		printf("sending heartbeat\n");
		shm_addr->task[task_id] = TICKS_FAILURE;

		sleep(TICK_TIME);
	}

	exit(0);
}
