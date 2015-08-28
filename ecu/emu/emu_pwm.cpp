#include "emu_pwm.h"

#include "tinythread.h"
#include "intercom.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <assert.h>
#include <sys/time.h>

PwmDevice PwmDevice::m_PwmDevices[PwmDevice::NUM_PWM_DEVICES];

PwmDevice::PwmDevice() :
	m_Id(0),
	m_State(Pwm_Undefined),
	m_Period(0),
	m_Duty(0)
{
}

PwmDevice::~PwmDevice() {
}

// Public API

extern "C" uint8_t PwmOut_getNumberOfDevices() {
	return PwmDevice::NUM_PWM_DEVICES;
}

extern "C" PwmOutputError PwmOut_setPeriod(PwmDevId pwm_id, uint16_t period) {
	if ((pwm_id < 0) || (pwm_id >= PwmDevice::NUM_PWM_DEVICES)) {
		return PwmOutputError_WrongDevice;
	}
	return PwmOutputError_Ok;
}

extern "C" PwmOutputError PwmOut_setDuty(PwmDevId pwm_id, uint16_t duty) {
	if ((pwm_id < 0) || (pwm_id >= PwmDevice::NUM_PWM_DEVICES)) {
		return PwmOutputError_WrongDevice;
	}
	return PwmOutputError_Ok;
}
