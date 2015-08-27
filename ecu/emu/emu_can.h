#ifndef EMU_CAN_H_8AE69370_4C11_11E5_95E0_10FEED04CD1C
#define EMU_CAN_H_8AE69370_4C11_11E5_95E0_10FEED04CD1C

#include "../os.h"
#include "emu_ecu.h"
#include "emu_can.h"

#include "fast_mutex.h"

struct CanDevice {
	CanDevice();
	virtual ~CanDevice();

	bool insertTxMessage(CanMsgId msg_id, uint8_t dlc, uint8_t * payload);
	bool insertRxMessage(CanMsgId msg_id, uint8_t dlc, uint8_t * payload);

	static const int NUM_CAN_DEVICES = 1;
	static const int NUM_CAN_TXBUFFERS = 8;
	static const int NUM_CAN_RXBUFFERS = 8;

	typedef enum CanTransceiverStateE {
		Tr_Undefined,
		Tr_Standby,
		Tr_Active,
	} CanTransceiverState;

	static CanDevice m_CanDevices[CanDevice::NUM_CAN_DEVICES];
	static CanDevId CurrentDevId;

	CanDevId getDevId() const;

	CanTransceiverState m_CanTransceiverState;

	CanMessage m_CanTxMessages[NUM_CAN_TXBUFFERS];
	int m_CanTxPos;
	int m_CanTxCnt;

	CanMessage m_CanRxMessages[NUM_CAN_RXBUFFERS];
	int m_CanRxPos;
	int m_CanRxCnt;

	tthread::fast_mutex m_Mutex;
	tthread::fast_mutex m_IsrMutex;
};

#endif // EMU_CAN_H_8AE69370_4C11_11E5_95E0_10FEED04CD1C