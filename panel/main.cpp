#include "config.h"
#include "panel.h"
#include "CarSvgBox.h"

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include <FL/Fl.H>
#include <FL/Fl_Tree.H>
#include <FL/Fl_Slider.H>
#include <FL/Fl_Dial.H>

typedef void (*CarCmdFunction)(void);

void CarCmdDoNothing() {
};

void CarCmdToggleHazard() {
	fprintf(stderr, "Toggle Hazard\n");
	wCarBox->LeftHazardLights(!wCarBox->LeftHazardLights());
	wCarBox->RightHazardLights(!wCarBox->RightHazardLights());
};

void ChangeAccel(Fl_Widget *w, void *data) {
	Fl_Slider * slider = reinterpret_cast<Fl_Slider *>(w);
	double value = (slider->maximum() - slider->value()) / slider->maximum();
	fprintf(stderr, "Accel %lf\n", value);

	wRpmDial->setDialValue(value);
};

void ChangeBrake(Fl_Widget *w, void *data) {
	Fl_Slider * slider = reinterpret_cast<Fl_Slider *>(w);
	double value = (slider->maximum() - slider->value()) / slider->maximum();
	fprintf(stderr, "Brake %lf\n", value);

	if (slider->maximum() != slider->value()) {
		wCarBox->BrakeLights(true);
	} else {
		wCarBox->BrakeLights(false);
	}
};

void ChangeSteeringWheel(Fl_Widget *w, void *data) {
	Fl_Dial * dial = reinterpret_cast<Fl_Dial *>(w);
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

int main(int argc, char * argv[]) {
	Fl_Double_Window * panel_window = makePanelWindow();

	wKeyState->add("Key Off",   0, cbKeyOff,   0, 0);
	wKeyState->add("Key On",    0, cbKeyOn,    0, 0);
	wKeyState->add("Engine On", 0, cbEngineOn, 0, 0);
	wKeyState->value(0);

	wCarCmdTree->root()->label("Commands");
	wCarCmdTree->selectmode(FL_TREE_SELECT_SINGLE);
	wCarCmdTree->begin();
	{
		Fl_Tree_Item *item;
		item = wCarCmdTree->add("Lights/Toggle Hazard");
		item->user_data((void*)CarCmdToggleHazard);
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
	wAccelSlider->callback(ChangeAccel);

	wBrakeSlider->scrollvalue(0 /*pos*/, 1 /*size*/, 0 /*first*/, 11 /*total*/);
	wBrakeSlider->value(wBrakeSlider->maximum());
	wBrakeSlider->callback(ChangeBrake);

	wSteeringWheel->angles(0, 360);
	wSteeringWheel->callback(ChangeSteeringWheel);
	wSteeringWheel->value(0.5f);

	wRpmDial->setSvgFilename("svg/dial_rpm.svg");
	wRG1Dial->setSvgFilename("svg/dial_rg1.svg");
	wOilWaterDial->setSvgFilename("svg/dial_ow.svg");

	panel_window->show(argc, argv);
	return Fl::run();
}
