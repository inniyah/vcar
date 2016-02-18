#ifndef COM_SYSTEM_H_8AE693A2_4C11_11E5_95E1_10FEED04CD1C
#define COM_SYSTEM_H_8AE693A2_4C11_11E5_95E1_10FEED04CD1C

#include "common/Singleton.h"
#include "common/LinkedList.h"

#include "AbstractCanMsgHandler.h"

namespace CanBus_can01 {
	struct TxMsgs;
	struct RxMsgs;
}

class CanDriver;

class ComSystem : public common::Singleton<ComSystem> {
public:
	ComSystem();
	~ComSystem();
	void init();
	void shutdown();

	AbstractCanTxMsgHandler * getCanTxMsgHandler(CanDevId can_id);
	AbstractCanRxMsgHandler * getCanRxMsgHandler(CanDevId can_id);

	void printCanBusRxSignals(CanDevId can_id);

private:
	static const int NUM_CAN_BUSES = 1;
	AbstractCanTxMsgHandler * m_pCanTxMsgs[NUM_CAN_BUSES];
	AbstractCanRxMsgHandler * m_pCanRxMsgs[NUM_CAN_BUSES];

	void processCanMessage_can01(CanMessage * can_msg);

	static ComSystem sys; // Make sure that the object instance is created

	static CanBus_can01::TxMsgs can01_tx;
	static CanBus_can01::RxMsgs can01_rx;
};

#endif // COM_SYSTEM_H_8AE693A2_4C11_11E5_95E1_10FEED04CD1C
