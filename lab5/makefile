
all: clean client server

server:
	gcc -Wall server-skel.c -o server

client:
	gcc -Wall client-skel.c -o client

.PHONY: clean
clean:
	$(RM) server client
