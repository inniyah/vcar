#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

void task_100ms(void) {
	time_t now = time(NULL);
	printf("Time: %s\n", ctime(&now));
}

void task_10ms(void) {
	printf("*");
}
