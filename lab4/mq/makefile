CC = gcc
CFLAGS = -Wall
LDFLAGS = -lrt

vpath %.c src
vpath %.o obj
vpath %.h include
vpath % bin

all: server task

server: server.o
	$(CC) $(CFLAGS) $(addprefix obj/, $^) -o bin/$@ $(LDFLAGS)

task: task.o
	$(CC) $(CFLAGS) $(addprefix obj/, $^) -o bin/$@ $(LDFLAGS) 

%.o: %.c
	$(CC) $(CFLAGS) -Iinclude -c $< -o obj/$@ $(LDFLAGS)

server.o: heartbeat.h

task.o: heartbeat.h

.PHONY: clean
clean:
	$(RM) obj/*.o bin/server bin/task
