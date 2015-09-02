#include "LightSystem.h"
#include "os.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

LightSystem LightSystem::instance;

PwmOutput::PwmOutput(PwmDevId id) : m_PwmDevId(id) {
}

bool PwmOutput::init() {
	if (PwmOutputError_Ok != PwmOut_init(m_PwmDevId)) {
		fprintf(stderr, "Can't init PWM Output #%d\n", m_PwmDevId);
		return false;
	}
	return true;
}

bool PwmOutput::start() {
	if (PwmOutputError_Ok != PwmOut_start(m_PwmDevId)) {
		fprintf(stderr, "Can't start PWM Output #%d\n", m_PwmDevId);
		return false;
	}
	return true;
}

bool PwmOutput::stop() {
	if (PwmOutputError_Ok != PwmOut_stop(m_PwmDevId)) {
		fprintf(stderr, "Can't stop PWM Output #%d\n", m_PwmDevId);
		return false;
	}
	return true;
}

bool PwmOutput::setPeriod(uint16_t period) {
	if (PwmOutputError_Ok != PwmOut_setPeriod(m_PwmDevId, period)) {
		fprintf(stderr, "Can't set PWM Output #%d Period\n", m_PwmDevId);
		return false;
	}
	return true;
}

bool PwmOutput::setDuty(uint16_t duty) {
	if (PwmOutputError_Ok != PwmOut_setDuty(m_PwmDevId, duty)) {
		fprintf(stderr, "Can't set PWM Output #%d Duty\n", m_PwmDevId);
		return false;
	}
	return true;
}

LightSystem::LightSystem() :
	common::Singleton<LightSystem>(this),
	m_IntLgtPwm(PWM_INTERIOR_LIGHT),
	m_BrkLgtPwm(PWM_BRAKE_LIGHT),
	m_BckLgtPwm(PWM_BACKWARDS_LIGHT),
	m_LHzckLgtPwm(PWM_LEFT_HAZARD_LIGHT),
	m_RHzckLgtPwm(PWM_RIGHT_HAZARD_LIGHT)
{
	fprintf(stderr, "LightSystem::LightSystem()\n");
}

LightSystem::~LightSystem() {
	fprintf(stderr, "~LightSystem::LightSystem()\n");
}

void LightSystem::init() {
	fprintf(stderr, "LightSystem::init()\n");
	m_IntLgtPwm.init();
	m_IntLgtPwm.setPeriod(255);
	m_BrkLgtPwm.init();
	m_BrkLgtPwm.setPeriod(255);
	m_BckLgtPwm.init();
	m_BckLgtPwm.setPeriod(255);
	m_LHzckLgtPwm.init();
	m_LHzckLgtPwm.setPeriod(255);
	m_RHzckLgtPwm.init();
	m_RHzckLgtPwm.setPeriod(255);
}

void LightSystem::shutdown() {
	fprintf(stderr, "LightSystem::shutdown()\n");
}
