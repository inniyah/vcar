#ifndef BSP_SYSTEM_H_8AE696F4_4C11_11E5_95F5_10FEED04CD1C
#define BSP_SYSTEM_H_8AE696F4_4C11_11E5_95F5_10FEED04CD1C

#include "os.h"
#include "common/Singleton.h"
#include "common/Delegate.h"

class BspSystem : public common::Singleton<BspSystem> {
public:
	BspSystem();
	~BspSystem();
	void init();
	void shutdown();

	typedef common::Delegate<void, CanMessage *> CanMessageDelegate;
	bool addCanReceiveDelegate(CanDevId dev_id, CanMessageDelegate delegate);
	bool removeCanReceiveDelegate(CanDevId dev_id, CanMessageDelegate delegate);
	void dispatchCanMessage(CanDevId dev_id, CanMessage * can_msg);

private:
	static BspSystem instance;

	static const int MAX_CAN_DELEGATES = 4;
	int m_NumCanRcvDelegates[NUMBER_OF_CAN_DEVICES];
	CanMessageDelegate m_CanRcvDelegates[MAX_CAN_DELEGATES][NUMBER_OF_CAN_DEVICES];
};

#endif // BSP_SYSTEM_H_8AE696F4_4C11_11E5_95F5_10FEED04CD1C
