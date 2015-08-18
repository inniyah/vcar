#include "CarState.h"
#include "intercom.h"

CarState::CarState() :
	stop(false),
	rcv_thread(receiveThreadFunc, this),
	snd_thread(sendThreadFunc, this)
{
}

CarState::~CarState() {
	stop = true;
	intercom::Sender sender(1);
	intercom::DataMessage msg;
	msg.createTextMsg("Bye!");
	sender.send(msg);
	snd_thread.join();
	rcv_thread.join();
}

void CarState::receiveLoop() {
	intercom::Receiver receiver(2);
	while (!stop) {
		intercom::DataMessage msg;
		receiver.receive(msg);
	}
}

void CarState::sendLoop() {
	intercom::Sender sender(1);
	intercom::DataMessage msg;

	sleep(1);
	msg.createTextMsg("Hello!");
	sender.send(msg);

	while (!stop) {
		sleep(1);
		const uint8_t can_data[] = { 1, 2, 3, 4, 5, 6, 7, 8 };
		msg.createCanMsg(0x100, sizeof(can_data), can_data);
		sender.send(msg);
	}
}

void CarState::receiveThreadFunc(void * arg) {
	CarState * car_state = reinterpret_cast<CarState *>(arg);
	if (NULL != car_state) {
		car_state->receiveLoop();
	}
}

void CarState::sendThreadFunc(void * arg) {
	CarState * car_state = reinterpret_cast<CarState *>(arg);
	if (NULL != car_state) {
		car_state->sendLoop();
	}
}
