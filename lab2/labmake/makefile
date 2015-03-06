CC = gcc
CFLAGS = -Wall

vpath %.c src
vpath %.o obj
vpath %.h include

all: sexta #final

sexta: sexta.c aux1.o aux2.o
	$(CC) $(CFLAGS) -Iobj $^ -o bin/$@

aux1.o: aux1.c
	$(CC) $(CFLAGS) -c $^ -o obj/$@ -lm

aux2.o: aux2.c aux2.h
	$(CC) $(CFLAGS) -Iinclude -c $< -o obj/$@

final: final.c aux1.o aux2.o
	$(CC) $(CFLAGS) -Iobj $^ -o bin/$@

clean:
	$(RM) obj/*.o bin/sexta bin/final
