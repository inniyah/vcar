#ifndef SYSTEM_H_8AE693A2_4C11_11E5_95E1_10FEED04CD1C
#define SYSTEM_H_8AE693A2_4C11_11E5_95E1_10FEED04CD1C

#include "common/Singleton.h"
#include "common/LinkedList.h"

#include "AbstractCanMsgHandler.h"
#include "ICanDriver.h"
#include "ISwc.h"

class System : public common::Singleton<System> {
public:
	System();
	~System();

	AbstractCanTxMsgHandler * getCanTxMsgHandler(CanDevId can_id);
	AbstractCanRxMsgHandler * getCanRxMsgHandler(CanDevId can_id);
	ICanDriver              * getCanDriver(CanDevId can_id);

	inline void insertSwc(ISwc & swc) {
		m_SwcList.push_back(swc);
	}

	inline void removeSwc(ISwc & swc) {
		m_SwcList.remove(swc);
	}

	void updateSwc();

	void printCanBusRxSignals(CanDevId can_id);

private:
	static const int NUM_CAN_BUSES = 1;
	AbstractCanTxMsgHandler  * m_pCanTxMsgs[NUM_CAN_BUSES];
	AbstractCanRxMsgHandler  * m_pCanRxMsgs[NUM_CAN_BUSES];
	ICanDriver               * m_pCanDriver[NUM_CAN_BUSES];
	common::LinkedList<ISwc>   m_SwcList;
};

#endif // SYSTEM_H_8AE693A2_4C11_11E5_95E1_10FEED04CD1C
