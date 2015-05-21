TARGET = server
CLIENT = client
CC = gcc
CFLAGS = -Wall -g
DEPS = TCPlib.h utils.h inetutils.h define.h chatstorage.h threads.h crashrecovery.h server.h relauncher.h protobufutils.h messages.pb-c.h 
OBJ = $(TARGET).o TCPlib.o utils.o inetutils.o chatstorage.o threads.o crashrecovery.o main.o server.o relauncher.o protobufutils.o messages.pb-c.o
EDITOR = gedit

.PHONY: all

all: $(TARGET) $(CLIENT)

$(TARGET): $(DEPS) $(OBJ)
	$(CC) $(CFLAGS) $^ -o $@ -lprotobuf-c -lpthread

$(CLIENT): $(CLIENT).o $(CLIENT).h messages.pb-c.o TCPlib.o inetutils.o protobufutils.o
	$(CC) $(CFLAGS) $^ -o $@ -lprotobuf-c

%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c $< -o $@

messages.pb-c.c: messages.proto
	protoc-c messages.proto --c_out=.

messages.pb-c.h: messages.proto
	protoc-c messages.proto --c_out=.


.PHONY: clean o

c:
	clear	
	$(RM) *.o

o:
	$(EDITOR) $(TARGET).c *.c *.h makefile &


# $@ reffers to named before :
# $^ reffers to all named after :
# $< reffers to first after :
