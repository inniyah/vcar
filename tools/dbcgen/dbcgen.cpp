#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "model/dbcReader.h"
#include "model/dbcWriter.h"

#include <stdint.h>

#include <cstdlib>
#include <cstdio>
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
};

typedef std::map<std::string, std::map<std::string, CanSignalInfo > >           CanSignalMap;
typedef std::map<std::string, CanSignalInfo >::iterator                         CanSignalMapElementIterator;
typedef std::map<std::string, std::map<std::string, CanSignalInfo > >::iterator CanSignalMapGroupIterator;

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
					si.Scale      = can_sgn->scale;
					si.Offset     = can_sgn->offset;
					si.Signedness = (can_sgn->signedness != 0);
					si.Units      = can_sgn->unit != NULL ? can_sgn->unit : "";
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
