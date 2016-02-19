#include "os.h"
#include "BusManager.h"

#include "common/Delegate.h"
#include "common/LinkedList.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

using namespace common;

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
