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

PwmDevice PwmDevice::s_PwmDevices[PwmDevice::NUM_PWM_DEVICES];
intercom::DataMessage::PwmMsg::Signal PwmDevice::s_PwmSignals[PwmDevice::NUM_PWM_DEVICES];

const uint32_t PwmDevice::s_PwmIds[PwmDevice::NUM_PWM_DEVICES] = {
	[PWM_INTERIOR_LIGHT]     = 'IntL',
	[PWM_BRAKE_LIGHT]        = 'BrkL',
	[PWM_BACKWARDS_LIGHT]    = 'BckL',
	[PWM_LEFT_HAZARD_LIGHT]  = 'LHaz',
	[PWM_RIGHT_HAZARD_LIGHT] = 'RHaz',
};

PwmDevice::PwmDevice() : m_State(Pwm_Undefined) {
}

PwmDevice::~PwmDevice() {
}

void PwmDevice::resetAllDevices() {
	for (int pwm_id = 0; pwm_id < PwmDevice::NUM_PWM_DEVICES; ++pwm_id) {
		PwmDevice::s_PwmDevices[pwm_id].m_State = PwmDevice::Pwm_Undefined;
		PwmDevice::s_PwmSignals[pwm_id].Id = ntohl(PwmDevice::s_PwmIds[pwm_id]);
		PwmDevice::s_PwmSignals[pwm_id].setPeriod(0);
		PwmDevice::s_PwmSignals[pwm_id].setPulseWidth(0);
	}
}

// Public API

extern "C" uint8_t PwmOut_getNumberOfDevices() {
	return PwmDevice::NUM_PWM_DEVICES;
}

extern "C" PwmOutputStatus PwmOut_getStatus(PwmDevId pwm_id) {
	if ((pwm_id < 0) || (pwm_id >= PwmDevice::NUM_PWM_DEVICES)) {
		return PwmOutputStatus_Undefined;
	}
	switch (PwmDevice::s_PwmDevices[pwm_id].m_State) {
		case PwmDevice::Pwm_Standby:
			return PwmOutputStatus_Standby;
		case PwmDevice::Pwm_Active:
			return PwmOutputStatus_Active;
		default:
			return PwmOutputStatus_Undefined;
	}
}

extern "C" PwmOutputError PwmOut_init(PwmDevId pwm_id) {
	if ((pwm_id < 0) || (pwm_id >= PwmDevice::NUM_PWM_DEVICES)) {
		return PwmOutputError_WrongDevice;
	}
	PwmDevice::s_PwmDevices[pwm_id].m_State = PwmDevice::Pwm_Standby;
	PwmDevice::s_PwmSignals[pwm_id].Id = ntohl(PwmDevice::s_PwmIds[pwm_id]);
	PwmDevice::s_PwmSignals[pwm_id].setPeriod(0);
	PwmDevice::s_PwmSignals[pwm_id].setPulseWidth(0);
	return PwmOutputError_Ok;
}

extern "C" PwmOutputError PwmOut_start(PwmDevId pwm_id) {
	if ((pwm_id < 0) || (pwm_id >= PwmDevice::NUM_PWM_DEVICES)) {
		return PwmOutputError_WrongDevice;
	}
	if (PwmDevice::s_PwmDevices[pwm_id].m_State != PwmDevice::Pwm_Standby) {
		return PwmOutputError_IllegalState;
	}
	PwmDevice::s_PwmDevices[pwm_id].m_State = PwmDevice::Pwm_Active;
	return PwmOutputError_Ok;
}

extern "C" PwmOutputError PwmOut_stop(PwmDevId pwm_id) {
	if ((pwm_id < 0) || (pwm_id >= PwmDevice::NUM_PWM_DEVICES)) {
		return PwmOutputError_WrongDevice;
	}
	if (PwmDevice::s_PwmDevices[pwm_id].m_State != PwmDevice::Pwm_Active) {
		return PwmOutputError_IllegalState;
	}
	PwmDevice::s_PwmDevices[pwm_id].m_State = PwmDevice::Pwm_Standby;
	return PwmOutputError_Ok;
}

extern "C" PwmOutputError PwmOut_setPeriod(PwmDevId pwm_id, uint16_t period) {
	if ((pwm_id < 0) || (pwm_id >= PwmDevice::NUM_PWM_DEVICES)) {
		return PwmOutputError_WrongDevice;
	}
	if (PwmDevice::s_PwmDevices[pwm_id].m_State == PwmDevice::Pwm_Undefined) {
		return PwmOutputError_IllegalState;
	}
	PwmDevice::s_PwmSignals[pwm_id].setPeriod(period);
	if (PwmDevice::s_PwmSignals[pwm_id].setPulseWidth(0)) {
		intercom::Sender sender(intercom::Sys_Ecu);
		intercom::DataMessage msg;
		msg.createPwmMsg(PwmDevice::s_PwmSignals[pwm_id]);
		sender.send(msg);
	}
	return PwmOutputError_Ok;
}

extern "C" PwmOutputError PwmOut_setDuty(PwmDevId pwm_id, uint16_t duty) {
	if ((pwm_id < 0) || (pwm_id >= PwmDevice::NUM_PWM_DEVICES)) {
		return PwmOutputError_WrongDevice;
	}
	if (PwmDevice::s_PwmDevices[pwm_id].m_State == PwmDevice::Pwm_Undefined) {
		return PwmOutputError_IllegalState;
	}
	if (duty > PwmDevice::s_PwmSignals[pwm_id].Period) {
		duty = PwmDevice::s_PwmSignals[pwm_id].Period;
	}
	if (PwmDevice::s_PwmSignals[pwm_id].setPulseWidth(duty)) {
		intercom::Sender sender(intercom::Sys_Ecu);
		intercom::DataMessage msg;
		msg.createPwmMsg(PwmDevice::s_PwmSignals[pwm_id]);
		sender.send(msg);
	}
	return PwmOutputError_Ok;
}
