#include "os.h"
#include "CanSystem.h"

#include "common/Delegate.h"
#include "common/LinkedList.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

using namespace common;

ISR(CAN_MSG_RECV) {
//	common::Delegate<int, int> d = common::Delegate<int, int>::from_function<A, &A::foo>(&a);
	printf("CAN_MSG_RECV (%u)\n", CanSystem_getIsrCurrentDevId());
	CanMessage * can_msg = CanDriver_getRxMessage(CanSystem_getIsrCurrentDevId());
	if (can_msg) {
		Singleton<CanSystem>::getInstance().dispatchCanMessage(CanSystem_getIsrCurrentDevId(), can_msg);
		CanDriver_delRxMessage(CanSystem_getIsrCurrentDevId());
	}
}

ISR(CAN_MSG_SENT) {
	//printf("CAN_MSG_SENT (%u)\n", CanSystem_getIsrCurrentDevId());
}
