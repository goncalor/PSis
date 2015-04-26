
all: server relauncher

server: server.c heartbeat.h
	gcc -Wall server.c -o server -lrt -lpthread

relauncher: relauncher.c heartbeat.h
	gcc -Wall relauncher.c -o relauncher -lrt -lpthread

.PHONY: clean
clean:
	$(RM) server relauncher
