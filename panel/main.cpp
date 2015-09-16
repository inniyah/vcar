#include "config.h"
#include "panel.h"
#include "CarSvgBox.h"
#include "CarState.h"
#include "RuntimeVarsWidgets.h"

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <assert.h>

#include <FL/Fl.H>
#include <FL/Fl_Tree.H>
#include <FL/Fl_Slider.H>
#include <FL/Fl_Dial.H>

typedef void (*CarCmdFunction)(void);

void CarCmdDoNothing() {
};

/*
void CarCmdToggleHazard() {
	fprintf(stderr, "Toggle Hazard\n");
	wCarBox->LeftHazardLights(1.0 - wCarBox->LeftHazardLights());
	wCarBox->RightHazardLights(1.0 - wCarBox->RightHazardLights());
};
*/

void ChangeAccel(Fl_Widget *w, void *data) {
	CarState * car_state = reinterpret_cast<CarState*>(data);
	assert(NULL != car_state);
	Fl_Slider * slider = reinterpret_cast<Fl_Slider *>(w);
	assert(NULL != slider);
	double value = (slider->maximum() - slider->value()) / slider->maximum();
	car_state->analog_data["driving_controls"]["acceleration"].RawValue = 250 * value;
	fprintf(stderr, "Accel %lf\n", value);
};

void ChangeBrake(Fl_Widget *w, void *data) {
	CarState * car_state = reinterpret_cast<CarState*>(data);
	assert(NULL != car_state);
	Fl_Slider * slider = reinterpret_cast<Fl_Slider *>(w);
	assert(NULL != slider);
	double value = (slider->maximum() - slider->value()) / slider->maximum();
	car_state->analog_data["driving_controls"]["brake"].RawValue = 250 * value;
	fprintf(stderr, "Brake %lf\n", value);

	if (slider->maximum() != slider->value()) {
		//wCarBox->BrakeLights(1.0);
	} else {
		//wCarBox->BrakeLights(0.0);
	}
};

void ChangeGear(Fl_Widget *w, void *data) {
	CarState * car_state = reinterpret_cast<CarState*>(data);
	assert(NULL != car_state);
	Fl_Slider * slider = reinterpret_cast<Fl_Slider *>(w);
	double value = (slider->maximum() - slider->value());
	car_state->analog_data["driving_controls"]["gear"].RawValue = value;
	car_state->engine.setGear(value);
	if (value != 0) {
		wGearBackSlider->deactivate();
	} else {
		wGearBackSlider->activate();
	}
}

void ChangeBackGear(Fl_Widget *w, void *data) {
	CarState * car_state = reinterpret_cast<CarState*>(data);
	assert(NULL != car_state);
	Fl_Slider * slider = reinterpret_cast<Fl_Slider *>(w);
	if (slider->maximum() != slider->value()) {
		car_state->analog_data["driving_controls"]["gear"].RawValue = 0;
		car_state->engine.setGear(0);
		//wCarBox->BackwardsLights(0.0);
		wGearSlider->activate();
	} else {
		car_state->analog_data["driving_controls"]["gear"].RawValue = 7;
		car_state->engine.setGear(7);
		//wCarBox->BackwardsLights(1.0);
		wGearSlider->deactivate();
	}
}

void ChangeSteeringWheel(Fl_Widget *w, void *data) {
	CarState * car_state = reinterpret_cast<CarState*>(data);
	assert(NULL != car_state);
	Fl_Dial * dial = reinterpret_cast<Fl_Dial *>(w);
	assert(NULL != dial);
	fprintf(stderr, "Steering Wheel %lf\n", dial->value() / dial->maximum());
};

void CarCmdOpenRightDoor(Fl_Widget *w, void *data) {
	wCarBox->RightDoorClosed(false);
};

void CarCmdCloseRightDoor(Fl_Widget *w, void *data) {
	wCarBox->RightDoorClosed(true);
};

void CarCmdOpenLeftDoor(Fl_Widget *w, void *data) {
	wCarBox->LeftDoorClosed(false);
};

void CarCmdCloseLeftDoor(Fl_Widget *w, void *data) {
	wCarBox->LeftDoorClosed(true);
};

void CarCmdTreeCallback(Fl_Widget *w, void *data) {
	Fl_Tree * tree = (Fl_Tree*)w;
	Fl_Tree_Item * item = (Fl_Tree_Item*)tree->callback_item();
	CarCmdFunction function = CarCmdDoNothing;
	if (NULL != item->user_data()) {
		function = reinterpret_cast<CarCmdFunction>(item->user_data());
	}
	switch ( tree->callback_reason() ) {
		case FL_TREE_REASON_OPENED:
			break;
		case FL_TREE_REASON_CLOSED:
			break;
		case FL_TREE_REASON_SELECTED:
			if ( Fl::event_clicks() == 1 ) {
				function();
			}
			break;
		case FL_TREE_REASON_DESELECTED:
			break;
		default:
			break;
	}
}

void initCarState(CarState & car_state) {
}

struct GlobalCarStateListener : public ICarStateListener {
	GlobalCarStateListener(CarState & car_state) : m_CarState(car_state) {
		m_CarState.listeners.push_back(this);
	}
	~GlobalCarStateListener() {
		m_CarState.listeners.remove(this);
	}
	virtual void eventCarStateChanged(void) {
		printf("eventCarStateChanged\n");
		Fl::lock();
		wCarBox->BrakeLights(m_CarState.pwm_data["BrkL"].getIntensity());
		wCarBox->BackwardsLights(m_CarState.pwm_data["BckL"].getIntensity());
		wCarBox->LeftHazardLights(m_CarState.pwm_data["LHaz"].getIntensity());
		wCarBox->RightHazardLights(m_CarState.pwm_data["RHaz"].getIntensity());
		wCarBox->InteriorLights(m_CarState.pwm_data["IntL"].getIntensity());
		wCarBox->redraw();
		Fl::unlock();
		Fl::awake();
	}
private:
	CarState & m_CarState;
};

void globalUpdate(CarState & car_state) {
	Fl::lock();

	double accel_value = (wAccelSlider->maximum() - wAccelSlider->value()) / wAccelSlider->maximum();
	double brake_value = (wBrakeSlider->maximum() - wBrakeSlider->value()) / wBrakeSlider->maximum();
	car_state.engine.update(0.1, accel_value, brake_value);

	double rpm = car_state.engine.getRpm();
	wRpmDial->setDialValue(rpm / 8000.0);
	//car_state.analog_data["engine"]["rpm"].RawValue = rpm;
	wRpmDial->redraw();

	double speed = car_state.engine.getV();
	wRG1Dial->setDialValue(speed / 250.0);
	//car_state.analog_data["engine"]["speed"].RawValue = speed;
	wRG1Dial->redraw();

	Fl::unlock();
	Fl::awake();
}

int main(int argc, char * argv[]) {
	Fl_Double_Window * panel_window = makePanelWindow();

	CarState car_state;
	initCarState(car_state);
	GlobalCarStateListener car_state_listener(car_state);

	wCarCmdTree->root()->label("Commands");
	wCarCmdTree->selectmode(FL_TREE_SELECT_SINGLE);
	wCarCmdTree->begin();
	{
		Fl_Tree_Item *item;
		//item = wCarCmdTree->add("Lights/Toggle Hazard");
		//item->user_data((void*)CarCmdToggleHazard);
		item = wCarCmdTree->add("Doors/Open Left Door");
		item->user_data((void*)CarCmdOpenLeftDoor);
		item = wCarCmdTree->add("Doors/Close Left Door");
		item->user_data((void*)CarCmdCloseLeftDoor);
		item = wCarCmdTree->add("Doors/Open Right Door");
		item->user_data((void*)CarCmdOpenRightDoor);
		item = wCarCmdTree->add("Doors/Close Right Door");
		item->user_data((void*)CarCmdCloseRightDoor);
	}
	wCarCmdTree->end();
	wCarCmdTree->callback(CarCmdTreeCallback);

	wAccelSlider->scrollvalue(0 /*pos*/, 1 /*size*/, 0 /*first*/, 11 /*total*/);
	wAccelSlider->value(wAccelSlider->maximum());
	wAccelSlider->callback(ChangeAccel, &car_state);

	wBrakeSlider->scrollvalue(0 /*pos*/, 1 /*size*/, 0 /*first*/, 11 /*total*/);
	wBrakeSlider->value(wBrakeSlider->maximum());
	wBrakeSlider->callback(ChangeBrake, &car_state);

	wGearSlider->scrollvalue(0 /*pos*/, 1 /*size*/, 0 /*first*/, 6 /*total*/); /* 5 Gears */
	wGearSlider->value(wGearSlider->maximum());
	wGearSlider->callback(ChangeGear, &car_state);
	wGearBackSlider->scrollvalue(0 /*pos*/, 1 /*size*/, 0 /*first*/, 2 /*total*/);
	wGearBackSlider->value(0);
	wGearBackSlider->callback(ChangeBackGear, &car_state);

	wSteeringWheel->angles(0, 360);
	wSteeringWheel->callback(ChangeSteeringWheel, &car_state);
	wSteeringWheel->value(0.5f);

	wRpmDial->setSvgFilename("svg/dial_rpm.svg");
	wRG1Dial->setSvgFilename("svg/dial_rg1.svg");
	wOilWaterDial->setSvgFilename("svg/dial_ow.svg");

	wRuntimeVars->setCarState(&car_state);

	panel_window->show(argc, argv);

	// Enable multi-thread support by locking from the main
	// thread.  Fl::wait() and Fl::run() call Fl::unlock() and
	// Fl::lock() as needed to release control to the child threads
	// when it is safe to do so...
	Fl::lock();

	return Fl::run();
}
