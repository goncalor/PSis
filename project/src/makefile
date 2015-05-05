TARGET = server
CLIENT = client
CC = gcc
CFLAGS = -Wall 
DEPS = TCPlib.h utils.h inetutils.h define.h chatstorage.h messages.pb-c.h
OBJ = $(TARGET).o TCPlib.o utils.o inetutils.o chatstorage.o messages.pb-c.o
EDITOR = gedit

.PHONY: all

all: $(TARGET) $(CLIENT)

$(TARGET): $(DEPS) $(OBJ)
	$(CC) $(CFLAGS) $^ -o $@ -l protobuf-c

$(CLIENT): $(CLIENT).o
	$(CC) $(CFLAGS) $^ -o $@ -l protobuf-c

%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c $< -o $@


.PHONY: clean o

clean:
	clear	
	$(RM) *.o

o:
	$(EDITOR) $(TARGET).c *.c *.h makefile &


# $@ reffers to named before :
# $^ reffers to all named after :
# $< reffers to first after :
