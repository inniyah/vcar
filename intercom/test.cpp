#include "intercom.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <tinythread.h>

void do_send(void * arg) {
	intercom::Sender sender;
	while (true) {
		intercom::DataMessage msg;
		msg.createTextMsg("Hello, World!");
		sender.send(msg);
		sleep(1);
	}
}

void do_receive(void * arg) {
	intercom::Receiver receiver;
	while (true) {
		intercom::DataMessage msg;
		receiver.receive(msg);
	}
}

int main(int argc, char *argv[]) {
	tthread::thread rcv_thread(do_receive, NULL);
	do_send(NULL);
	rcv_thread.join();
	return EXIT_SUCCESS;
}
