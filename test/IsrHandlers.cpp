#include "os.h"
#include "BusManager.h"

#include "common/Delegate.h"
#include "common/LinkedList.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

struct CanMessageDelegate : public common::Delegate<void, const CanMessage &> {
	typedef common::Delegate<void, const CanMessage &> BaseDelegate;
	inline CanMessageDelegate() : BaseDelegate(BaseDelegate::fromObjectMethod<CanMessageDelegate,&CanMessageDelegate::EmptyEventReceiver>(this)) {
	}
	inline CanMessageDelegate(const BaseDelegate & delegate) : BaseDelegate(delegate) {
	}
	inline void clean() {
		BaseDelegate::operator==(BaseDelegate::fromObjectMethod<CanMessageDelegate,&CanMessageDelegate::EmptyEventReceiver>(this));
	}
	void EmptyEventReceiver(const CanMessage &) {
		fprintf(stderr, "CanMessageDelegate::EmptyEventReceiver\n");
	}
};

CanMessageDelegate can_message_handler[NUMBER_OF_CAN_DEVICES];

ISR(CAN_MSG_RECV) {
	printf("CAN_MSG_RECV\n");
//	CanMessage * can_msg = CanDriver_getRxMessage(CanSystem_getIsrCurrentDevId());
//	if (can_msg) {
//		Singleton<BusManager>::getInstance().dispatchCanMessage(CanSystem_getIsrCurrentDevId(), can_msg);
//		CanDriver_delRxMessage(CanSystem_getIsrCurrentDevId());
//	}
}

ISR(CAN_MSG_SENT) {
	printf("CAN_MSG_SENT\n");
}
