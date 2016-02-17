#include "os.h"

#include "common/Delegate.h"
#include "common/LinkedList.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

using namespace common;

ISR(CAN_MSG_RECV) {
	printf("CAN_MSG_RECV\n");
}

ISR(CAN_MSG_SENT) {
	printf("CAN_MSG_SENT\n");
}
