#include "intercom.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "messageDecoder.h"
#include "busAssignment.h"
#include "signalFormat.h"

static void printSignalProc(
		message_t * dbc_msg,
		canMessage_t * can_msg,
		const signal_t * s,
		uint32 rawValue,
		double physicalValue,
		void * cbData
) {
	fprintf(stderr,
			"   %s.%s = %f (raw=%ld): %d|%d@%d%c (%f + raw * %f) [%f|%f] %d %ul \"%s\"\n",
		dbc_msg->name,
		s->name,
		physicalValue,
		rawValue,
		s->bit_start,
		s->bit_len,
		s->endianess,
		s->signedness?'-':'+',
		s->offset,
		s->scale,
		s->min,
		s->max,
		s->mux_type,
		(unsigned int)s->mux_value,
		s->comment !=NULL ? s->comment : ""
	);
}

static void processCanMessage(canMessage_t * can_msg, busAssignment_t * bus_assignment, signalFormat_t signalFormat) {
	/* lookup can_msg in message hash */
	messageHashKey_t key = can_msg->id;
	message_t * dbc_msg = NULL;
	int i;

	/* loop over all bus assigments */
	for(i = 0; i < bus_assignment->n ; i++) {
		busAssignmentEntry_t * entry = &bus_assignment->list[i];

		/* check if bus matches */
		if((entry->bus == -1) || (entry->bus == can_msg->bus)) {
			dbc_msg = (message_t *)hashtable_search(entry->messageHash, &key);
			if(NULL != dbc_msg) {

				canMessage_decode(
					dbc_msg,
					can_msg,
					printSignalProc,
					NULL
				);

				/* end search if message was found */
				break;
			}
		}
	}
}

int main(int argc, char *argv[]) {
	intercom::Receiver receiver;

	const char * dbc_filename = "../dbc/can01.dbc";

	busAssignment_t * busAssignment = busAssignment_create();
	signalFormat_t signalFormat = signalFormat_Name;
	int bus = -1;

	busAssignment_associate(busAssignment, bus, dbc_filename);
	busAssignment_parseDBC(busAssignment);

	canMessage_t can_msg_1 = {
		.t = { 0, 0 },
		.bus = 1,
		.id = 0x100,
		.dlc = 6,
		.byte_arr = { 1, 2, 3, 4, 5, 6, 7, 8 }
	};

	canMessage_t can_msg_2 = {
		.t = { 0, 0 },
		.bus = 1,
		.id = 0x110,
		.dlc = 4,
		.byte_arr = { 1, 2, 3, 4, 5, 6, 7, 8 }
	};

	processCanMessage(&can_msg_1, busAssignment, signalFormat);
	processCanMessage(&can_msg_2, busAssignment, signalFormat);

	//	while (true) {
	//		intercom::DataMessage msg;
	//		receiver.receive(msg);
	//	}

	busAssignment_free(busAssignment);
	return EXIT_SUCCESS;
}
