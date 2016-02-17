#include "os.h"
#include "sys_cfg.h"
#include "tinythread.h"
#include "intercom.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>
#include <errno.h>
#include <assert.h>
#include <event.h>

// Main Function

int main(int argc, const char * argv[]) {
	event_init();
	task_init(0, 0, NULL);

	while (!isExitRequested()) {
		event_dispatch();
	}

	printf("</end>\n");

	return EXIT_SUCCESS;
}
