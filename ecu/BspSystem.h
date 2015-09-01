#ifndef BSP_SYSTEM_H_8AE696F4_4C11_11E5_95F5_10FEED04CD1C
#define BSP_SYSTEM_H_8AE696F4_4C11_11E5_95F5_10FEED04CD1C

#include "os.h"
#include "common/Singleton.h"
#include "common/DelegateList.h"

class BspSystem : public common::Singleton<BspSystem> {
public:
	BspSystem();
	~BspSystem();

	void dispatchCanMessage(CanDevId dev_id, CanMessage * can_msg);

private:
	static BspSystem instance;

	common::DelegateList<void, CanMessage *> m_CanReceivers[NUMBER_OF_CAN_DEVICES];
};

#endif // BSP_SYSTEM_H_8AE696F4_4C11_11E5_95F5_10FEED04CD1C
