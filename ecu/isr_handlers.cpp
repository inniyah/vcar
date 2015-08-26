#include "os.h"
#include "com.h"

#include "common/delegate.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

ISR(CAN_MSG_RECV) {
//	common::Delegate<int, int> d = common::Delegate<int, int>::from_function<A, &A::foo>(&a);
	printf("CAN_MSG_RECV (%u)\n", CanSystem_getIsrCurrentDevId());
	CanMessage * can_msg = CanDriver_getRxMessage(CanSystem_getIsrCurrentDevId());
	if (can_msg) {
		printf("  0x%04X %d\n", can_msg->Id, can_msg->Dlc);
		CanDriver_delRxMessage(CanSystem_getIsrCurrentDevId());
	}
}

ISR(CAN_MSG_SENT) {
	//printf("CAN_MSG_SENT (%u)\n", CanSystem_getIsrCurrentDevId());
}
