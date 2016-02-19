#ifndef OS_SCHED_H_7D9F1B62_D666_11E5_86AC_10FEED04CD1C
#define OS_SCHED_H_7D9F1B62_D666_11E5_86AC_10FEED04CD1C

#include "os_cfg.h"

#include <stdint.h>
#include <event.h>

#ifdef __cplusplus

#define ISR(x) extern "C" void isr_##x(int, short , void *)
#define TASK(x) extern "C" void task_##x(int, short , void *)

#else

#define ISR(x) void isr_##x(int, short , void *)
#define TASK(x) void task_##x(int, short , void *)

#endif

typedef struct event event;

// Task Functions

TASK(init);
TASK(100ms);
TASK(10ms);

// Interrupt Service Routines (ISRs)

ISR(CAN_MSG_RECV);
ISR(CAN_MSG_SENT);

// Events

#ifdef __cplusplus
extern "C" {
#endif

void requestExit();
int isExitRequested();

void addEventAfterUs  (struct event & ev, uint64_t us, void (*fn)(int, short, void *), void * arg);
void addEventAfterMs  (struct event & ev, uint64_t ms, void (*fn)(int, short, void *), void * arg);
void addEventAfterSec (struct event & ev, uint32_t s,  void (*fn)(int, short, void *), void * arg);
void addEventEveryUs  (struct event & ev, uint64_t us, void (*fn)(int, short, void *), void * arg);
void addEventEveryMs  (struct event & ev, uint64_t ms, void (*fn)(int, short, void *), void * arg);
void addEventEverySec (struct event & ev, uint64_t s,  void (*fn)(int, short, void *), void * arg);

#ifdef __cplusplus
}
#endif

#endif // OS_SCHED_H_7D9F1B62_D666_11E5_86AC_10FEED04CD1C
