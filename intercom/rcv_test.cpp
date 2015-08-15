#include "intercom.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
	intercom::Receiver receiver;
	while (true) {
		intercom::DataMessage msg;
		receiver.receive(msg);
	}
	return EXIT_SUCCESS;
}
