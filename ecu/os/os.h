#ifndef OSOS_H_
#define OSOS_H_

#include <stdint.h>
#include <event.h>

#ifdef __cplusplus

#define ISR(x) extern "C" void isr_##x(int, short , void *)
#define TASK(x) extern "C" void task_##x(int, short , void *)
#define DEF_EVENT(x) extern "C" { struct event ev_##x; }

#else

#define ISR(x) void isr_##x(int, short , void *)
#define TASK(x) void task_##x(int, short , void *)
#define EVENT(x) struct event ev_##x;

#endif

#define EVENT(x) ev_##x

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

// Can Bus

#define CAN_MAX_SIZE 8

typedef uint8_t  CanDevId;
typedef uint32_t CanMsgId;

typedef struct CanMessageS {
	CanMsgId Id;                   /* 32 bit CAN_ID + EFF/RTR/ERR flags */
	uint8_t Dlc;                   /* frame payload length in byte (0 .. CAN_MAX_DLEN) */
	uint8_t Payload[CAN_MAX_SIZE];
} __attribute__((packed)) CanMessage;

typedef enum {
	CanTransceiverStatus_Undefined,
	CanTransceiverStatus_Standby,
	CanTransceiverStatus_Active,
} CanTransceiverStatus;

typedef enum {
	CanTransceiverError_Ok,
	CanTransceiverError_IllegalState,
	CanTransceiverError_WrongDevice,
} CanTransceiverError;

typedef enum {
	CanDriverStatus_Undefined,
	CanDriverStatus_Closed,
	CanDriverStatus_Initialized,
	CanDriverStatus_Open,
	CanDriverStatus_Muted,
} CanDriverStatus;

typedef enum {
	CanDriverError_Ok,
	CanDriverError_TxHwQueueFull,
	CanDriverError_TxOffline,
	CanDriverError_TxFail,
	CanDriverError_IllegalState,
	CanDriverError_WrongDevice,
} CanDriverError;

#ifdef __cplusplus
extern "C" {
#endif

uint8_t              CanSystem_getNumberOfDevices();

CanTransceiverStatus CanTransceiver_getStatus(CanDevId can_id);
CanTransceiverError  CanTransceiver_init(CanDevId can_id);
CanTransceiverError  CanTransceiver_start(CanDevId can_id);
CanTransceiverError  CanTransceiver_stop(CanDevId can_id);

CanDriverStatus      CanDriver_getStatus(CanDevId can_id);
CanDriverError       CanDriver_open(CanDevId can_id);
CanDriverError       CanDriver_close(CanDevId can_id);
CanDriverError       CanDriver_mute(CanDevId can_id);
CanDriverError       CanDriver_unmute(CanDevId can_id);

#ifdef __cplusplus
}
#endif

#endif
