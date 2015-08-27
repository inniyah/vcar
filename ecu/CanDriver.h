#ifndef CAN_DRIVER_H_8AE696F4_4C11_11E5_95F5_10FEED04CD1C
#define CAN_DRIVER_H_8AE696F4_4C11_11E5_95F5_10FEED04CD1C

#include "os.h"
#include "AbstractCanMsgHandler.h"
#include "ICanDriver.h"

class CanDriver : public ICanDriver {
public:
	CanDriver(CanDevId id, AbstractCanTxMsgHandler * tx_msgs, AbstractCanRxMsgHandler * rx_msgs);
	~CanDriver();

	virtual void processMessage(CanMessage * can_msg);

private:
	CanDevId m_CanDevId;
	AbstractCanTxMsgHandler * m_pCanTxMsgs;
	AbstractCanRxMsgHandler * m_pCanRxMsgs;
};

#endif // CAN_DRIVER_H_8AE696F4_4C11_11E5_95F5_10FEED04CD1C
