#include "panel.h"
#include "Fl_CairoBox.h"

#include <stdlib.h>
#include <stdio.h>

#include <FL/Fl.H>

int main(int argc, char * argv[]) {
	Fl_Double_Window * panel_window = makePanelWindow();
	wKeyState->add("Key Off",   0, cbKeyOff,   0, 0);
	wKeyState->add("Key On",    0, cbKeyOn,    0, 0);
	wKeyState->add("Engine On", 0, cbEngineOn, 0, 0);
	wKeyState->value(0);
	panel_window->show(argc, argv);
	return Fl::run();
}
