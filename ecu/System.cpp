#include "os.h"
#include "System.h"
#include "rte.h"

#include "can01.dbc.h"
#include "CanDriver.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

static System sys;
static Rte rte;

static CanBus_can01::TxMsgs can01_tx;
static CanBus_can01::RxMsgs can01_rx;
static CanDriver            can01_drv(0, &can01_tx, &can01_rx);

System::System() : common::Singleton<System>(this) {
	fprintf(stderr, "System::System()\n");
	for (int i = 0; i < NUM_CAN_BUSES; ++i) {
		m_pCanTxMsgs[i] = NULL;
		m_pCanRxMsgs[i] = NULL;
		m_pCanDriver[i] = NULL;
	}
	m_pCanTxMsgs[0] = &can01_tx;
	m_pCanRxMsgs[0] = &can01_rx;
	m_pCanDriver[0] = &can01_drv;
	rte.init();
}

System::~System() {
	fprintf(stderr, "~System::System()\n");
}

AbstractCanTxMsgHandler * System::getCanTxMsgHandler(CanDevId can_id) {
	if ((can_id >= 0) && (can_id < NUM_CAN_BUSES)) {
		return m_pCanTxMsgs[can_id];
	} else {
		return NULL;
	}
}

AbstractCanRxMsgHandler * System::getCanRxMsgHandler(CanDevId can_id) {
	if ((can_id >= 0) && (can_id < NUM_CAN_BUSES)) {
		return m_pCanRxMsgs[can_id];
	} else {
		return NULL;
	}
}

ICanDriver * System::getCanDriver(CanDevId can_id) {
	if ((can_id >= 0) && (can_id < NUM_CAN_BUSES)) {
		return m_pCanDriver[can_id];
	} else {
		return NULL;
	}
}

void System::printCanBusRxSignals(CanDevId can_id) {
	switch (can_id) {
		case 0:
			printf("Acc: %d, Brake: %d, Wheel: %d, Gear: %d\n",
				can01_rx.Msg_driving_controls.getSignal_acceleration(),
				can01_rx.Msg_driving_controls.getSignal_brake(),
				can01_rx.Msg_driving_controls.getSignal_wheel(),
				can01_rx.Msg_driving_controls.getSignal_gear()
			);
		default:
			break;
	}
}

void System::updateSwc() {
	rte.readInputs(can01_rx);
	rte.writeOutputs(can01_tx);
}
