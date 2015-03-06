#include <stdio.h>
#include <time.h>
#include "aux1.h"
#include "aux2.h"

main() {
	/* Funcoes/estruturas do time */
	time_t now;
	struct tm *local;
	now = time((time_t *)NULL);
	local = localtime(&now);
	
	/* Definidos nos aux?.c */
	fazcontas(local);
	imprime(local);
}
