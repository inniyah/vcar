#include "emu_can.h"

#include "tinythread.h"
#include "intercom.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <assert.h>
#include <sys/time.h>

ISR(CAN_MSG_RECV);
ISR(CAN_MSG_SENT);

CanDevice CanDevice::s_CanDevices[CanDevice::NUM_CAN_DEVICES];
CanDevId CanDevice::s_CurrentDevId = INVALID_CAN_DEVICE;

CanDevice::CanDevice() :
	m_CanTransceiverState(Tr_Undefined),
	m_CanDriverState(Drv_Undefined),
	m_CanTxPos(0),
	m_CanTxCnt(0),
	m_CanRxPos(0),
	m_CanRxCnt(0)
{
}

CanDevice::~CanDevice() {
}

CanDevId CanDevice::getDevId() const {
	CanDevId dev_id = (this - s_CanDevices) / sizeof(s_CanDevices[0]);
	if (dev_id >= 0 && dev_id < NUM_CAN_DEVICES) {
		return dev_id;
	} else {
		return INVALID_CAN_DEVICE;
	}
}

bool CanDevice::insertTxMessage(CanMsgId msg_id, uint8_t dlc, uint8_t * payload) {
	if ((dlc > CAN_MAX_SIZE) || (NULL == payload)) {
		return false;
	}
	m_Mutex.lock();
	if ((Tr_Active != m_CanTransceiverState)) {
		m_Mutex.unlock();
		fprintf(stderr, "Can Transceiver Not Active\n");
		return false;
	}
	if (m_CanTxCnt >= NUM_CAN_TXBUFFERS) {
		m_Mutex.unlock();
		fprintf(stderr, "Can Tx Buffer Full\n");
		return false;
	}
	int pos = ((m_CanTxPos + m_CanTxCnt) % NUM_CAN_TXBUFFERS);
	m_CanTxMessages[pos].Id = msg_id;
	m_CanTxMessages[pos].Dlc = dlc;
	memcpy(m_CanTxMessages[pos].Payload, payload, dlc);
	++m_CanTxCnt;
	m_Mutex.unlock();
	// Trigger Interruption
	m_IsrMutex.lock();
	s_CurrentDevId = getDevId();
	isr_CAN_MSG_SENT(0, 0, NULL);
	s_CurrentDevId = INVALID_CAN_DEVICE;
	m_IsrMutex.unlock();
	return true;
}

bool CanDevice::insertRxMessage(CanMsgId msg_id, uint8_t dlc, uint8_t * payload) {
	if ((dlc > CAN_MAX_SIZE) || (NULL == payload)) {
		return false;
	}
	m_Mutex.lock();
	if ((Tr_Active != m_CanTransceiverState)) {
		m_Mutex.unlock();
		fprintf(stderr, "Can Transceiver Not Active\n");
		return false;
	}
	if (m_CanRxCnt >= NUM_CAN_RXBUFFERS) {
		m_Mutex.unlock();
		fprintf(stderr, "Can Rx Buffer Full\n");
		return false;
	}
	int pos = ((m_CanRxPos + m_CanRxCnt) % NUM_CAN_RXBUFFERS);
	m_CanRxMessages[pos].Id = msg_id;
	m_CanRxMessages[pos].Dlc = dlc;
	memcpy(m_CanRxMessages[pos].Payload, payload, dlc);
	++m_CanRxCnt;
	m_Mutex.unlock();
	// Trigger Interruption
	m_IsrMutex.lock();
	s_CurrentDevId = getDevId();
	isr_CAN_MSG_RECV(0, 0, NULL);
	s_CurrentDevId = INVALID_CAN_DEVICE;
	m_IsrMutex.unlock();
	return true;
}

void CanDevice::resetAllDevices() {
	for (int can_id = 0; can_id < CanDevice::NUM_CAN_DEVICES; ++can_id) {
		CanDevice::s_CanDevices[can_id].m_CanTransceiverState = CanDevice::Tr_Undefined;
		CanDevice::s_CanDevices[can_id].m_CanTxPos = 0;
		CanDevice::s_CanDevices[can_id].m_CanTxCnt = 0;
		CanDevice::s_CanDevices[can_id].m_CanRxPos = 0;
		CanDevice::s_CanDevices[can_id].m_CanRxCnt = 0;
	}
}

// Public API

extern "C" uint8_t CanSystem_getNumberOfDevices() {
	return CanDevice::NUM_CAN_DEVICES;
}

extern "C" CanDevId CanSystem_getIsrCurrentDevId() {
	return CanDevice::s_CurrentDevId;
}

extern "C" CanTransceiverStatus CanTransceiver_getStatus(CanDevId can_id) {
	if ((can_id < 0) || (can_id >= CanDevice::NUM_CAN_DEVICES)) {
		return CanTransceiverStatus_Undefined;
	}
	switch (CanDevice::s_CanDevices[can_id].m_CanTransceiverState) {
		case CanDevice::Tr_Standby:
			return CanTransceiverStatus_Standby;
		case CanDevice::Tr_Active:
			return CanTransceiverStatus_Active;
		default:
			return CanTransceiverStatus_Undefined;
	}
}

extern "C" CanTransceiverError  CanTransceiver_init(CanDevId can_id) {
	if ((can_id < 0) || (can_id >= CanDevice::NUM_CAN_DEVICES)) {
		return CanTransceiverError_WrongDevice;
	}
	CanDevice::s_CanDevices[can_id].m_CanTransceiverState = CanDevice::Tr_Standby;
	return CanTransceiverError_Ok;
}

extern "C" CanTransceiverError  CanTransceiver_start(CanDevId can_id) {
	if ((can_id < 0) || (can_id >= CanDevice::NUM_CAN_DEVICES)) {
		return CanTransceiverError_WrongDevice;
	}
	if (CanDevice::s_CanDevices[can_id].m_CanTransceiverState != CanDevice::Tr_Standby) {
		return CanTransceiverError_IllegalState;
	}
	CanDevice::s_CanDevices[can_id].m_CanTransceiverState = CanDevice::Tr_Active;
	return CanTransceiverError_Ok;
}

extern "C" CanTransceiverError  CanTransceiver_stop(CanDevId can_id) {
	if ((can_id < 0) || (can_id >= CanDevice::NUM_CAN_DEVICES)) {
		return CanTransceiverError_WrongDevice;
	}
	if (CanDevice::s_CanDevices[can_id].m_CanTransceiverState != CanDevice::Tr_Active) {
		return CanTransceiverError_IllegalState;
	}
	CanDevice::s_CanDevices[can_id].m_CanTransceiverState = CanDevice::Tr_Standby;
	return CanTransceiverError_Ok;
}

extern "C" CanDriverStatus CanDriver_getStatus(CanDevId can_id) {
	if ((can_id < 0) || (can_id >= CanDevice::NUM_CAN_DEVICES)) {
		return CanDriverStatus_Undefined;
	}
	switch (CanDevice::s_CanDevices[can_id].m_CanDriverState) {
		case CanDevice::Drv_Closed:
			return CanDriverStatus_Close;
		case CanDevice::Drv_Silent:
			return CanDriverStatus_Silent;
		case CanDevice::Drv_Open:
			return CanDriverStatus_Open;
		default:
			return CanDriverStatus_Undefined;
	}
}

extern "C" CanDriverError CanDriver_init(CanDevId can_id) {
	if ((can_id < 0) || (can_id >= CanDevice::NUM_CAN_DEVICES)) {
		return CanDriverError_WrongDevice;
	}
	CanDevice::s_CanDevices[can_id].m_CanDriverState = CanDevice::Drv_Closed;
	return CanDriverError_Ok;
}

extern "C" CanDriverError CanDriver_open(CanDevId can_id) {
	if ((can_id < 0) || (can_id >= CanDevice::NUM_CAN_DEVICES)) {
		return CanDriverError_WrongDevice;
	}
	if (CanDevice::s_CanDevices[can_id].m_CanDriverState == CanDevice::Drv_Undefined) {
		return CanDriverError_IllegalState;
	}
	CanDevice::s_CanDevices[can_id].m_CanDriverState = CanDevice::Drv_Open;
	return CanDriverError_Ok;
}

extern "C" CanDriverError CanDriver_close(CanDevId can_id) {
	if ((can_id < 0) || (can_id >= CanDevice::NUM_CAN_DEVICES)) {
		return CanDriverError_WrongDevice;
	}
	if (CanDevice::s_CanDevices[can_id].m_CanDriverState == CanDevice::Drv_Undefined) {
		return CanDriverError_IllegalState;
	}
	CanDevice::s_CanDevices[can_id].m_CanDriverState = CanDevice::Drv_Closed;
	return CanDriverError_Ok;
}

extern "C" CanDriverError CanDriver_silent(CanDevId can_id) {
	if ((can_id < 0) || (can_id >= CanDevice::NUM_CAN_DEVICES)) {
		return CanDriverError_WrongDevice;
	}
	if (CanDevice::s_CanDevices[can_id].m_CanDriverState == CanDevice::Drv_Undefined) {
		return CanDriverError_IllegalState;
	}
	CanDevice::s_CanDevices[can_id].m_CanDriverState = CanDevice::Drv_Silent;
	return CanDriverError_Ok;
}

extern "C" CanMessage * CanDriver_getRxMessage(CanDevId can_id) {
	if ((can_id < 0) || (can_id >= CanDevice::NUM_CAN_DEVICES)) {
		return NULL;
	}
	if (0 == CanDevice::s_CanDevices[can_id].m_CanRxCnt) {
		return NULL;
	}
	return & CanDevice::s_CanDevices[can_id].m_CanRxMessages[CanDevice::s_CanDevices[can_id].m_CanRxPos];
}

extern "C" CanDriverError CanDriver_delRxMessage(CanDevId can_id) {
	if ((can_id < 0) || (can_id >= CanDevice::NUM_CAN_DEVICES)) {
		return CanDriverError_WrongDevice;
	}
	if (0 == CanDevice::s_CanDevices[can_id].m_CanRxCnt) {
		return CanDriverError_RxEmptyQueue;
	}
	CanDevice::s_CanDevices[can_id].m_CanRxPos = ((CanDevice::s_CanDevices[can_id].m_CanRxPos + 1) % CanDevice::NUM_CAN_RXBUFFERS);
	CanDevice::s_CanDevices[can_id].m_CanRxCnt--;
	return CanDriverError_Ok;
}