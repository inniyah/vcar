#include "BspSystem.h"
#include "os.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

BspSystem BspSystem::instance;

BspSystem::BspSystem() : common::Singleton<BspSystem>(this) {
	fprintf(stderr, "BspSystem::BspSystem()\n");
}

BspSystem::~BspSystem() {
	fprintf(stderr, "~BspSystem::BspSystem()\n");
}

void BspSystem::init() {
	fprintf(stderr, "BspSystem::init()\n");

	if (CanTransceiverError_Ok != CanTransceiver_init(0)) {
		fprintf(stderr, "Can't init CAN Transceiver #0\n");
	}
	if (CanTransceiverError_Ok != CanTransceiver_start(0)) {
		fprintf(stderr, "Can't start CAN Transceiver #0\n");
	}

	if (PwmOutputError_Ok != PwmOut_init(PWM_INTERIOR_LIGHT)) {
		fprintf(stderr, "Can't init PWM Output #%d\n", PWM_INTERIOR_LIGHT);
	}

	if (PwmOutputError_Ok != PwmOut_setPeriod(PWM_INTERIOR_LIGHT, 255)) {
		fprintf(stderr, "Can't set PWM Output #%d Period\n", PWM_INTERIOR_LIGHT);
	}

	if (PwmOutputError_Ok != PwmOut_setDuty(PWM_INTERIOR_LIGHT, 255)) {
		fprintf(stderr, "Can't set PWM Output #%d Duty\n", PWM_INTERIOR_LIGHT);
	}
}

void BspSystem::shutdown() {
	fprintf(stderr, "BspSystem::shutdown()\n");
}

void BspSystem::addCanReceiveDelegate(CanDevId dev_id, common::DelegateListNode<CanMessage *> delegate) {
	m_CanRcvDelegates[dev_id].insert(delegate);
}

void BspSystem::removeCanReceiveDelegate(CanDevId dev_id, common::DelegateListNode<CanMessage *> delegate) {
	m_CanRcvDelegates[dev_id].remove(delegate);
}

void BspSystem::dispatchCanMessage(CanDevId dev_id, CanMessage * can_msg) {
	m_CanRcvDelegates[dev_id](can_msg);
}
