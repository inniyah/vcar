#include "intercom.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
	intercom::Receiver receiver(2);
	while (true) {
		intercom::DataMessage msg;
		receiver.receive(msg);
		fputs("Msg Rcv: ", stderr);
		msg.fprint(stderr);
		fputs("\n", stderr);
	}
	return EXIT_SUCCESS;
}
