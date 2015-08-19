#include "CarState.h"
#include "intercom.h"

#include "busAssignment.h"
#include "signalFormat.h"

#include <string.h>

/* CAN message type */
typedef struct {
	struct {
		time_t tv_sec;
		uint32 tv_nsec;
	} t;         /* time stamp */
	uint8   bus; /* can bus */
	uint32  id;  /* numeric CAN-ID */
	uint8   dlc;
	uint8   byte_arr[8];
} canMessage_t;

class CanMsgParser {
public:
	CanMsgParser(const char * dbc_filename);
	virtual ~CanMsgParser();

	void processCanMessage(intercom::DataMessage::CanMsg * can_msg);
	void decodeCanMessage(message_t * dbc_msg, intercom::DataMessage::CanMsg * can_msg);
	void processCanSignal(const signal_t * sgn, message_t * dbc_msg, intercom::DataMessage::CanMsg * can_msg, uint32 raw_value, double phys_value);

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
			if (NULL != can_msg) {
				car_msg_parser->processCanMessage(can_msg);
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

void CanMsgParser::processCanMessage(intercom::DataMessage::CanMsg * can_msg) {
	/* lookup can_msg in message hash */
	messageHashKey_t key = ntohl(can_msg->Id);
	message_t * dbc_msg = NULL;
	int i;

	/* loop over all bus assigments */
	for(i = 0; i < busAssignment->n ; i++) {
		busAssignmentEntry_t * entry = &busAssignment->list[i];

		/* check if bus matches */
		if((entry->bus == -1) || (entry->bus == can_msg->Bus)) {
			dbc_msg = (message_t *)hashtable_search(entry->messageHash, &key);
			if(NULL != dbc_msg) {
				decodeCanMessage(dbc_msg, can_msg);
				break; /* end search if message was found */
			}
		}
	}
}

void CanMsgParser::processCanSignal(const signal_t * sgn, message_t * dbc_msg, intercom::DataMessage::CanMsg * can_msg, uint32 raw_value, double phys_value) {
	fprintf(stderr, "   %s.%s = %f (raw=%ld): %d|%d@%d%c (%f + raw * %f) [%f|%f] %d %ul \"%s\"\n",
		dbc_msg->name,
		sgn->name,
		phys_value,
		raw_value,
		sgn->bit_start,
		sgn->bit_len,
		sgn->endianess,
		sgn->signedness ? '-' : '+',
		sgn->offset,
		sgn->scale,
		sgn->min,
		sgn->max,
		sgn->mux_type,
		(unsigned int)sgn->mux_value,
		sgn->comment !=NULL ? sgn->comment : ""
	);
}

void CanMsgParser::decodeCanMessage(message_t * dbc_msg, intercom::DataMessage::CanMsg * can_msg) {
	signal_list_t * sl;

	/* iterate over all signals */
	for(sl = dbc_msg->signal_list; sl != NULL; sl = sl->next) {
		/*
		 * The "raw value" of a signal is the value as it is transmitted
		 * over the network.
		 */
		uint32 rawValue   = 0;

		/*
		 * compute some signal properties
		 *
		 * signal bit order:
		 *
		 *     7  6  5  4  3  2  1  0 offset
		 *    bit
		 * 0   7  6  5  4  3  2  1  0
		 * 1  15 14 13 12 11 10  9  8
		 * 2  23 22 21 20 19 18 17 16
		 * 3  31 30 29 28 27 26 25 24
		 * 4  39 38 37 36 35 34 33 32
		 * 5  47 46 45 44 43 42 41 40
		 * 6  55 54 53 52 51 50 49 48
		 * 7  63 62 61 60 59 58 57 56
		 * |
		 * start_byte
		 *
		 * big endian place value exponent
		 *                15 14 13 12   <- start_byte
		 *    11 10  9  8  7  6  5  4
		 *     3  2  1  0               <- end_byte
		 *
		 * little endian place value exponent
		 *     3  2  1  0               <- start_byte
		 *    11 10  9  8  7  6  5  4
		 *                15 14 13 12   <- end_byte
		 */
		const signal_t *const s = sl->signal;
		uint8  bit_len          = s->bit_len;
		uint8  start_offset     = s->bit_start & 7;
		uint8  start_byte       = s->bit_start / 8;
		uint8  data;
		uint8  shift;

		/* align signal into ulong32 */
		/* 0 = Big Endian, 1 = Little Endian */
		if(s->endianess == 0) { /* big endian */
			uint8  end_byte     = start_byte + (7 + bit_len - start_offset - 1)/8;
			uint8  end_offset   = (start_offset - bit_len + 1) & 7;

			/* loop over all source bytes from start_byte to end_byte */
			for (int work_byte = start_byte; work_byte <= end_byte; work_byte++) {
				/* fetch source byte */
				data = can_msg->Payload[work_byte];

				/* process source byte */
				if (work_byte == start_byte && start_offset != 7) {
					/* less that 8 bits in start byte? mask out unused bits */
					data &= (uint8)~0 >> (7 - start_offset);
					shift = start_offset + 1;
				} else {
					shift = 8; /* use all eight bits */
				}

				if (work_byte == end_byte && end_offset != 0) {
					/* less that 8 bits in end byte? shift out unused bits */
					data >>= end_offset;
					shift -= end_offset;
				}

				/* store processed byte */
				rawValue <<= shift; /* make room for shift bits */
				rawValue |= data; /* insert new bits at low position */
			}
		} else {
			/* little endian - similar algorithm with reverse bit significance  */
			uint8  end_byte     = start_byte + (bit_len + start_offset - 1)/8;
			uint8  end_offset   = (start_offset + bit_len - 1) & 7;

			for (int work_byte = end_byte; work_byte >= start_byte; work_byte--) {
				data = can_msg->Payload[work_byte];
				if(work_byte == end_byte && end_offset != 7) {
					data &= (uint8)~0 >> (7 - end_offset);
					shift = end_offset + 1;
				} else {
					shift = 8;
				}

				if (work_byte == start_byte && start_offset != 0) {
					data >>= start_offset;
					shift -= start_offset;
				}
				rawValue <<= shift;
				rawValue |= data;
			}
		}

		double physicalValue;

		/* perform sign extension */
		if(s->signedness && (bit_len < 32)) {
			sint32 m = 1<< (bit_len-1);
			rawValue = ((sint32)rawValue ^ m) - m;
		}

		/*
		 * Factor, Offset and Physical Unit
		 *
		 * The "physical value" of a signal is the value of the physical
		 * quantity (e.g. speed, rpm, temperature, etc.) that represents
		 * the signal.
		 * The signal's conversion formula (Factor, Offset) is used to
		 * transform the raw value to a physical value or in the reverse
		 * direction.
		 * [Physical value] = ( [Raw value] * [Factor] ) + [Offset]
		 */
		if(s->signedness) {
			physicalValue = (double)(sint32)rawValue * s->scale + s->offset;
		} else {
			physicalValue = (double)        rawValue * s->scale + s->offset;
		}

		processCanSignal(s, dbc_msg, can_msg, rawValue, physicalValue);
	}
}
