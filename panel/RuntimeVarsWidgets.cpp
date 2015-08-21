#include "RuntimeVarsWidgets.h"

#include "config.h"

#include "CarState.h"

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

void RtVarsTree::draw(void) {
	buildTree();
	Fl_Tree::draw();
}

void RtVarsTree::buildTree() {
	clear();
	if (p_CarState) {
		char buffer[64];
		buffer[sizeof(buffer)-1] = '\0';
		AnalogMapVarIterator v;
		AnalogMapGroupIterator g;
		begin();
		for (g = p_CarState->analog_data.begin(); g != p_CarState->analog_data.end(); g++) {
			for (v = (*g).second.begin(); v != (*g).second.end(); v++) {
				Fl_Tree_Item *item;

				double physical_value;
				if ((*v).second.Sign) {
					physical_value = (double)(int32_t)(*v).second.RawValue * (*v).second.Scale + (*v).second.Offset;
				} else {
					physical_value = (double)        (*v).second.RawValue * (*v).second.Scale + (*v).second.Offset;
				}

				snprintf(buffer, sizeof(buffer)-1, "%s/%s = %lu (%lf)", (*g).first.c_str(), (*v).first.c_str(), (long unsigned)(*v).second.RawValue, physical_value);
				item = add(buffer);
				item->user_data((void*)NULL);
				printf(">> %s\n", buffer);
			}
		end();
		}
	}
}

void RtVarsTree::setCarState(CarState * car_state) {
	p_CarState = car_state;
	redraw();
}