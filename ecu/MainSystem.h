#ifndef MAIN_SYSTEM_H_8AE693A2_4C11_11E5_95E1_10FEED04CD1C
#define MAIN_SYSTEM_H_8AE693A2_4C11_11E5_95E1_10FEED04CD1C

#include "common/Singleton.h"
#include "common/LinkedList.h"

#include "AbstractCanMsgHandler.h"
#include "ISwc.h"

namespace CanBus_can01 {
	struct TxMsgs;
	struct RxMsgs;
}

struct Rte;
class CanDriver;

class MainSystem : public common::Singleton<MainSystem> {
public:
	MainSystem();
	~MainSystem();
	void init();
	void shutdown();

	AbstractCanTxMsgHandler * getCanTxMsgHandler(CanDevId can_id);
	AbstractCanRxMsgHandler * getCanRxMsgHandler(CanDevId can_id);

	inline void insertSwc(ISwc & swc) {
		m_SwcList.push_back(swc);
	}

	inline void removeSwc(ISwc & swc) {
		m_SwcList.remove(swc);
	}

	void runSwcCyclic();

	void printCanBusRxSignals(CanDevId can_id);

	void swcCall(void (ISwc::*method)(Rte &));
	template <void (ISwc::*TMethod)(Rte &)> void swcCall();

private:
	static const int NUM_CAN_BUSES = 1;
	AbstractCanTxMsgHandler  * m_pCanTxMsgs[NUM_CAN_BUSES];
	AbstractCanRxMsgHandler  * m_pCanRxMsgs[NUM_CAN_BUSES];
	common::LinkedList<ISwc>   m_SwcList;

	void processCanMessage_can01(CanMessage * can_msg);

	static MainSystem sys;
	static Rte rte;
	static CanBus_can01::TxMsgs can01_tx;
	static CanBus_can01::RxMsgs can01_rx;
};

#endif // MAIN_SYSTEM_H_8AE693A2_4C11_11E5_95E1_10FEED04CD1C
