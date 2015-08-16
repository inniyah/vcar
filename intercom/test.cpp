#include "intercom.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <tinythread.h>

void do_send(void * arg) {
	intercom::Sender sender;
	while (true) {
		sleep(1);

		intercom::DataMessage msg;

		msg.createTextMsg("Hello, World!");
		sender.send(msg);

		const uint8_t can_data[] = { 1, 2, 3, 4, 5, 6, 7, 8 };
		msg.createCanMsg(can_data);
		sender.send(msg);
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
