#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "model/dbcReader.h"
#include "model/dbcWriter.h"

#include <stdint.h>

#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <string>
#include <iostream>
#include <map>
#include <string>
#include <algorithm>
#include <iomanip>
#include <vector>

struct CanMessageInfo {
	uint8_t      Dlt;
};

typedef std::map<std::string, CanMessageInfo >                                  CanMessageMap;
typedef std::map<std::string, CanMessageInfo >::iterator                        CanMessageMapIterator;

struct CanSignalInfo {
	double       Scale;
	double       Offset;
	bool         Signedness;
	std::string  Units;
	uint64_t     StartRawValue;
};

typedef std::map<std::string, std::map<std::string, CanSignalInfo > >           CanSignalMap;
typedef std::map<std::string, CanSignalInfo >::iterator                         CanSignalMapElementIterator;
typedef std::map<std::string, std::map<std::string, CanSignalInfo > >::iterator CanSignalMapGroupIterator;

typedef enum {
	CanSignalAttr_Unknown,
	CanSignalAttr_GenSigStartValue
} CanSignalAttrId;


std::string getCanSignalDecoder(const message_t * dbc_msg, const signal_t * can_sgn);
std::string getCanSignalEncoder(const message_t * dbc_msg, const signal_t * can_sgn);

int main(int argc, const char * argv[]) {
	dbc_t      * dbc;
	const char * inFilename  = (argc>1 ? argv[1] : NULL);
	const char * outFilename = (argc>2 ? argv[2] : NULL);
	int ret = EXIT_SUCCESS;

	FILE * dbcout;

	if (outFilename != NULL) {
		if(NULL == (dbcout = fopen(outFilename,"w"))) {
			fprintf(stderr, "can't open output file '%s'\n", outFilename);
			ret = EXIT_FAILURE;
		}
	} else {
		dbcout = stdout;
	}

	if (dbcout != NULL) {

		if (NULL != (dbc = dbc_read_file(inFilename))) {
			CanMessageMap can_msg_map;
			CanSignalMap  can_sgn_map;

			message_list_t * ml;
			for (ml = dbc->message_list; ml != NULL; ml = ml->next) {
				message_t * can_msg = ml->message;
				fprintf(stderr, "  CAN MSG 0x%lX (%s)\n", (unsigned long)can_msg->id, can_msg->name);
				CanMessageInfo & mi = can_msg_map[can_msg->name];
				mi.Dlt      = can_msg->len;
				signal_list_t * sl;
				for(sl = can_msg->signal_list; sl != NULL; sl = sl->next) {
					signal_t * can_sgn = sl->signal;
					fprintf(stderr, "    SIGNAL %s (msg=0x%lX): len=%d end=%s sign=%s [sc=%.2f,of=%.2f] (%.2f-%.2f) [%s]\n",
						can_sgn->name,
						(unsigned long)can_msg->id,
						can_sgn->bit_len,
						can_sgn->endianess != 0 ? "le" : "be",
						can_sgn->signedness != 0 ? "-" : "+",
						can_sgn->scale,
						can_sgn->offset,
						can_sgn->min,
						can_sgn->max,
						can_sgn->unit
					);
					CanSignalInfo & si = can_sgn_map[can_msg->name][can_sgn->name];
					si.Scale         = can_sgn->scale;
					si.Offset        = can_sgn->offset;
					si.Signedness    = (can_sgn->signedness != 0);
					si.Units         = can_sgn->unit != NULL ? can_sgn->unit : "";
					si.StartRawValue = 0;

					attribute_list_t * al;
					for (al = sl->signal->attribute_list; al != NULL; al = al->next) {
						attribute_t * sgn_att = al->attribute;
						CanSignalAttrId attr_id = CanSignalAttr_Unknown;
						if (!strcmp(sgn_att->name, "GenSigStartValue")) {
							attr_id = CanSignalAttr_GenSigStartValue;
						}
						switch (sgn_att->value->value_type) {
							case vt_integer: /* sgn_att->value->value.int_val; */
								switch (attr_id) {
									case CanSignalAttr_GenSigStartValue:
										si.StartRawValue = sgn_att->value->value.int_val;
									default:
										break;
								}
								break;
							case vt_float:   /* sgn_att->value->value.double_val; */
								break;
							case vt_string:  /* sgn_att->value->value.string_val; */
								break;
							case vt_enum:    /* sgn_att->value->value.enum_val; */
								break;
							case vt_hex:     /* sgn_att->value->value.hex_val; */
								break;
							default:
								break;
						}
					}
					getCanSignalDecoder(can_msg, can_sgn);
					getCanSignalEncoder(can_msg, can_sgn);
				}
			}
			dbc_free(dbc);
			dbc = NULL;

			CanSignalMapElementIterator v;
			CanSignalMapGroupIterator g;
			for (g = can_sgn_map.begin(); g != can_sgn_map.end(); g++) {
				printf("struct CanMsg_%s {\n", (*g).first.c_str());
				for (v = (*g).second.begin(); v != (*g).second.end(); v++) {

					printf("\tuint32_t %s; /* %s */\n",
						(*v).first.c_str(),
						(*v).second.Units.c_str()
					);
				}
				printf("}; /* CanMsg_%s */\n\n", (*g).first.c_str());
			}

		} else {
			fprintf(stderr, "can't open input file '%s'\n", inFilename);
			ret = EXIT_FAILURE;
		}

	}

	if (dbcout != NULL) {
		if(outFilename != NULL) {
			fclose(dbcout);
		}
	}

	return ret;
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

std::string getCanSignalDecoder(const message_t * dbc_msg, const signal_t * can_sgn) {
	if (!dbc_msg || !can_sgn) {
		return "";
	}

	char cstr[256];
	memset(cstr, 0, sizeof(cstr));

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

	uint8  bit_len      = can_sgn->bit_len;
	uint8  start_offset = can_sgn->bit_start & 7;
	uint8  start_byte   = can_sgn->bit_start / 8;

	uint8  mask;
	uint8  rotate;
	uint8  shift;

	if (can_sgn->endianess == 0) { /* 0 = Big Endian */
		/*
		 *     7  6  5  4  3  2  1  0      offset
		 *
		 * big endian place value exponent
		 *                   15 14 13   <- start_byte
		 *    12 11 10  9  8  7  6  5 
		 *     4  3  2  1  0            <- end_byte
		 */

		uint8  end_byte   = start_byte + (7 + bit_len - start_offset - 1)/8;
		uint8  end_offset = (start_offset - bit_len + 1) & 7;
		uint8  position   = can_sgn->bit_len;
		char   buff[100];

		/* loop over all source bytes from start_byte to end_byte */
		for (int work_byte = start_byte; work_byte <= end_byte; ++work_byte) {
			if (work_byte == start_byte && work_byte == end_byte && start_offset != 7 && end_offset != 0) {
				mask = (uint8)~0 >> (7 - start_offset);
				rotate = end_offset;
				shift = start_offset - end_offset + 1;
			} else if (work_byte == start_byte && start_offset != 7) {
				mask = (uint8)~0 >> (7 - start_offset);
				rotate = 0;
				shift = start_offset + 1;
			} else if (work_byte == end_byte && end_offset != 0) {
				mask = 0xFF;
				rotate = end_offset;
				shift = 8 - end_offset;
			} else {
				mask = 0xFF;
				rotate = 0;
				shift = 8;
			}

			position -= shift;
			memset(buff, 0, sizeof(buff));
			if (position > rotate) {
				snprintf(buff, sizeof(buff)-1, "((static_cast<int>(data[%d]) & 0x%02X) << %d)", work_byte, mask, (position - rotate));
			} else if (position < rotate) {
				snprintf(buff, sizeof(buff)-1, "((static_cast<int>(data[%d]) & 0x%02X) >> %d)", work_byte, mask, (rotate - position));
			} else {
				snprintf(buff, sizeof(buff)-1, "(static_cast<int>(data[%d]) & 0x%02X)", work_byte, mask);
			}

			if ('\0' == cstr[0]) {
				strncpy(cstr, buff, sizeof(cstr)-1);
			} else {
				strncat(cstr, " + ", sizeof(cstr)-1);
				strncat(cstr, buff,  sizeof(cstr)-1);
			}
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

		uint8  end_byte   = start_byte + (bit_len + start_offset - 1)/8;
		uint8  end_offset = (start_offset + bit_len - 1) & 7;
		uint8  position   = can_sgn->bit_len;
		char   buff[100];

		for (int work_byte = end_byte; work_byte >= start_byte; --work_byte) {
			if (work_byte == start_byte && work_byte == end_byte && start_offset != 0 && end_offset != 7) {
				mask = (uint8)~0 >> (7 - end_offset);
				rotate = start_offset;
				shift = end_offset - start_offset + 1;
			} else if (work_byte == end_byte && end_offset != 7) {
				mask = (uint8)~0 >> (7 - end_offset);
				rotate = 0;
				shift = end_offset + 1;
			} else if (work_byte == start_byte && start_offset != 0) {
				mask = 0xFF;
				rotate = start_offset;
				shift = 8 - start_offset;
			} else {
				mask = 0xFF;
				rotate = 0;
				shift = 8;
			}

			position -= shift;
			memset(buff, 0, sizeof(buff));
			if (position > rotate) {
				snprintf(buff, sizeof(buff)-1, "((static_cast<int>(data[%d]) & 0x%02X) << %d)", work_byte, mask, (position - rotate));
			} else if (position < rotate) {
				snprintf(buff, sizeof(buff)-1, "((static_cast<int>(data[%d]) & 0x%02X) >> %d)", work_byte, mask, (rotate - position));
			} else {
				snprintf(buff, sizeof(buff)-1, "(static_cast<int>(data[%d]) & 0x%02X)", work_byte, mask);
			}

			if ('\0' == cstr[0]) {
				strncpy(cstr, buff, sizeof(cstr)-1);
			} else {
				strncat(cstr, " + ", sizeof(cstr)-1);
				strncat(cstr, buff,  sizeof(cstr)-1);
			}
		}
	}

	printf("%s\n", cstr);
	return cstr;
}

std::string getCanSignalEncoder(const message_t * dbc_msg, const signal_t * can_sgn) {
	if (!dbc_msg || !can_sgn) {
		return "";
	}

	char cstr[256];
	memset(cstr, 0, sizeof(cstr));

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

	uint8  bit_len      = can_sgn->bit_len;
	uint8  start_offset = can_sgn->bit_start & 7;
	uint8  start_byte   = can_sgn->bit_start / 8;
	uint8  mask;
	uint8  rotate;
	uint8  shift;

#if 0
	/* TODO perform sign extension */
	if (can_sgn->signedness && (bit_len < 32)) {
		sint32 m = 1<< (bit_len-1);
		rawValue = ((sint32)rawValue + m) ^ m;
	}
#endif

	if (can_sgn->endianess == 0) { /* 0 = Big Endian */
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
		uint8  position   = can_sgn->bit_len;
		char   buff[100];

		printf("%u:%u -> %u:%u\n", start_byte, start_offset, end_byte, end_offset);

		for (int work_byte = end_byte; work_byte >= start_byte; --work_byte) {
			if (work_byte == end_byte && end_offset != 7) {
				mask   =  0xFF ^ ((uint8)~0 >> (9 - end_offset));
				rotate = 7 - end_offset - 2;
				shift  = 9 - end_offset;
			} else if (work_byte == start_byte && start_offset != 0) {
				mask   = (uint8)~0 >> (8 - start_offset);
				rotate = 0;
				shift  = start_offset;
			} else {
				mask   = 0xFF;
				rotate = 0;
				shift  = 8;
			}

			position -= shift;
			memset(buff, 0, sizeof(buff));

			//can_msg->Payload[work_byte] = ( (data & mask) | (can_msg->Payload[work_byte] & (~mask)) );
			//mask_msg.Payload[work_byte] = ( (0xFF & mask) | (mask_msg.Payload[work_byte] & (~mask)) );
			//rawValue >>= shift;
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
		uint8  position   = can_sgn->bit_len;
		char   buff[100];

		printf("%u:%u -> %u:%u\n", start_byte, start_offset, end_byte, end_offset);

		for (int work_byte = start_byte; work_byte <= end_byte; ++work_byte) {
			if (work_byte == end_byte && work_byte == start_byte && (start_offset != 0 || end_offset != 7)) {
				mask    = (uint8)~0 >> (7 - end_offset + start_offset) << (start_offset);
				rotate  = start_offset;
				shift   = start_offset;
			} else if (work_byte == end_byte && end_offset != 7) {
				mask   = 0xFF ^ ((uint8)~0 >> (7 - end_offset));
				rotate = 7 - end_offset - 2;
				shift  = 7 - end_offset;
			} else if (work_byte == start_byte && start_offset != 0) {
				mask   = (uint8)~0 >> (8 - start_offset);
				rotate = 0;
				shift  = start_offset;
			} else {
				mask   = 0xFF;
				rotate = 0;
				shift  = 8;
			}

			position -= shift;
			memset(buff, 0, sizeof(buff));

			printf("data[%d] = (((static_cast<int>(v) >> %d) & 0x%02X) << %d)\n", work_byte, position, mask, rotate);

			//can_msg->Payload[work_byte] = ( (data & mask) | (can_msg->Payload[work_byte] & (~mask)) );
			//mask_msg.Payload[work_byte] = ( (0xFF & mask) | (mask_msg.Payload[work_byte] & (~mask)) );
			//rawValue >>= shift;
		}

	}

	printf("%s\n", cstr);
	return cstr;
}
