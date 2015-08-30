#include "RuntimeVarsWidgets.h"

#include "config.h"

#include "CarState.h"

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

RtVarsTree::RtVarsTree(int X, int Y, int W, int H, const char * L) : Fl_Tree(X,Y,W,H,L), p_CarState(NULL) {
}

RtVarsTree::~RtVarsTree() {
	if (p_CarState) {
		p_CarState->listeners.remove(this);
		p_CarState = NULL;
	}
}

void RtVarsTree::draw(void) {
	buildTree();
	Fl_Tree::draw();
}

void RtVarsTree::buildTree() {
	clear();
	if (p_CarState) {
		char buffer[64];
		buffer[sizeof(buffer)-1] = '\0';

		begin();

		// Can Values

		AnalogMapVarIterator v;
		AnalogMapGroupIterator g;

		for (g = p_CarState->analog_data.begin(); g != p_CarState->analog_data.end(); g++) {
			for (v = (*g).second.begin(); v != (*g).second.end(); v++) {
				Fl_Tree_Item *item;

				double physical_value;
				if ((*v).second.Signedness) {
					physical_value = (double)(int32_t)(*v).second.RawValue * (*v).second.Scale + (*v).second.Offset;
				} else {
					physical_value = (double)        (*v).second.RawValue * (*v).second.Scale + (*v).second.Offset;
				}

				snprintf(buffer, sizeof(buffer)-1, "%s/%s = %lu (%lf%s%s)",
					(*g).first.c_str(),
					(*v).first.c_str(),
					(long unsigned)(*v).second.RawValue,
					physical_value,
					(*v).second.Units[0] != '\0' ? " " : "",
					(*v).second.Units
				);

				item = add(buffer);
				item->user_data((void*)NULL);
				printf(">> %s\n", buffer);
			}
		}

		// Pwm Values
		PwmMapIterator p;
		for (p = p_CarState->pwm_data.begin(); p != p_CarState->pwm_data.end(); p++) {
				Fl_Tree_Item *item;
				snprintf(buffer, sizeof(buffer)-1, "PWM/%s = %lu of %lu (%.2lf%%)",
					(*p).first.c_str(),
					(long unsigned)(*p).second.PulseWidth,
					(long unsigned)(*p).second.Period,
					(long unsigned)(*p).second.Period == 0 ? 0.0 :
						100.0
							* static_cast<double>((long unsigned)(*p).second.PulseWidth)
							/ static_cast<double>((long unsigned)(*p).second.Period)
				);
				item = add(buffer);
				item->user_data((void*)NULL);
				printf(">> %s\n", buffer);
		}

		end();

	}
}

void RtVarsTree::setCarState(CarState * car_state) {
	if (p_CarState) {
		p_CarState->listeners.remove(this);
	}
	p_CarState = car_state;
	if (p_CarState) {
		p_CarState->listeners.push_back(this);
	}
	redraw();
}

void RtVarsTree::eventCarStateChanged(void) { // ICarStateListener
	//printf("eventCarStateChanged\n");
	redraw();
}
