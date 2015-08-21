#include "os.h"
#include "can.h"

#include "tinythread.h"
#include "intercom.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <unistd.h>
#include <errno.h>
#include <assert.h>
#include <event.h>

// Can Bus Handler

class CanBusHandler {
public:
	CanBusHandler();
	virtual ~CanBusHandler();

private:
	bool stop;

	void receiveLoop();
	void sendLoop();

	intercom::Receiver receiver;

	static void receiveThreadFunc(void * arg);
	static void sendThreadFunc(void * arg);

	tthread::thread rcv_thread;
	tthread::thread snd_thread;
};

CanBusHandler::CanBusHandler() :
	stop(false),
	receiver(intercom::Sys_Ecu),
	rcv_thread(receiveThreadFunc, this),
	snd_thread(sendThreadFunc, this)
{
}

CanBusHandler::~CanBusHandler() {
	stop = true;
	intercom::Sender sender(intercom::Sys_Ecu);
	intercom::DataMessage msg;
	msg.createTextMsg("Bye!");
	sender.send(msg);
	receiver.shutdown();
	snd_thread.join();
	rcv_thread.join();
}

void CanBusHandler::receiveLoop() {
	//intercom::Receiver receiver(intercom::Sys_Ecu);
	while (!stop) {
		intercom::DataMessage msg;
		receiver.receive(msg);
		bool ignore = false;

		if (receiver.getSysId() == msg.getSysId()) {
			fputs("* Msg Ign: ", stderr); msg.fprint(stderr); fputs("\n", stderr);
			ignore = true;
		} else {
			fputs("* Msg Rcv: ", stderr); msg.fprint(stderr); fputs("\n", stderr);
			ignore = false;
		}

		switch (msg.getMsgType()) {
			case intercom::DataMessage::MsgCan:
				if (!ignore) {
					intercom::DataMessage::CanMsg * can_msg = msg.getCanInfo();
					if ((NULL != can_msg) && (can_msg->Bus >= 0) && (can_msg->Bus < CanDevice::NUM_CAN_DEVICES)) {
						CanDevice::m_CanDevices[can_msg->Bus].insertTxMessage(can_msg->Id, can_msg->Dlc, can_msg->Payload);
					}
				}
				break;
			default:
				break;
		}
	}
}

void CanBusHandler::sendLoop() {
	intercom::Sender sender(intercom::Sys_Ecu);
	intercom::DataMessage msg;

	sleep(1);
	msg.createTextMsg("Hello!");
	sender.send(msg);

	while (!stop) {
		sleep(1);
	}
}

void CanBusHandler::receiveThreadFunc(void * arg) {
	CanBusHandler * obj = reinterpret_cast<CanBusHandler *>(arg);
	if (NULL != obj) {
		obj->receiveLoop();
	}
}

void CanBusHandler::sendThreadFunc(void * arg) {
	CanBusHandler * obj = reinterpret_cast<CanBusHandler *>(arg);
	if (NULL != obj) {
		obj->sendLoop();
	}
}

// Main Function

void heartbeat(int fd, short event, void *arg) {
  printf("Heart Beat!\n");
}

int main(int argc, const char * argv[]) {
	event_init();
	task_init(0, 0, NULL);

	CanBusHandler can_bus_handler;

	struct event ev_100ms;
	addEventEveryMs(ev_100ms, 100, task_100ms, NULL);

	struct event ev_10ms;
	addEventEveryMs(ev_10ms,   10, task_10ms, NULL);

	struct event ev_hb;
	addEventEverySec(ev_hb,     1, heartbeat, NULL);

	while (!isExitRequested()) {
		event_dispatch();
	}

	printf("</end>\n");

	return EXIT_SUCCESS;
}
