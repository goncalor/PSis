CC = gcc
CFLAGS = -Wall

vpath %.c src
vpath %.o obj
vpath %.h include
vpath % bin

all: sexta #final

sexta: sexta.o aux1.o aux2.o
	$(CC) $(CFLAGS) $(addprefix obj/, $^) -o bin/$@

sexta.o: sexta.c aux1.h aux2.h
	$(CC) $(CFLAGS) -Iinclude -c $< -o obj/$@

%.o: %.c
	$(CC) $(CFLAGS) -Iinclude -c $< -o obj/$@

#aux1.o: aux1.c aux1.h
	#$(CC) $(CFLAGS) -Iinclude -c $< -o obj/$@

#aux2.o: aux2.c aux2.h
	#$(CC) $(CFLAGS) -Iinclude -c $< -o obj/$@

final: final.c aux1.o aux2.o
	$(CC) $(CFLAGS) -Iobj $^ -o bin/$@

clean:
	$(RM) obj/*.o bin/sexta bin/final

aux1.o: aux1.c aux1.h
aux2.o: aux2.c aux2.h
