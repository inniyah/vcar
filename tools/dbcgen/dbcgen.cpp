#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "model/dbcReader.h"
#include "model/dbcWriter.h"

#include <stdint.h>

#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <cassert>
#include <string>
#include <iostream>
#include <map>
#include <string>
#include <algorithm>
#include <iomanip>
#include <vector>

struct CanMessageInfo {
	uint32_t     Id;
	uint8_t      Dlt;
};

typedef std::map<std::string, CanMessageInfo >                                  CanMessageMap;
typedef std::map<std::string, CanMessageInfo >::iterator                        CanMessageMapIterator;

typedef std::vector<std::pair<uint32_t,std::string> >                           CanMessageIdList;
typedef std::vector<std::pair<uint32_t,std::string> >::iterator                 CanMessageIdListIterator;

struct CanSignalInfo {
	double       Scale;
	double       Offset;
	bool         Signedness;
	std::string  Units;
	std::string  GetValueCode;
	std::string  SetValueCode;
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
			CanMessageMap    can_msg_map;
			CanMessageIdList can_msg_list;
			CanSignalMap     can_sgn_map;

			message_list_t * ml;
			for (ml = dbc->message_list; ml != NULL; ml = ml->next) {
				message_t * can_msg = ml->message;
				fprintf(stderr, "  CAN MSG 0x%lX (%s)\n", (unsigned long)can_msg->id, can_msg->name);
				CanMessageInfo & mi = can_msg_map[can_msg->name];
				mi.Id  = can_msg->id;
				mi.Dlt = can_msg->len;
				can_msg_list.push_back(std::make_pair(can_msg->id, can_msg->name));
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
					si.GetValueCode = getCanSignalDecoder(can_msg, can_sgn);
					si.SetValueCode = getCanSignalEncoder(can_msg, can_sgn);
				}
			}
			dbc_free(dbc);
			dbc = NULL;

			std::sort(can_msg_list.begin(), can_msg_list.end());

			CanSignalMapElementIterator v;
			CanSignalMapGroupIterator g;
			for (g = can_sgn_map.begin(); g != can_sgn_map.end(); g++) {
				printf("#pragma pack(1)\n");
				printf("struct CanMsg_%s {\n", (*g).first.c_str());
				printf("\tstatic const uint8_t id  = 0x%04X;\n",  can_msg_map[(*g).first].Id);
				printf("\tstatic const uint8_t dlt = %d;\n", can_msg_map[(*g).first].Dlt);
				printf("\tuint8_t data[%d];\n", can_msg_map[(*g).first].Dlt);

				for (v = (*g).second.begin(); v != (*g).second.end(); v++) {
					if (0 != (*v).second.Offset) {
						printf("\tinline int getSignal_%s() const { /* Real Value = Raw Value * %lf + %lf",
							(*v).first.c_str(),
							(*v).second.Scale,
							(*v).second.Offset
						);
					} else {
						printf("\tinline int getSignal_%s() const { /* Real Value = Raw Value * %lf",
							(*v).first.c_str(),
							(*v).second.Scale
						);
					}
					printf("%s%s */\n%s\n\t}\n",
						(*v).second.Units.c_str()[0] != '\0' ? " " : "",
						(*v).second.Units.c_str(),
						(*v).second.GetValueCode.c_str()
					);
					printf("\tinline void setSignal_%s(int value) {\n%s\n\t}\n",
						(*v).first.c_str(),
						(*v).second.SetValueCode.c_str()
					);
					printf("\tinline void setDefaultSignal_%s() {\n\t\tsetSignal_%s(%ld); /* %lf%s%s */\n\t}\n",
						(*v).first.c_str(),
						(*v).first.c_str(),
						(*v).second.StartRawValue,
						(double) (long signed int) (*v).second.StartRawValue * (*v).second.Scale + (*v).second.Offset,
						(*v).second.Units.c_str()[0] != '\0' ? " " : "",
						(*v).second.Units.c_str()
					);
				}

				printf("\tinline void setDefaultSignals() {\n");
				printf("\t\tmemset(data, 0, sizeof(data));\n");
				for (v = (*g).second.begin(); v != (*g).second.end(); v++) {
					printf("\t\tsetDefaultSignal_%s();\n",
						(*v).first.c_str()
					);
				}
				printf("\t}\n");

				printf("} __attribute__((packed)); /* CanMsg_%s */\n", (*g).first.c_str());
				printf("#pragma pack()\n\n");
			}

			printf("struct CanDb {\n");
			for (CanMessageIdListIterator it = can_msg_list.begin(); it != can_msg_list.end(); it++) {
				printf("\tCanMsg_%s Msg_%s;\n", (*it).second.c_str(), (*it).second.c_str());
			}
			printf("\n");

			printf("\tuint8_t * getCanMessageBuffer(uint32_t id, uint8_t dlt) {\n");
			printf("\t\tswitch(id) {\n");
			for (CanMessageIdListIterator it = can_msg_list.begin(); it != can_msg_list.end(); it++) {
				printf("\t\t\tcase Msg_%s.id:  /* 0x%04X or %u */\n\t\t\t\treturn (Msg_%s.dlt == dlt ? Msg_%s.data : NULL);\n",
					(*it).second.c_str(),
					(*it).first,
					(*it).first,
					(*it).second.c_str(),
					(*it).second.c_str()
				);
			}
			printf("\t\t\tdefault:\n\t\t\t\treturn NULL;\n");
			printf("\t\t}\n");
			printf("\t}\n\n");

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

#if 0
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
#endif

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

	char cstr[512];
	memset(cstr, 0, sizeof(cstr));

	uint8_t msg_mask[8];
	memset(msg_mask, 0, sizeof(msg_mask));

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
		char   buff[256];

		/* loop over all source bytes from start_byte to end_byte */
		for (int work_byte = start_byte; work_byte <= end_byte; ++work_byte) {
			if (work_byte == start_byte && work_byte == end_byte && start_offset != 7 && end_offset != 0) {
				mask   = (uint8)~0 >> (7 - start_offset + end_offset) << end_offset;
				rotate = end_offset;
				shift  = start_offset - end_offset + 1;
			} else if (work_byte == start_byte && start_offset != 7) {
				mask   = (uint8)~0 >> (7 - start_offset);
				rotate = 0;
				shift  = start_offset + 1;
			} else if (work_byte == end_byte && end_offset != 0) {
				mask   = 0xFF ^ ((uint8)~0 >> (8 - end_offset));
				rotate = end_offset;
				shift  = 8 - end_offset;
			} else {
				mask   = 0xFF;
				rotate = 0;
				shift  = 8;
			}

			msg_mask[work_byte] = ( (0xFF & mask) | (msg_mask[work_byte] & (~mask)) );

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
				strncpy(cstr, "\t\treturn ", sizeof(cstr)-1);
			} else {
				strncat(cstr, "\n\t\t\t+ ", sizeof(cstr)-1);
			}
			strncat(cstr, buff, sizeof(cstr)-1);
		}
		assert(0 == position);

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
		uint8  position   = 0;
		char   buff[256];

		for (int work_byte = start_byte; work_byte <= end_byte; ++work_byte) {
			if (work_byte == start_byte && work_byte == end_byte && start_offset != 0 && end_offset != 7) {
				mask   = (uint8)~0 >> (7 - end_offset + start_offset) << (start_offset);
				rotate = start_offset;
				shift  = end_offset - start_offset + 1;
			} else if (work_byte == end_byte && end_offset != 7) {
				mask   = (uint8)~0 >> (7 - end_offset);
				rotate = 0;
				shift  = end_offset + 1;
			} else if (work_byte == start_byte && start_offset != 0) {
				mask   = 0xFF ^ ((uint8)~0 >> (8 - start_offset));
				rotate = start_offset;
				shift  = 8 - start_offset;
			} else {
				mask   = 0xFF;
				rotate = 0;
				shift  = 8;
			}

			msg_mask[work_byte] = ( (0xFF & mask) | (msg_mask[work_byte] & (~mask)) );

			memset(buff, 0, sizeof(buff));
			if (position > rotate) {
				snprintf(buff, sizeof(buff)-1, "((static_cast<int>(data[%d]) & 0x%02X) << %d)", work_byte, mask, (position - rotate));
			} else if (position < rotate) {
				snprintf(buff, sizeof(buff)-1, "((static_cast<int>(data[%d]) & 0x%02X) >> %d)", work_byte, mask, (rotate - position));
			} else {
				snprintf(buff, sizeof(buff)-1, "(static_cast<int>(data[%d]) & 0x%02X)", work_byte, mask);
			}

			if ('\0' == cstr[0]) {
				strncpy(cstr, "\t\treturn ", sizeof(cstr)-1);
			} else {
				strncat(cstr, "\n\t\t\t+ ", sizeof(cstr)-1);
			}
			strncat(cstr, buff, sizeof(cstr)-1);

			position += shift;
		}
		assert(can_sgn->bit_len == position);

	}

	strncat(cstr, ";", sizeof(cstr)-1);

	//print_bits(dbc_msg->len, msg_mask); printf("\n");
	//printf("%s\n", cstr);
	return cstr;
}

std::string getCanSignalEncoder(const message_t * dbc_msg, const signal_t * can_sgn) {
	if (!dbc_msg || !can_sgn) {
		return "";
	}

	char cstr[512];
	memset(cstr, 0, sizeof(cstr));

	uint8_t msg_mask[8];
	memset(msg_mask, 0, sizeof(msg_mask));

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
		uint8  position     = can_sgn->bit_len;
		char   buff[100];

		//printf("%u:%u -> %u:%u (be)\n", start_byte, start_offset, end_byte, end_offset);

		for (int work_byte = start_byte; work_byte <= end_byte; ++work_byte) {
			if (work_byte == start_byte && work_byte == end_byte && start_offset != 7 && end_offset != 0) {
				mask   = (uint8)~0 >> (7 - start_offset + end_offset) << end_offset;
				rotate = end_offset;
				shift  = start_offset - end_offset + 1;
			} else if (work_byte == start_byte && start_offset != 7) {
				mask   = (uint8)~0 >> (7 - start_offset);
				rotate = 0;
				shift  = start_offset + 1;
			} else if (work_byte == end_byte && end_offset != 0) {
				mask   = 0xFF ^ ((uint8)~0 >> (8 - end_offset));
				rotate = end_offset;
				shift  = 8 - end_offset;
			} else {
				mask   = 0xFF;
				rotate = 0;
				shift  = 8;
			}

			msg_mask[work_byte] = ( (0xFF & mask) | (msg_mask[work_byte] & (~mask)) );

			position -= shift;
			memset(buff, 0, sizeof(buff));
			if (0xFF == mask) {
				if (0 == rotate) {
					if (0 == position) {
						snprintf(buff, sizeof(buff)-1, "data[%d] = static_cast<int>(value) & 0xFF;", work_byte);
					} else {
						snprintf(buff, sizeof(buff)-1, "data[%d] = (static_cast<int>(value) >> %d) & 0xFF;", work_byte, position);
					}
				} else {
					if (0 == position) {
						snprintf(buff, sizeof(buff)-1, "data[%d] = (static_cast<int>(value) << %d) & 0xFF;", work_byte, rotate);
					} else {
						snprintf(buff, sizeof(buff)-1, "data[%d] = ((static_cast<int>(value) >> %d) << %d) & 0xFF;", work_byte, position, rotate);
					}
				}
			} else {
				if (0 == rotate) {
					if (0 == position) {
						snprintf(buff, sizeof(buff)-1, "data[%d] = (0x%02X & data[%d]) | (0x%02X & static_cast<int>(value));", work_byte, (~mask) & 0xFF, work_byte, mask & 0xFF);
					} else {
						snprintf(buff, sizeof(buff)-1, "data[%d] = (0x%02X & data[%d]) | (0x%02X & (static_cast<int>(value) >> %d));", work_byte, (~mask) & 0xFF, work_byte, mask & 0xFF, position);
					}
				} else {
					if (0 == position) {
						snprintf(buff, sizeof(buff)-1, "data[%d] = (0x%02X & data[%d]) | (0x%02X & (static_cast<int>(value) << %d));", work_byte, (~mask) & 0xFF, work_byte, mask & 0xFF, rotate);
					} else {
						snprintf(buff, sizeof(buff)-1, "data[%d] = (0x%02X & data[%d]) | (0x%02X & ((static_cast<int>(value) >> %d) << %d));", work_byte, (~mask) & 0xFF, work_byte, mask & 0xFF, position, rotate);
					}
				}
			}
			if ('\0' == cstr[0]) {
				strncpy(cstr, "\t\t",   sizeof(cstr)-1);
			} else {
				strncat(cstr, "\n\t\t", sizeof(cstr)-1);
			}
			strncat(cstr, buff, sizeof(cstr)-1);
		}
		assert(0 == position);

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
		uint8  position     = 0;
		char   buff[100];

		//printf("%u:%u -> %u:%u (le)\n", start_byte, start_offset, end_byte, end_offset);

		for (int work_byte = start_byte; work_byte <= end_byte; ++work_byte) {
			if (work_byte == end_byte && work_byte == start_byte && (start_offset != 0 || end_offset != 7)) {
				mask    = (uint8)~0 >> (7 - end_offset + start_offset) << (start_offset);
				rotate  = start_offset;
				shift   = end_offset - start_offset + 1;
			} else if (work_byte == end_byte && end_offset != 7) {
				mask   = (uint8)~0 >> (7 - end_offset);
				rotate = 0;
				shift  = end_offset + 1;
			} else if (work_byte == start_byte && start_offset != 0) {
				mask   = 0xFF ^ ((uint8)~0 >> (8 - start_offset));
				rotate = start_offset;
				shift  = 8 - start_offset;
			} else {
				mask   = 0xFF;
				rotate = 0;
				shift  = 8;
			}

			msg_mask[work_byte] = ( (0xFF & mask) | (msg_mask[work_byte] & (~mask)) );

			memset(buff, 0, sizeof(buff));
			if (0xFF == mask) {
				if (0 == rotate) {
					if (0 == position) {
						snprintf(buff, sizeof(buff)-1, "data[%d] = static_cast<int>(value) & 0xFF;", work_byte);
					} else {
						snprintf(buff, sizeof(buff)-1, "data[%d] = (static_cast<int>(value) >> %d) & 0xFF;", work_byte, position);
					}
				} else {
					if (0 == position) {
						snprintf(buff, sizeof(buff)-1, "data[%d] = (static_cast<int>(value) << %d) & 0xFF;", work_byte, rotate);
					} else {
						snprintf(buff, sizeof(buff)-1, "data[%d] = ((static_cast<int>(value) >> %d) << %d) & 0xFF;", work_byte, position, rotate);
					}
				}
			} else {
				if (0 == rotate) {
					if (0 == position) {
						snprintf(buff, sizeof(buff)-1, "data[%d] = (0x%02X & data[%d]) | (0x%02X & static_cast<int>(value));", work_byte, (~mask) & 0xFF, work_byte, mask & 0xFF);
					} else {
						snprintf(buff, sizeof(buff)-1, "data[%d] = (0x%02X & data[%d]) | (0x%02X & (static_cast<int>(value) >> %d));", work_byte, (~mask) & 0xFF, work_byte, mask & 0xFF, position);
					}
				} else {
					if (0 == position) {
						snprintf(buff, sizeof(buff)-1, "data[%d] = (0x%02X & data[%d]) | (0x%02X & (static_cast<int>(value) << %d));", work_byte, (~mask) & 0xFF, work_byte, mask & 0xFF, rotate);
					} else {
						snprintf(buff, sizeof(buff)-1, "data[%d] = (0x%02X & data[%d]) | (0x%02X & ((static_cast<int>(value) >> %d) << %d));", work_byte, (~mask) & 0xFF, work_byte, mask & 0xFF, position, rotate);
					}
				}
			}
			if ('\0' == cstr[0]) {
				strncpy(cstr, "\t\t",   sizeof(cstr)-1);
			} else {
				strncat(cstr, "\n\t\t", sizeof(cstr)-1);
			}
			strncat(cstr, buff, sizeof(cstr)-1);

			position += shift;
		}
		assert(can_sgn->bit_len == position);

	}

	//print_bits(dbc_msg->len, msg_mask); printf("\n");
	//printf("%s\n", cstr);
	return cstr;
}
