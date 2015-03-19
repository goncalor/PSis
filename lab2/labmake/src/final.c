#include <stdio.h>
#include <time.h>
#include "aux1.h"
#include "aux2.h"

int main() {
	/* Funcoes/estruturas do time */
	time_t now;
	struct tm *local;
	now = time((time_t *)NULL);
	local = localtime(&now);
	
	/* Definidos nos aux?.c */
	fazcontas2(local);
	imprime2(local);

	return 0;
}
