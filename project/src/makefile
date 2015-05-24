TARGET = server
CLIENT = client
CONTROLLER = server-controller
CC = gcc
CFLAGS = -Wall -g
DEPS = TCPlib.h utils.h inetutils.h define.h chatstorage.h threads.h crashrecovery.h server.h relauncher.h protobufutils.h messages.pb-c.h list.h item.h clientlist.h controllerfifos.h logging.h
OBJ = main.o server.o TCPlib.o utils.o inetutils.o chatstorage.o threads.o crashrecovery.o main.o server.o relauncher.o protobufutils.o messages.pb-c.o list.o clientlist.o controllerfifos.o logging.o

DEPS_client = $(CLIENT).h messages.pb-c.h TCPlib.h inetutils.h define.h protobufutils.h client.h boolean.h
OBJ_client = $(CLIENT).o messages.pb-c.o TCPlib.o inetutils.o protobufutils.o client.o 

DEPS_controller = messages.pb-c.h TCPlib.h protobufutils.h boolean.h controllerfifos.h define.h
OBJ_controller = inputcontroller.o messages.pb-c.o TCPlib.o protobufutils.o inputcontroller.o

TESTINGDIR = ../testing
EDITOR = gedit

.PHONY: all

all: $(TARGET) $(CLIENT) $(CONTROLLER)

$(TARGET): $(DEPS) $(OBJ)
	$(CC) $(CFLAGS) $^ -o $@ -lprotobuf-c -lpthread

$(CLIENT): $(DEPS_client) $(OBJ_client)
	$(CC) $(CFLAGS) $^ -o $@ -lprotobuf-c

$(CONTROLLER): $(DEPS_controller) $(OBJ_controller)
	$(CC) $(CFLAGS) $^ -o $@ -lprotobuf-c

%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c $< -o $@

messages.pb-c.c: messages.proto
	protoc-c messages.proto --c_out=.

messages.pb-c.h: messages.proto
	protoc-c messages.proto --c_out=.


.PHONY: clean o tests

c:
	clear	
	$(RM) *.o server-*.log
	cd $(TESTINGDIR) ; make clean

tests:
	cd $(TESTINGDIR) ; make all

o:
	$(EDITOR) $(TARGET).c *.c *.h makefile &


# $@ reffers to named before :
# $^ reffers to all named after :
# $< reffers to first after :
