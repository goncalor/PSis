#include <time.h>
#include <stdio.h>
#include "aux2.h"

void imprime(struct tm *t) {
        printf("Faltam %d dias %dh %dm %ds para a proxima 6a feira\n", t->tm_wday, t->tm_hour, t->tm_min, t->tm_sec);
}

void imprime2(struct tm *t) {
        printf("Faltam %d dias %dh %dm %ds para o fim do 1o semestre!\n",t->tm_yday, t->tm_hour, t->tm_min, 
t->tm_sec); }

