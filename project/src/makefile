TARGET = server
CC = gcc
CFLAGS = -Wall 
DEPS = TCPlib.h utils.h inetutils.h define.h chatstorage.h
OBJ = $(TARGET).o TCPlib.o utils.o inetutils.o chatstorage.o
EDITOR = gedit

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) $^ -o $@

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
