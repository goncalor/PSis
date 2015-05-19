#include "TCPlib.h"
#include <arpa/inet.h>
#include <stdlib.h>
#include <stdio.h>

/* length-prefix framing for protocol buffers */
/* care is taken so that the lenght of the messages uses proper endianness */

/* writes "len" bytes of "message" in a length-prefixed frame to the socket associated to "fd". returns 0 on success */
int PROTOsend(int fd, char *message, unsigned len)
{
	int len_aux = htonl(len);

#ifdef DEBUG
	printf("len_aux %d\n", len);
#endif
	return TCPsend(fd, (char*) &len_aux, 4) + TCPsend(fd, message, len);
}

/* reads a a lenght-prefixed message from the socket associated to "fd" into "message". message is allocated with the needed size. returns the length of the message or negative on error. */
/* the fact that the message is allocated with the size of the prefix-length is exploitable */
int PROTOrecv(int fd, char **message)
{
	char len_char[4];
	char *ptr = len_char;
	unsigned len;
	int nleft = 4, nread;

	while(nleft>0)
	{
		nread = TCPrecv(fd, len_char, nleft);
		if(nread < 0)
			return -1;
		nleft -= nread;
		ptr += nread;
	}   

	len = ntohl(*((uint32_t*) len_char));
#ifdef DEBUG
	printf("len_aux %d\n", len);
#endif
	*message = malloc(len*sizeof(char));
	if(*message == NULL)
	{
		perror("malloc in PROTOrecv()");
		exit(EXIT_FAILURE);
	}

	nleft = len;
	while(nleft>0)
	{
		nread = TCPrecv(fd, *message, nleft);
		if(nread < 0)
			return -2;
		nleft -= nread;
		ptr += nread;
	}   

	return len;
}
