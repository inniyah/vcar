#include "os.h"
#include "can.h"

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

static bool exit_requested = false;

extern "C" void requestExit() {
	exit_requested = true;
}

extern "C" int isExitRequested() {
	return (exit_requested ? ~0 : 0);
}

extern "C" void addEventAfterUs(struct event & ev, uint64_t us, void (*fn)(int, short, void *), void * arg) {
	struct timeval tv;
	tv.tv_sec = (us / 1000000);
	tv.tv_usec = (us % 1000000);
	evtimer_set(&ev, fn, arg);
	evtimer_add(&ev, &tv);
}

extern "C" void addEventAfterMs(struct event & ev, uint64_t ms, void (*fn)(int, short, void *), void * arg) {
	struct timeval tv;
	tv.tv_sec = (ms / 1000);
	tv.tv_usec = (ms % 1000) * 1000;
	evtimer_set(&ev, fn, arg);
	evtimer_add(&ev, &tv);
}

extern "C" void addEventAfterS(struct event & ev, uint32_t s, void (*fn)(int, short, void *), void * arg) {
	struct timeval tv;
	tv.tv_sec = s;
	tv.tv_usec = 0;
	evtimer_set(&ev, fn, arg);
	evtimer_add(&ev, &tv);
}

extern "C" void addEventEveryUs(struct event & ev, uint64_t us, void (*fn)(int, short, void *), void * arg) {
	struct timeval tv;
	tv.tv_sec = (us / 1000000);
	tv.tv_usec = (us % 1000000);
	event_set(&ev, 0, EV_PERSIST, fn, arg);
	evtimer_add(&ev, &tv);
}

extern "C" void addEventEveryMs(struct event & ev, uint64_t ms, void (*fn)(int, short, void *), void * arg) {
	struct timeval tv;
	tv.tv_sec = (ms / 1000);
	tv.tv_usec = (ms % 1000) * 1000;
	event_set(&ev, 0, EV_PERSIST, fn, arg);
	evtimer_add(&ev, &tv);
}

extern "C" void addEventEverySec(struct event & ev, uint64_t s, void (*fn)(int, short, void *), void * arg) {
	struct timeval tv;
	tv.tv_sec = s;
	tv.tv_usec = 0;
	event_set(&ev, 0, EV_PERSIST, fn, arg);
	evtimer_add(&ev, &tv);
}
