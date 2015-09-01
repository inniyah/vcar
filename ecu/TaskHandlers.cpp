#include "os.h"
#include "BspSystem.h"
#include "MainSystem.h"
#include "AbstractCanMsgHandler.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

using namespace common;

event ev_100ms;
event ev_10ms;

event ev_hb;
void heartbeat(int fd, short event, void *arg) {
	printf("Heart Beat!\n");
	Singleton<MainSystem>::getInstance().printCanBusRxSignals(0);
}

TASK(init) {
	fprintf(stderr, "<init>\n");

	Singleton<BspSystem>::getInstance().init();
	Singleton<MainSystem>::getInstance().init();

	addEventEveryMs(ev_100ms, 100, task_100ms, NULL);
	addEventEveryMs(ev_10ms,  10,  task_10ms,  NULL);

	addEventEverySec(ev_hb, 5, heartbeat, NULL);

	fprintf(stderr, "</init>\n");
}

TASK(100ms) {
	//fprintf(stderr, "<100ms>\n");
	//time_t now = time(NULL);
	//fprintf(stderr, "Time: %s\n", ctime(&now));
	Singleton<MainSystem>::getInstance().runSwcCyclic();
}

TASK(10ms) {
	//fprintf(stderr, "<10ms>\n");
}
