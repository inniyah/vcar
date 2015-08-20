#include "CarState.h"
#include "intercom.h"

#include "busAssignment.h"
#include "signalFormat.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

class CanMsgParser {
public:
	CanMsgParser(const char * dbc_filename);
	virtual ~CanMsgParser();

	void processCanMessage(
		const intercom::DataMessage::CanMsg * can_msg
	);
	void decodeCanMessage(
		const message_t * dbc_msg,
		const intercom::DataMessage::CanMsg * can_msg
	);
	void processCanSignal(
		const signal_t * sgn,
		const message_t * dbc_msg,
		const intercom::DataMessage::CanMsg * can_msg,
		uint32 raw_value,
		double phys_value
	);

	void requestCanMessage(
		intercom::DataMessage::CanMsg * can_msg
	);

	void encodeCanMessage(
		const message_t * dbc_msg,
		intercom::DataMessage::CanMsg * can_msg
	);

	uint32 requestCanSignal(
		const signal_t * sgn,
		const message_t * dbc_msg
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
		//sender.send(msg);

		msg.createCanMsg(0x110);
		intercom::DataMessage::CanMsg * can_msg = msg.getCanInfo();
		if (NULL != can_msg) {
			car_msg_parser->requestCanMessage(can_msg);
			sender.send(msg);
		}
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


static void print_bits(int size, const void * const ptr) {
	const unsigned char * b = reinterpret_cast<const unsigned char*>(ptr);
	unsigned char byte;

	printf("< ");
	for (int i = 0; i < size; ++i) {
		for (int j = 7; j >= 0; --j) {
			byte = b[i] & (1<<j);
			byte >>= j;
			printf("%u", byte);
		}
		printf(" ");
	}
		printf("> ");
}


void CanMsgParser::processCanMessage(const intercom::DataMessage::CanMsg * can_msg) {
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

void CanMsgParser::processCanSignal(const signal_t * sgn, const message_t * dbc_msg, const intercom::DataMessage::CanMsg * can_msg, uint32 raw_value, double phys_value) {
#if 1
	fprintf(stderr, "   %s.%s = %f (raw=%ld=0x%lX): %d|%d@%d%c (%f + raw * %f) [%f|%f] %d %ul \"%s\"\n",
		dbc_msg->name,
		sgn->name,
		phys_value,
		raw_value,
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
#endif
}

/*  CanMsgParser::decodeCanMessage --  decode CAN messages
	Copyright (C) 2007-2009 Andreas Heitmann

	This program is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

void CanMsgParser::decodeCanMessage(const message_t * dbc_msg, const intercom::DataMessage::CanMsg * can_msg) {
	if (!dbc_msg || !can_msg) {
		return;
	}

	printf("IN:  "); print_bits(can_msg->Dlc, can_msg->Payload); puts("\n");

	/* iterate over all signals */
	for(signal_list_t * sitem = dbc_msg->signal_list; sitem != NULL; sitem = sitem->next) {
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

		const signal_t *const sgn = sitem->signal;
		uint8  bit_len            = sgn->bit_len;
		uint8  start_offset       = sgn->bit_start & 7;
		uint8  start_byte         = sgn->bit_start / 8;
		uint8  data;
		uint8  shift;

		/* align signal into ulong32 */

		if(sgn->endianess == 0) { /* 0 = Big Endian */
			/*
			 *     7  6  5  4  3  2  1  0      offset
			 *
			 * big endian place value exponent
			 *                   15 14 13   <- start_byte
			 *    12 11 10  9  8  7  6  5 
			 *     4  3  2  1  0            <- end_byte
			 */

			uint8  end_byte     = start_byte + (7 + bit_len - start_offset - 1)/8;
			uint8  end_offset   = (start_offset - bit_len + 1) & 7;

			/* loop over all source bytes from start_byte to end_byte */
			for (int work_byte = start_byte; work_byte <= end_byte; ++work_byte) {
				/* fetch source byte */
				data = can_msg->Payload[work_byte];

				printf("{%u}: %u:%u -> %u:%u -> 0x%02X -> ", work_byte, start_byte, start_offset, end_byte, end_offset, data);

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

				printf("(<<%d) + 0x%02X\n", shift, data);

				rawValue <<= shift; /* make room for shift bits */
				rawValue |= data; /* insert new bits at low position */
			}
		} else { /* 1 = Little Endian */
			/*
			 *     7  6  5  4  3  2  1  0      offset
			 *
			 * little endian place value exponent
			 *     4  3  2  1  0            <- start_byte
			 *    12 11 10  9  8  7  6  5 
			 *                   15 14 13   <- end_byte
			 */

			uint8  end_byte     = start_byte + (bit_len + start_offset - 1)/8;
			uint8  end_offset   = (start_offset + bit_len - 1) & 7;

			for (int work_byte = end_byte; work_byte >= start_byte; --work_byte) {
				data = can_msg->Payload[work_byte];

				printf("[%u]: %u:%u -> %u:%u -> 0x%02X -> ", work_byte, start_byte, start_offset, end_byte, end_offset, data);

				if (work_byte == end_byte && end_offset != 7) {
					data &= (uint8)~0 >> (7 - end_offset);
					shift = end_offset + 1;
				} else {
					shift = 8;
				}

				if (work_byte == start_byte && start_offset != 0) {
					data >>= start_offset;
					shift -= start_offset;
				}

				printf("(<<%d) + 0x%02X\n", shift, data);

				rawValue <<= shift;
				rawValue |= data;
			}
		}

		double physicalValue;

		/* perform sign extension */
		if(sgn->signedness && (bit_len < 32)) {
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
		if (sgn->signedness) {
			physicalValue = (double)(sint32)rawValue * sgn->scale + sgn->offset;
		} else {
			physicalValue = (double)        rawValue * sgn->scale + sgn->offset;
		}

		processCanSignal(sgn, dbc_msg, can_msg, rawValue, physicalValue);
	}
}

void CanMsgParser::requestCanMessage(intercom::DataMessage::CanMsg * can_msg) {
	/* lookup can_msg in message hash */
	messageHashKey_t key = ntohl(can_msg->Id);
	message_t * dbc_msg = NULL;
	int i;

	/* loop over all bus assigments */
	for(i = 0; i < busAssignment->n ; i++) {
		busAssignmentEntry_t * entry = &busAssignment->list[i];

		/* check if bus matches */
		if ((entry->bus == -1) || (entry->bus == can_msg->Bus)) {
			dbc_msg = (message_t *)hashtable_search(entry->messageHash, &key);
			if(NULL != dbc_msg) {
				encodeCanMessage(dbc_msg, can_msg);
				break; /* end search if message was found */
			}
		}
	}
}

uint32 CanMsgParser::requestCanSignal(const signal_t * sgn, const message_t * dbc_msg) {
#if 1
	int bits = sgn->bit_len;
	uint32 raw = 0x8197 & (0xFFFFFFFFFFFFFFFFll >> (64 - bits)); // 10000001 10010111
	fprintf(stderr, "   req %s.%s -> raw=%ld=0x%lX: %d|%d@%d%c (%f + raw * %f) [%f|%f] %d %ul \"%s\"\n",
		dbc_msg->name,
		sgn->name,
		raw,
		raw,
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
#endif
	return raw;
}

void CanMsgParser::encodeCanMessage(const message_t * dbc_msg, intercom::DataMessage::CanMsg * can_msg) {
	if (!dbc_msg || !can_msg) {
		return;
	}

	can_msg->Dlc = dbc_msg->len;
	print_bits(can_msg->Dlc, can_msg->Payload); puts("\n");

		/*
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
		 */

	for(signal_list_t * sitem = dbc_msg->signal_list; sitem != NULL; sitem = sitem->next) {

		const signal_t *const sgn = sitem->signal;
		uint8  bit_len            = sgn->bit_len;
		uint8  start_offset       = sgn->bit_start & 7;
		uint8  start_byte         = sgn->bit_start / 8;
		uint8  data;
		uint8  mask;
		uint8  shift;

		uint32 rawValue   = requestCanSignal(sgn, dbc_msg);

		/* perform sign extension */
		if (sgn->signedness && (bit_len < 32)) {
			sint32 m = 1<< (bit_len-1);
			rawValue = ((sint32)rawValue + m) ^ m;
		}

		if(sgn->endianess == 0) { /* 0 = Big Endian */
		/*
		 *     7  6  5  4  3  2  1  0      offset
		 *
		 * big endian place value exponent
		 *                   15 14 13   <- start_byte
		 *    12 11 10  9  8  7  6  5 
		 *     4  3  2  1  0            <- end_byte
		 */

			uint8  end_byte     = start_byte + (7 + bit_len - start_offset - 1)/8;
			uint8  end_offset   = (start_offset - bit_len + 1) & 7;

			printf("%u:%u -> %u:%u ->", start_byte, start_offset, end_byte, end_offset);

			for (int work_byte = end_byte; work_byte >= start_byte; --work_byte) {
				if (work_byte == end_byte && end_offset != 7) {
					mask  =  0xFF ^ ((uint8)~0 >> (9 - end_offset));
					data  = (rawValue << (7 - end_offset - 2)) & mask;
					shift = 9 - end_offset;
				} else if (work_byte == start_byte && start_offset != 0) {
					mask  = (uint8)~0 >> (8 - start_offset);
					data  = rawValue & mask;
					shift = start_offset;
				} else {
					mask  = 0xFF;
					data  = rawValue & mask;
					shift = 8;
				}

				printf(" {%d|%lX|%X|%X|%d}", work_byte, rawValue, data, mask, shift);

				can_msg->Payload[work_byte] = ( (data & mask) | (can_msg->Payload[work_byte] & (~mask)) );
				rawValue >>= shift;
			}

			printf(" {%lX}\n", rawValue);

		} else { /* 1 = Little Endian */
		/*
		 *     7  6  5  4  3  2  1  0      offset
		 *
		 * little endian place value exponent
		 *     4  3  2  1  0            <- start_byte
		 *    12 11 10  9  8  7  6  5 
		 *                   15 14 13   <- end_byte
		 */

			uint8  end_byte     = start_byte + (bit_len + start_offset - 1)/8;
			uint8  end_offset   = (start_offset + bit_len - 1) & 7;

			printf("%u:%u -> %u:%u ->", start_byte, start_offset, end_byte, end_offset);

			for (int work_byte = start_byte; work_byte <= end_byte; ++work_byte) {
				if (work_byte == end_byte && end_offset != 7) {
					shift = end_offset + 1;
					mask  = (uint8)~0 >> (8 - shift);
					data  = (rawValue & mask);
				} else if (work_byte == start_byte && start_offset != 0) {
					shift = 8 - start_offset;
					mask  = 0xFF ^ ((uint8)~0 >> (shift));
					data  = (rawValue << (start_offset)) & mask;
				} else {
					shift = 8;
					mask  = 0xFF;
					data  = rawValue & mask;
				}

				printf(" [%d|%lX|%X|%X|%d]", work_byte, rawValue, data, mask, shift);

				can_msg->Payload[work_byte] = ( (data & mask) | (can_msg->Payload[work_byte] & (~mask)) );
				rawValue >>= shift;
			}

			printf(" [%lX]\n", rawValue);

		}
		printf("OUT: "); print_bits(can_msg->Dlc, can_msg->Payload); puts("\n");
	}
}
