#include "intercom.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
	intercom::Sender sender;
	while (true) {
		intercom::DataMessage msg;
		msg.createTextMsg("Hello, World!");
		sender.send(msg);
		sleep(1);
	}
	return EXIT_SUCCESS;
}
