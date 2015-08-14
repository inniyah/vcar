// generated by Fast Light User Interface Designer (fluid) version 1.0302

#ifndef panel_h
#define panel_h
#include <FL/Fl.H>
#include <FL/Fl_Double_Window.H>
#include "config.h"
#include "panel_cb.h"
#include <FL/Fl_Light_Button.H>
extern Fl_Light_Button *wKeyOn;
extern Fl_Light_Button *wEngineOn;
#include <FL/Fl_Choice.H>
extern Fl_Choice *wKeyState;
#include "CarSvgBox.h"
extern CarSvgBox *wCarBox;
#include "CmdTree.h"
extern CmdTree *wCarCmdTree;
#include <FL/Fl_Slider.H>
extern Fl_Slider *wBrakeSlider;
extern Fl_Slider *wAccelSlider;
Fl_Double_Window* makePanelWindow();
#endif
