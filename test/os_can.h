#ifndef OS_CAN_H_7D9F1C2A_D666_11E5_86AE_10FEED04CD1C
#define OS_CAN_H_7D9F1C2A_D666_11E5_86AE_10FEED04CD1C

#include "os_cfg.h"
#include "os_sched.h"

#include <stdint.h>

#define CAN_MAX_SIZE 8

typedef uint8_t  CanDevId;
typedef uint32_t CanMsgId;

#define INVALID_CAN_DEVICE ((CanDevId)~0)

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
	CanTransceiverError_Ok = 0,
	CanTransceiverError_IllegalState,
	CanTransceiverError_WrongDevice,
} CanTransceiverError;

typedef enum {
	CanDriverStatus_Undefined,
	CanDriverStatus_Close,
	CanDriverStatus_Silent,
	CanDriverStatus_Open,
} CanDriverStatus;

typedef enum {
	CanDriverError_Ok = 0,
	CanDriverError_TxHwQueueFull,
	CanDriverError_TxOffline,
	CanDriverError_TxFail,
	CanDriverError_RxEmptyQueue,
	CanDriverError_IllegalState,
	CanDriverError_WrongDevice,
} CanDriverError;

#ifdef __cplusplus
extern "C" {
#endif

uint8_t              CanSystem_getNumberOfDevices();
CanDevId             CanSystem_getIsrCurrentDevId();

CanTransceiverStatus CanTransceiver_getStatus(CanDevId can_id);
CanTransceiverError  CanTransceiver_init(CanDevId can_id);
CanTransceiverError  CanTransceiver_start(CanDevId can_id);
CanTransceiverError  CanTransceiver_stop(CanDevId can_id);

CanDriverStatus      CanDriver_getStatus(CanDevId can_id);
CanDriverError       CanDriver_init(CanDevId can_id);
CanDriverError       CanDriver_close(CanDevId can_id);
CanDriverError       CanDriver_open(CanDevId can_id);
CanDriverError       CanDriver_silent(CanDevId can_id);
CanMessage *         CanDriver_getRxMessage(CanDevId can_id);
CanDriverError       CanDriver_delRxMessage(CanDevId can_id);

#ifdef __cplusplus
}
#endif

#endif // OS_CAN_H_7D9F1C2A_D666_11E5_86AE_10FEED04CD1C
