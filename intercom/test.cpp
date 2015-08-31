#include "intercom.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <tinythread.h>

void do_send(void * arg) {
	intercom::Sender sender(3);
	while (true) {
		sleep(1);

		intercom::DataMessage msg;

		msg.createTextMsg("Hello, World!");
		sender.send(msg);

		sleep(1);

		const uint8_t can_data[] = { 1, 2, 3, 4, 5, 6, 7, 8 };
		msg.createCanMsg(0x100, sizeof(can_data), can_data);
		sender.send(msg);

		sleep(1);

		intercom::DataMessage::PwmSignal pwm_signal('Miry', 300, 10000);
		msg.createPwmMsg(pwm_signal);
		sender.send(msg);

		sleep(1);

		intercom::DataMessage::AdcSignal adc_signal('Miry', 31337);
		msg.createAdcMsg(adc_signal);
		sender.send(msg);
	}
}

void do_receive(void * arg) {
	intercom::Receiver receiver(4);
	while (true) {
		intercom::DataMessage msg;
		receiver.receive(msg);
		fputs("Msg Rcv: ", stderr);
		msg.fprint(stderr);
		fputs("\n", stderr);
	}
}

int main(int argc, char *argv[]) {
	tthread::thread rcv_thread(do_receive, NULL);
	do_send(NULL);
	rcv_thread.join();
	return EXIT_SUCCESS;
}
