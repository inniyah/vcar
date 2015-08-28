#ifndef EMU_PWM_H_8AE697F8_4C11_11E5_95FB_10FEED04CD1C
#define EMU_PWM_H_8AE697F8_4C11_11E5_95FB_10FEED04CD1C

#include "../os.h"
#include "emu_ecu.h"

#include "fast_mutex.h"

struct PwmDevice {
	PwmDevice();
	virtual ~PwmDevice();

	bool insertTxMessage(CanMsgId msg_id, uint8_t dlc, uint8_t * payload);
	bool insertRxMessage(CanMsgId msg_id, uint8_t dlc, uint8_t * payload);

	static const int NUM_PWM_DEVICES = NUMBER_OF_PWM_DEVICES;

	static PwmDevice m_PwmDevices[PwmDevice::NUM_PWM_DEVICES];

	typedef enum PwmStateE {
		Pwm_Undefined,
		Pwm_Standby,
		Pwm_Active,
	} PwmState;

	uint32_t m_Id;
	PwmState m_State;
	uint16_t m_Period;
	uint16_t m_Duty;
};

#endif // EMU_PWM_H_8AE697F8_4C11_11E5_95FB_10FEED04CD1C