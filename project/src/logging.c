#include "define.h"
#include "logging.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

/* creates a log of the form name-PID.ext. returns a file descriptor on success or -1 on error */
int LOGcreate(char *name, char *ext)
{
	char log_name[strlen(name) + strlen(ext) + 9];	// save some space for a PID and dot

	sprintf(log_name, "%s-%d.%s", name, (int) getpid(), ext);
	#ifdef DEBUG
	printf("creating log file: %s\n", log_name);
	#endif

	return open(log_name, O_CREAT | O_RDWR | O_APPEND, 0600);
}


/* a line of the form "nr line\n" is appended to the file associated to fd 
 * returns 0 on success and -1 on failure */
int LOGadd(int fd, int nr, char *line)
{
	char aux[sizeof(char)*(strlen(line) + 2 + 16)];	// 1 for \n and 1 for \0 and spare bytes for number

	sprintf(aux, "%d %s\n", nr, line);

	#ifdef DEBUG
	printf("adding to the log: %s", aux);
	#endif

	return write(fd, aux, strlen(aux));
}
