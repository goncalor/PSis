#define MAX_TASKS 7
#define TICK_TIME 5
#define TICKS_FAILURE 4

typedef struct shm {
	char task[MAX_TASKS];
} shm;

