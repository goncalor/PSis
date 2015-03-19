CC = gcc
CFLAGS = -Wall

vpath %.c src
vpath %.o obj
vpath %.h include
vpath % bin

all: sexta final

sexta: sexta.o aux1.o aux2.o
	$(CC) $(CFLAGS) $(addprefix obj/, $^) -o bin/$@

final: final.o aux1.o aux2.o
	$(CC) $(CFLAGS) $(addprefix obj/, $^) -o bin/$@

sexta.o: sexta.c aux1.h aux2.h
	$(CC) $(CFLAGS) -Iinclude -c $< -o obj/$@

%.o: %.c
	$(CC) $(CFLAGS) -Iinclude -c $< -o obj/$@

.PHONY: clean
clean:
	$(RM) obj/*.o bin/sexta bin/final
