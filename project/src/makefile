TARGET = server
CLIENT = client
CC = gcc
CFLAGS = -Wall 
DEPS = TCPlib.h utils.h inetutils.h define.h chatstorage.h threads.h crashrecovery.h # messages.pb-c.h 
OBJ = $(TARGET).o TCPlib.o utils.o inetutils.o chatstorage.o threads.o crashrecovery.o # messages.pb-c.o
EDITOR = gedit

.PHONY: all

all: $(TARGET) $(CLIENT)

$(TARGET): $(DEPS) $(OBJ)
	$(CC) $(CFLAGS) $^ -o $@ -lprotobuf-c -lpthread

$(CLIENT): $(CLIENT).o
	$(CC) $(CFLAGS) $^ -o $@ -lprotobuf-c

%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c $< -o $@


.PHONY: clean o

c:
	clear	
	$(RM) *.o

o:
	$(EDITOR) $(TARGET).c *.c *.h makefile &


# $@ reffers to named before :
# $^ reffers to all named after :
# $< reffers to first after :
