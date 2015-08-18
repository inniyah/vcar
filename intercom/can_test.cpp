#include "intercom.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "messageDecoder.h"
#include "busAssignment.h"
#include "signalFormat.h"

static void printSignalProc( /* for debugging: print data to stderr */
		const signal_t * s,
		double dtime,
		uint32 rawValue,
		double physicalValue,
		void * cbData
) {
	const char * local_prefix = "a";
	char * outputSignalName = signalFormat_stringAppend(local_prefix, s->name);

	fprintf(stderr,
			"   %s\t=%f ~ raw=%ld\t~ %d|%d@%d%c (%f,%f)"
			" [%f|%f] %d %ul \"%s\"\n",
		outputSignalName,
		physicalValue,
		rawValue,
		s->bit_start,
		s->bit_len,
		s->endianess,
		s->signedness?'-':'+',
		s->scale,
		s->offset,
		s->min,
		s->max,
		s->mux_type,
		(unsigned int)s->mux_value,
		s->comment !=NULL ? s->comment : ""
	);

	/* free temp. signal name */
	if(outputSignalName != NULL) {
		free(outputSignalName);
	}
}

static void processCanMessage(canMessage_t * can_msg, busAssignment_t * bus_assignment, signalFormat_t signalFormat, sint32 timeResolution) {
	/* lookup can_msg in message hash */
	messageHashKey_t key = can_msg->id;
	message_t * dbc_msg;
	int i;

	/* loop over all bus assigments */
	for(i = 0; i < bus_assignment->n ; i++) {
		busAssignmentEntry_t * entry = &bus_assignment->list[i];

		/* check if bus matches */
		if((entry->bus == -1) || (entry->bus == can_msg->bus)) {
			dbc_msg = (message_t *)hashtable_search(entry->messageHash, &key);
			if(NULL != dbc_msg) {

				/* found the message in the database */
				char * local_prefix;
				const char * const prefix = NULL;

				/* setup and forward message prefix */
				if (signalFormat & signalFormat_Message) {
					local_prefix = signalFormat_stringAppend(prefix, dbc_msg->name);
				} else {
					if (NULL != prefix) {
						local_prefix = strdup(prefix);
					} else {
						local_prefix = NULL;
					}
				}

				canMessage_decode(
					dbc_msg,
					can_msg,
					timeResolution,
					printSignalProc,
					NULL
				);

				/* free local prefix */
				if(local_prefix != NULL) {
					free(local_prefix);
				}

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
	sint32 timeResolution = 10000;
	int bus = -1;

	busAssignment_associate(busAssignment, bus, dbc_filename);
	busAssignment_parseDBC(busAssignment);

	canMessage_t can_msg = {
		.t = { 0, 0 },
		.bus = 1,
		.id = 0x100,
		.dlc = 6,
		.byte_arr = { 1, 2, 3, 4, 5, 6, 7, 8 }
	};

	processCanMessage(&can_msg, busAssignment, signalFormat, timeResolution);

	//	while (true) {
	//		intercom::DataMessage msg;
	//		receiver.receive(msg);
	//	}

	busAssignment_free(busAssignment);
	return EXIT_SUCCESS;
}
