#define SERVER_FIFO "/tmp/storyserver_fifo"
#define MESSAGE_LEN 100

typedef struct message{
	int n_fifo;
	char buffer[MESSAGE_LEN];
} message;
