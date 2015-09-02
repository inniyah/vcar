#include "CanSystem.h"
#include "os.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

CanSystem CanSystem::instance;

CanSystem::CanSystem() : common::Singleton<CanSystem>(this) {
	fprintf(stderr, "CanSystem::CanSystem()\n");
}

CanSystem::~CanSystem() {
	fprintf(stderr, "~CanSystem::CanSystem()\n");
}

void CanSystem::init() {
	fprintf(stderr, "CanSystem::init()\n");

	if (CanTransceiverError_Ok != CanTransceiver_init(0)) {
		fprintf(stderr, "Can't init CAN Transceiver #0\n");
	}
	if (CanTransceiverError_Ok != CanTransceiver_start(0)) {
		fprintf(stderr, "Can't start CAN Transceiver #0\n");
	}
}

void CanSystem::shutdown() {
	fprintf(stderr, "CanSystem::shutdown()\n");
}

bool CanSystem::addCanReceiveDelegate(CanDevId dev_id, CanMessageDelegate delegate) {
	if (m_NumCanRcvDelegates[dev_id] < MAX_CAN_DELEGATES) {
		m_CanRcvDelegates[m_NumCanRcvDelegates[dev_id]][dev_id] = delegate;
		++m_NumCanRcvDelegates[dev_id];
		return true;
	} else {
		return false;
	}
}

bool CanSystem::removeCanReceiveDelegate(CanDevId dev_id, CanMessageDelegate delegate) {
	for (int i = 0; i < m_NumCanRcvDelegates[dev_id]; ++i) {
		if (m_CanRcvDelegates[i][dev_id] == delegate) {
			for (int j = i + 1; j < m_NumCanRcvDelegates[dev_id]; ++j) {
				m_CanRcvDelegates[j-1][dev_id] = m_CanRcvDelegates[j][dev_id];
			}
			--m_NumCanRcvDelegates[dev_id];
			return true;
		}
	}
	return false;
}

void CanSystem::dispatchCanMessage(CanDevId dev_id, CanMessage * can_msg) {
	for (int i = 0; i < m_NumCanRcvDelegates[dev_id]; ++i) {
		m_CanRcvDelegates[i][dev_id].operator()(can_msg);
	}

}
