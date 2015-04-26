
all: server relauncher

server: server.c
	gcc -Wall server.c -o server -lrt -lpthread

relauncher: relauncher.c
	gcc -Wall relauncher.c -o relauncher

.PHONY: clean
clean:
	$(RM) server relauncher
