#include "intercom.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "measurement.h"
#include "busAssignment.h"
#include "signalFormat.h"

int main(int argc, char *argv[]) {
	intercom::Receiver receiver;

	const char * dbc_filename = "../dbc/can01.dbc";

	busAssignment_t * busAssignment = busAssignment_create();
	signalFormat_t signalFormat = signalFormat_Name;
	measurement_t * measurement;
	sint32 timeResolution = 10000;
	int bus = -1;

	busAssignment_associate(busAssignment, bus, dbc_filename);

	//	while (true) {
	//		intercom::DataMessage msg;
	//		receiver.receive(msg);
	//	}
	return EXIT_SUCCESS;
}
