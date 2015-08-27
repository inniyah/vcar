#include "os.h"
#include "System.h"
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
	Singleton<System>::getInstance().printCanBusRxSignals(0);
}

TASK(init) {
	fprintf(stderr, "<init>\n");
	if (CanTransceiverError_Ok != CanTransceiver_init(0)) {
		fprintf(stderr, "Can't init CAN Transceiver #0\n");
	}
	if (CanTransceiverError_Ok != CanTransceiver_start(0)) {
		fprintf(stderr, "Can't start CAN Transceiver #0\n");
	}

	addEventEveryMs(ev_100ms, 100, task_100ms, NULL);
	addEventEveryMs(ev_10ms,  10,  task_10ms,  NULL);

	addEventEverySec(ev_hb, 5, heartbeat, NULL);

	fprintf(stderr, "</init>\n");
}

TASK(100ms) {
	//fprintf(stderr, "<100ms>\n");
	//time_t now = time(NULL);
	//fprintf(stderr, "Time: %s\n", ctime(&now));
}

TASK(10ms) {
	//fprintf(stderr, "<10ms>\n");
}
