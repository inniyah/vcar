#include "CanDriver.h"

#include <cstring>

CanDriver::CanDriver(CanDevId id, AbstractCanTxMsgHandler * tx_msgs, AbstractCanRxMsgHandler * rx_msgs) :
		m_CanDevId(id), m_pCanTxMsgs(tx_msgs), m_pCanRxMsgs(rx_msgs)
{
}

CanDriver::~CanDriver() {
}

void CanDriver::processMessage(CanMessage * can_msg) {
	if (can_msg) {
		printf("  0x%04X %d\n", can_msg->Id, can_msg->Dlc);
		if (m_pCanRxMsgs) {
			uint8_t * buff = m_pCanRxMsgs->getCanMessageBufferWithDlt(can_msg->Id, can_msg->Dlc);
			if (buff) {
				printf("  0x%04X %d -> %p\n", can_msg->Id, can_msg->Dlc, buff);
				memcpy(buff, can_msg->Payload, can_msg->Dlc);
			}
		}
	}
}
