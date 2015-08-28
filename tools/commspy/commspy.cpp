#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>

#include "tinythread.h"
#include "intercom.h"
#include "busAssignment.h"
#include "signalFormat.h"

#include <cstdlib>
#include <cstdio>
#include <string>
#include <iostream>
#include <map>
#include <string>
#include <algorithm>
#include <iomanip>
#include <vector>
#include <map>

struct AnalogValue {
	uint64_t     RawValue;
	uint64_t     StartRawValue;
	double       Scale;
	double       Offset;
	bool         Signedness;
	const char * Units;
};

typedef std::map<std::string, std::map<std::string, AnalogValue > > AnalogMap;
typedef std::map<std::string, AnalogValue >::iterator AnalogMapVarIterator;
typedef std::map<std::string, std::map<std::string, AnalogValue > >::iterator AnalogMapGroupIterator;

class CanMsgParser {
public:
	CanMsgParser(const char * dbc_filename);
	virtual ~CanMsgParser();

	void processCanMessage(
		AnalogMap & analog_data,
		const intercom::DataMessage::CanMsg * can_msg
	);
	void decodeCanMessage(
		AnalogMap & analog_data,
		const message_t * dbc_msg,
		const intercom::DataMessage::CanMsg * can_msg
	);
	void processCanSignal(
		AnalogMap & analog_data,
		const signal_t * sgn,
		const message_t * dbc_msg,
		const intercom::DataMessage::CanMsg * can_msg,
		uint32 raw_value,
		double phys_value
	);

	busAssignment_t * busAssignment;
	signalFormat_t signalFormat;
};

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
	printf("< ");
	for (int i = 0; i < size; ++i) {
		for (int j = 7; j >= 0; --j) {
			unsigned char byte = b[i] & (1<<j);
			byte >>= j;
			printf("%u", byte);
		}
		printf(" ");
	}
	printf("> ");
}

void CanMsgParser::processCanMessage(AnalogMap & analog_data, const intercom::DataMessage::CanMsg * can_msg) {
	if (!can_msg) {
		return;
	}

	/* lookup can_msg in message hash */
	messageHashKey_t key = ntohl(can_msg->Id);
	message_t * dbc_msg = NULL;
	int i;

	/* loop over all bus assigments */
	for (i = 0; i < busAssignment->n ; i++) {
		busAssignmentEntry_t * entry = &busAssignment->list[i];

		/* check if bus matches */
		if ((entry->bus == -1) || (entry->bus == can_msg->Bus)) {
			dbc_msg = (message_t *)hashtable_search(entry->messageHash, &key);
			if (NULL != dbc_msg) {
				print_bits(can_msg->Dlc, can_msg->Payload); printf("\n");
				decodeCanMessage(analog_data, dbc_msg, can_msg);
				break; /* end search if message was found */
			}
		}
	}
}

void CanMsgParser::processCanSignal(AnalogMap & analog_data, const signal_t * sgn, const message_t * dbc_msg, const intercom::DataMessage::CanMsg * can_msg, uint32 raw_value, double phys_value) {
	analog_data[dbc_msg->name][sgn->name].RawValue = raw_value;

#if 1
	fprintf(stderr, "   RCV CAN: %s.%s = %f (raw=%ld=0x%lX): %d|%d@%d%c (%f + raw * %f) [%f|%f] %d %ul \"%s\"\n",
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

void CanMsgParser::decodeCanMessage(AnalogMap & analog_data, const message_t * dbc_msg, const intercom::DataMessage::CanMsg * can_msg) {
	if (!dbc_msg || !can_msg) {
		return;
	}

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
		if (sgn->signedness && (bit_len < 32)) {
			sint32 m = 1 << (bit_len-1);
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

		processCanSignal(analog_data, sgn, dbc_msg, can_msg, rawValue, physicalValue);
	}
}



static void can_bus_msg_cb(int can_bus, message_t * can_msg, void * arg) {
	AnalogMap & analog_data = *(reinterpret_cast<AnalogMap *>(arg));
	printf("  CAN MSG 0x%lX (%s) on bus %d\n", (unsigned long)can_msg->id, can_msg->name, can_bus);
}

static void can_bus_msg_sgn_cb(int can_bus, message_t * can_msg, signal_t * can_sgn, void * arg) {
	if (NULL != arg) {
		AnalogMap & analog_data = *(reinterpret_cast<AnalogMap *>(arg));
		AnalogValue & av        = analog_data[can_msg->name][can_sgn->name];
		av.StartRawValue        = 0;
		av.RawValue             = 0;
		av.Scale                = can_sgn->scale;
		av.Offset               = can_sgn->offset;
		av.Signedness           = (can_sgn->signedness != 0);
		av.Units                = can_sgn->unit != NULL ? can_sgn->unit : "";
	}
	printf("    SIGNAL %s (msg=0x%lX): len=%d end=%s sign=%s [sc=%.2f,of=%.2f] (%.2f-%.2f)\n",
		can_sgn->name,
		(unsigned long)can_msg->id,
		can_sgn->bit_len,
		can_sgn->endianess != 0 ? "le" : "be",
		can_sgn->signedness != 0 ? "-" : "+",
		can_sgn->scale,
		can_sgn->offset,
		can_sgn->min,
		can_sgn->max
	);
}

static void can_bus_msg_sgn_att_cb(int can_bus, message_t * can_msg, signal_t * can_sgn, attribute_t * sgn_att, void * arg) {
	if (NULL != arg && NULL != sgn_att && NULL != sgn_att->value) {
		AnalogMap & analog_data = *(reinterpret_cast<AnalogMap *>(arg));
		AnalogValue & av = analog_data[can_msg->name][can_sgn->name];
		if (!strcmp(sgn_att->name, "GenSigStartValue")) {
			switch (sgn_att->value->value_type) {
				case vt_integer:
					av.StartRawValue = (sgn_att->value->value.int_val) & (0xFFFFFFFFu & ((uint32)~0 >> (32 - can_sgn->bit_len)));
					break;
				case vt_float:
					break;
				case vt_string:
					break;
				case vt_enum:
					break;
				case vt_hex:
					break;
				default:
					break;
			}
			av.RawValue = av.StartRawValue;

			printf("      ATTRIBUTE %s -> %lu\n",
				sgn_att->name,
				av.StartRawValue
			);
		}
	}
}

static void rcv_loop(AnalogMap & analog_data, CanMsgParser * msg_parser) {
	intercom::Receiver receiver(intercom::Sys_Spy);
	while (true) {
		intercom::DataMessage msg;
		receiver.receive(msg);

		bool self_msg = false;
		if (receiver.getSysId() == msg.getSysId()) {
			fputs("* Msg Ign: ", stderr); msg.fprint(stderr); fputs("\n", stderr);
			self_msg = true;
		} else {
			fputs("* Msg Rcv: ", stderr); msg.fprint(stderr); fputs("\n", stderr);
		}

		if ((NULL != msg_parser) && (intercom::DataMessage::MsgCan == msg.getMsgType())) {
			intercom::DataMessage::CanMsg * can_msg = msg.getCanInfo();
			if (NULL != can_msg) {
				msg_parser->processCanMessage(analog_data, can_msg);
			}
		}
	}
}

int main(int argc, char * argv[]) {
	CanMsgParser * msg_parser = NULL;
	AnalogMap analog_data;

	msg_parser = new CanMsgParser("../../panel/dbc/can01.dbc");
	if (msg_parser) {
		if (msg_parser->busAssignment) {
			busAssignment_iterate(
				msg_parser->busAssignment,
				can_bus_msg_cb,
				can_bus_msg_sgn_cb,
				can_bus_msg_sgn_att_cb,
				&analog_data
			);
		}
	}

	rcv_loop(analog_data, msg_parser);

	return EXIT_SUCCESS;
}
