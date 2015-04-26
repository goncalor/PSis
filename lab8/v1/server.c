#include <pthread.h>

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int i = 0;

void * thread_code(void *arg){
	int value = *((int *)arg);
	
	while(1){
		sleep(value);
		printf("Thread %d %d\n", value, i);
		i++;
	}
	pthread_exit(NULL);
}


int main(){
	pthread_t thread_id_1;
	pthread_t thread_id_2;
	void * res;
	int arg_1;
	int arg_2;
	
	arg_1 = 1;
	pthread_create(&thread_id_1, NULL, thread_code, &arg_1);
	printf("New thread %d\n", (int) thread_id_1);

	arg_2 = 2;	
	pthread_create(&thread_id_2, NULL, thread_code, &arg_2);
	printf("New thread %d\n", (int) thread_id_2);

	getchar();

	pthread_join(thread_id_1, &res);
	pthread_join(thread_id_2, &res);
	exit(0);
}
