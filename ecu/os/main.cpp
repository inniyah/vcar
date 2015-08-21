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

// Periodic Alarms

class PeriodicAlarm {
public:
	PeriodicAlarm(void (*function)(void), unsigned long period_ms, unsigned long delay_ms = 0lu) :
		m_Function(function),
		m_Period_ms(period_ms),
		m_Delay_ms(delay_ms),
		m_Thread(loop, this)
	{
	}

	~PeriodicAlarm() {
		m_Thread.join();
	}

private:
	void (*m_Function)(void);
	unsigned long m_Period_ms;
	unsigned long m_Delay_ms;
	tthread::thread m_Thread;

	static void loop(void * arg);
};

void PeriodicAlarm::loop(void * arg) {
	PeriodicAlarm * alarm = reinterpret_cast<PeriodicAlarm*>(arg);
	assert(alarm != NULL);

	if (alarm->m_Delay_ms != 0) {
		usleep(1000lu * alarm->m_Delay_ms);
	}

	unsigned long time_us = 1000lu * alarm->m_Period_ms;
	unsigned long long start_us = 0LLU;
	struct timeval tv;
	if( gettimeofday(&tv, NULL) == 0 ) {
		start_us = (tv.tv_sec * 1000000llu) + (tv.tv_usec);
	}

	while (true) {
		alarm->m_Function();
		if( gettimeofday(&tv, NULL) == 0 ) {
			unsigned long long current_us = (tv.tv_sec * 1000000llu) + (tv.tv_usec);
			unsigned long elapsed_us = (current_us - start_us);
			if (elapsed_us < time_us) {
				usleep(time_us - (current_us - start_us) );
				start_us += time_us;
			} else {
				start_us += time_us * (elapsed_us / time_us);
			}
		}
	}
}

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
	task_init();

	PeriodicAlarm periodic_alarm_100ms (&task_100ms, 100);
	PeriodicAlarm periodic_alarm_10ms  (&task_10ms,   10);
	CanBusHandler can_bus_handler;

	struct event ev;
	while (!isExitRequested()) {
		setEventAfterMs(ev, 3000, heartbeat, NULL);
		event_dispatch();
	}

	printf("</end>\n");

	return EXIT_SUCCESS;
}
