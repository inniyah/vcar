#ifndef OS_PWM_H_7D9F1BD0_D666_11E5_86AD_10FEED04CD1C
#define OS_PWM_H_7D9F1BD0_D666_11E5_86AD_10FEED04CD1C

#include "os_cfg.h"
#include "os_sched.h"

#include <stdint.h>

typedef uint8_t  PwmDevId;

typedef enum {
	PwmOutputError_Ok = 0,
	PwmOutputError_IllegalState,
	PwmOutputError_InvalidValue,
	PwmOutputError_WrongDevice,
} PwmOutputError;

typedef enum {
	PwmOutputStatus_Undefined,
	PwmOutputStatus_Standby,
	PwmOutputStatus_Active,
} PwmOutputStatus;

#ifdef __cplusplus
extern "C" {
#endif

uint8_t              PwmOut_getNumberOfDevices();
PwmOutputStatus      PwmOut_getStatus(PwmDevId pwm_id);
PwmOutputError       PwmOut_init(PwmDevId pwm_id);
PwmOutputError       PwmOut_start(PwmDevId pwm_id);
PwmOutputError       PwmOut_stop(PwmDevId pwm_id);
PwmOutputError       PwmOut_setPeriod(PwmDevId pwm_id, uint16_t period);
PwmOutputError       PwmOut_setDuty(PwmDevId pwm_id, uint16_t duty);

#ifdef __cplusplus
}
#endif

#endif // OS_PWM_H_7D9F1BD0_D666_11E5_86AD_10FEED04CD1C
