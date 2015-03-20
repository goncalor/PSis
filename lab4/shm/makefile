CC = gcc
CFLAGS = -Wall
LDFLAGS = -lrt

vpath %.c src
vpath %.o obj
vpath %.h include
vpath % bin

all: server task

server: server.o
		$(CC) $(CFLAGS) $(LDFLAGS) $(addprefix obj/, $^) -o bin/$@

task: task.o
		$(CC) $(CFLAGS) $(LDFLAGS) $(addprefix obj/, $^) -o bin/$@

%.o: %.c
		$(CC) $(CFLAGS) $(LDFLAGS) -Iinclude -c $< -o obj/$@

server.o: heartbeat.h

task.o: heartbeat.h

.PHONY: clean
clean:
	$(RM) obj/*.o bin/server bin/task
