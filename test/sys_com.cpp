#include "sys_com.h"
#include "os.h"
#include "sys_cfg.h"
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

SysComHandler::SysComHandler() :
	stop(false),
	receiver(intercom::Sys_Ecu),
	rcv_thread(receiveThreadFunc, this),
	snd_thread(sendThreadFunc, this)
{
}

SysComHandler::~SysComHandler() {
	stop = true;
	intercom::Sender sender(intercom::Sys_Ecu);
	intercom::DataMessage msg;
	msg.createTextMsg("Bye!");
	sender.send(msg);
	receiver.shutdown();
	snd_thread.join();
	rcv_thread.join();
}

void SysComHandler::receiveLoop() {
	//intercom::Receiver receiver(intercom::Sys_Ecu);
	while (!stop) {
		intercom::DataMessage msg;
		receiver.receive(msg);
		bool ignore = false;

		if (receiver.getSysId() == msg.getSysId()) {
			//fputs("* Msg Ign: ", stderr); msg.fprint(stderr); fputs("\n", stderr);
			ignore = true;
		} else {
			//fputs("* Msg Rcv: ", stderr); msg.fprint(stderr); fputs("\n", stderr);
			ignore = false;
		}

		switch (msg.getMsgType()) {
			case intercom::DataMessage::MsgCan:
				if (!ignore) {
					intercom::DataMessage::CanMsg * can_msg = msg.getCanInfo();
					if ((NULL != can_msg) && (can_msg->Bus >= 0)) {
					}
				}
				break;
			default:
				break;
		}
	}
}

void SysComHandler::sendLoop() {
	intercom::Sender sender(intercom::Sys_Ecu);
	intercom::DataMessage msg;

	sleep(1);
	msg.createTextMsg("Hello!");
	sender.send(msg);

	while (!stop) {
		sleep(1);
	}
}

void SysComHandler::receiveThreadFunc(void * arg) {
	SysComHandler * obj = reinterpret_cast<SysComHandler *>(arg);
	if (NULL != obj) {
		obj->receiveLoop();
	}
}

void SysComHandler::sendThreadFunc(void * arg) {
	SysComHandler * obj = reinterpret_cast<SysComHandler *>(arg);
	if (NULL != obj) {
		obj->sendLoop();
	}
}

