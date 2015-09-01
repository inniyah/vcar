#include "os.h"
#include "MainSystem.h"
#include "rte.h"

#include "can01.dbc.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

MainSystem MainSystem::sys;
Rte MainSystem::rte;
CanBus_can01::TxMsgs MainSystem::can01_tx;
CanBus_can01::RxMsgs MainSystem::can01_rx;

MainSystem::MainSystem() : common::Singleton<MainSystem>(this) {
	fprintf(stderr, "MainSystem::MainSystem()\n");
	for (int i = 0; i < NUM_CAN_BUSES; ++i) {
		m_pCanTxMsgs[i] = NULL;
		m_pCanRxMsgs[i] = NULL;
	}
	m_pCanTxMsgs[0] = &can01_tx;
	m_pCanRxMsgs[0] = &can01_rx;
	rte.init();
}

MainSystem::~MainSystem() {
	fprintf(stderr, "~MainSystem::MainSystem()\n");
}

AbstractCanTxMsgHandler * MainSystem::getCanTxMsgHandler(CanDevId can_id) {
	if ((can_id >= 0) && (can_id < NUM_CAN_BUSES)) {
		return m_pCanTxMsgs[can_id];
	} else {
		return NULL;
	}
}

AbstractCanRxMsgHandler * MainSystem::getCanRxMsgHandler(CanDevId can_id) {
	if ((can_id >= 0) && (can_id < NUM_CAN_BUSES)) {
		return m_pCanRxMsgs[can_id];
	} else {
		return NULL;
	}
}

void MainSystem::printCanBusRxSignals(CanDevId can_id) {
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

void MainSystem::updateSwc() {
	rte.readInputs(can01_rx);
	rte.writeOutputs(can01_tx);
}

void MainSystem::processCanMessage_can01(CanMessage * can_msg) {
	if (can_msg) {
		printf("  0x%04X %d\n", can_msg->Id, can_msg->Dlc);
		uint8_t * buff = can01_rx.getCanMessageBufferWithDlt(can_msg->Id, can_msg->Dlc);
		if (buff) {
			printf("  0x%04X %d -> %p\n", can_msg->Id, can_msg->Dlc, buff);
			memcpy(buff, can_msg->Payload, can_msg->Dlc);
		}
	}
}
