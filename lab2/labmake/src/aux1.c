#include <time.h>
#include <stdlib.h>

void fazcontas(struct tm *local) {

	int tmp;
	tmp = local->tm_wday-4;
        local->tm_wday = tmp > 0 ? 7-tmp : abs(tmp);
        local->tm_hour = 23-local->tm_hour;
        local->tm_min = 59-local->tm_min;
	local->tm_sec = 59-local->tm_sec;
}

void fazcontas2(struct tm *local) {

        local->tm_yday = local->tm_yday >= 353 ? 353 + 364 - local->tm_yday : 353 - local->tm_yday;
        local->tm_hour = 23-local->tm_hour;
        local->tm_min = 59-local->tm_min;
        local->tm_sec = 59-local->tm_sec;
}
