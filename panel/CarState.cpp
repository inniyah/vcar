#include "CarState.h"
#include "intercom.h"

#include "messageDecoder.h"
#include "busAssignment.h"
#include "signalFormat.h"

#include <string.h>

class CanMsgParser {
public:
	CanMsgParser(const char * dbc_filename);
	virtual ~CanMsgParser();

	void processCanMessage(canMessage_t * can_msg);

	static void printSignalProc(
		message_t * dbc_msg,
		canMessage_t * can_msg,
		const signal_t * s,
		uint32 rawValue,
		double physicalValue,
		void * cbData
	);

	busAssignment_t * busAssignment;
	signalFormat_t signalFormat;
};

// CarState

CarState::CarState() :
	stop(false),
	rcv_thread(receiveThreadFunc, this),
	snd_thread(sendThreadFunc, this),
	car_msg_parser(NULL)
{
	car_msg_parser = new CanMsgParser("../dbc/can01.dbc");
}

CarState::~CarState() {
	stop = true;
	intercom::Sender sender(intercom::Sys_Panel);
	intercom::DataMessage msg;
	msg.createTextMsg("Bye!");
	sender.send(msg);
	snd_thread.join();
	rcv_thread.join();

	if (car_msg_parser) {
		delete car_msg_parser;
		car_msg_parser = NULL;
	}
}

void CarState::receiveLoop() {
	intercom::Receiver receiver(intercom::Sys_Panel);
	while (!stop) {
		intercom::DataMessage msg;
		receiver.receive(msg);

		if ((NULL != car_msg_parser) && (intercom::DataMessage::MsgCan == msg.getMsgType())) {
			intercom::DataMessage::CanMsg * can_msg = msg.getCanInfo();
			if ((NULL != can_msg) && (can_msg->Dlc < sizeof(canMessage_t::byte_arr)) ) {
				canMessage_t cmsg = {
					.t = { 0, 0 },
					.bus = 1,
					.id = ntohl(can_msg->Id),
					.dlc = can_msg->Dlc,
					.byte_arr = { 1, 2, 3, 4, 5, 6, 7, 8 }
				};
				memcpy(cmsg.byte_arr, can_msg->Payload, can_msg->Dlc);
				car_msg_parser->processCanMessage(&cmsg);
			}
		}
	}
}

void CarState::sendLoop() {
	intercom::Sender sender(intercom::Sys_Panel);
	intercom::DataMessage msg;

	sleep(1);
	msg.createTextMsg("Hello!");
	sender.send(msg);

	while (!stop) {
		sleep(1);
		const uint8_t can_data[] = { 1, 2, 3, 4, 5, 6 };
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

// CanMsgParser

CanMsgParser::CanMsgParser(const char * dbc_filename) {
	busAssignment = busAssignment_create();
	signalFormat = signalFormat_Name;
	int bus = -1;

	busAssignment_associate(busAssignment, bus, dbc_filename);
	busAssignment_parseDBC(busAssignment);
}

CanMsgParser::~CanMsgParser() {
	busAssignment_free(busAssignment);
}

void CanMsgParser::printSignalProc(
		message_t * dbc_msg,
		canMessage_t * can_msg,
		const signal_t * s,
		uint32 rawValue,
		double physicalValue,
		void * cbData
) {
	CanMsgParser * that = reinterpret_cast<CanMsgParser *>(cbData);
	fprintf(stderr, "   %s.%s = %f (raw=%ld): %d|%d@%d%c (%f + raw * %f) [%f|%f] %d %ul \"%s\"\n",
		dbc_msg->name,
		s->name,
		physicalValue,
		rawValue,
		s->bit_start,
		s->bit_len,
		s->endianess,
		s->signedness?'-':'+',
		s->offset,
		s->scale,
		s->min,
		s->max,
		s->mux_type,
		(unsigned int)s->mux_value,
		s->comment !=NULL ? s->comment : ""
	);
}

void CanMsgParser::processCanMessage(canMessage_t * can_msg) {
	/* lookup can_msg in message hash */
	messageHashKey_t key = can_msg->id;
	message_t * dbc_msg = NULL;
	int i;

	/* loop over all bus assigments */
	for(i = 0; i < busAssignment->n ; i++) {
		busAssignmentEntry_t * entry = &busAssignment->list[i];

		/* check if bus matches */
		if((entry->bus == -1) || (entry->bus == can_msg->bus)) {
			dbc_msg = (message_t *)hashtable_search(entry->messageHash, &key);
			if(NULL != dbc_msg) {
				canMessage_decode(dbc_msg, can_msg, printSignalProc, this );
				break; /* end search if message was found */
			}
		}
	}
}
