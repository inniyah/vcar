#ifndef CAN_SYSTEM_H_8AE696F4_4C11_11E5_95F5_10FEED04CD1C
#define CAN_SYSTEM_H_8AE696F4_4C11_11E5_95F5_10FEED04CD1C

#include "os.h"
#include "common/Singleton.h"
#include "common/Delegate.h"

class CanSystem : public common::Singleton<CanSystem> {
public:
	CanSystem();
	~CanSystem();
	void init();
	void shutdown();

	struct CanMessageDelegate : public common::Delegate<void, CanMessage *> {
		typedef common::Delegate<void, CanMessage *> BaseDelegate;
		inline CanMessageDelegate() : BaseDelegate(BaseDelegate::fromObjectMethod<CanMessageDelegate,&CanMessageDelegate::EmptyEventReceiver>(this)) {
		}
		inline CanMessageDelegate(const BaseDelegate & delegate) : BaseDelegate(delegate) {
		}
		inline void clean() {
			BaseDelegate::operator==(BaseDelegate::fromObjectMethod<CanMessageDelegate,&CanMessageDelegate::EmptyEventReceiver>(this));
		}
		void EmptyEventReceiver(CanMessage *);
	};

	bool addCanReceiveDelegate(CanDevId dev_id, CanMessageDelegate delegate);
	bool removeCanReceiveDelegate(CanDevId dev_id, CanMessageDelegate delegate);
	void dispatchCanMessage(CanDevId dev_id, CanMessage * can_msg);


private:
	static CanSystem instance;

	static const int MAX_CAN_DELEGATES = 4;
	int m_NumCanRcvDelegates[NUMBER_OF_CAN_DEVICES];
	CanMessageDelegate m_CanRcvDelegates[MAX_CAN_DELEGATES][NUMBER_OF_CAN_DEVICES];
};

#endif // CAN_SYSTEM_H_8AE696F4_4C11_11E5_95F5_10FEED04CD1C
