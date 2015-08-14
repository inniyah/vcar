// generated by Fast Light User Interface Designer (fluid) version 1.0303

#include "panel.h"

Fl_Light_Button *wKeyOn=(Fl_Light_Button *)0;

Fl_Light_Button *wEngineOn=(Fl_Light_Button *)0;

Fl_Choice *wKeyState=(Fl_Choice *)0;

CarSvgBox *wCarBox=(CarSvgBox *)0;

CmdTree *wCarCmdTree=(CmdTree *)0;

Fl_Slider *wBrakeSlider=(Fl_Slider *)0;

Fl_Slider *wAccelSlider=(Fl_Slider *)0;

Fl_Dial *wSteeringWheel=(Fl_Dial *)0;

Fl_Group *wExportButtons=(Fl_Group *)0;

Fl_Button *wToPngButton=(Fl_Button *)0;

Fl_Button *wToSvgButton=(Fl_Button *)0;

Fl_Button *wToEpsButton=(Fl_Button *)0;

Fl_Button *wToPdfButton=(Fl_Button *)0;

Fl_Button *wToScriptButton=(Fl_Button *)0;

Fl_Double_Window* makePanelWindow() {
  Fl_Double_Window* w;
  { Fl_Double_Window* o = new Fl_Double_Window(750, 550);
    w = o;
    { wKeyOn = new Fl_Light_Button(5, 10, 100, 35, "Key On");
    } // Fl_Light_Button* wKeyOn
    { wEngineOn = new Fl_Light_Button(5, 45, 100, 35, "Engine On");
    } // Fl_Light_Button* wEngineOn
    { wKeyState = new Fl_Choice(255, 15, 110, 20, "Key State");
      wKeyState->down_box(FL_BORDER_BOX);
    } // Fl_Choice* wKeyState
    { wCarBox = new CarSvgBox(495, 7, 250, 382, "Car");
      wCarBox->box(FL_NO_BOX);
      wCarBox->color(FL_BACKGROUND_COLOR);
      wCarBox->selection_color(FL_BACKGROUND_COLOR);
      wCarBox->labeltype(FL_NORMAL_LABEL);
      wCarBox->labelfont(0);
      wCarBox->labelsize(14);
      wCarBox->labelcolor(FL_FOREGROUND_COLOR);
      wCarBox->align(Fl_Align(FL_ALIGN_CENTER));
      wCarBox->when(FL_WHEN_RELEASE);
    } // CarSvgBox* wCarBox
    { wCarCmdTree = new CmdTree(495, 415, 250, 130);
      wCarCmdTree->box(FL_DOWN_BOX);
      wCarCmdTree->color(FL_BACKGROUND2_COLOR);
      wCarCmdTree->selection_color(FL_SELECTION_COLOR);
      wCarCmdTree->labeltype(FL_NORMAL_LABEL);
      wCarCmdTree->labelfont(0);
      wCarCmdTree->labelsize(14);
      wCarCmdTree->labelcolor(FL_FOREGROUND_COLOR);
      wCarCmdTree->align(Fl_Align(FL_ALIGN_TOP));
      wCarCmdTree->when(FL_WHEN_CHANGED);
    } // CmdTree* wCarCmdTree
    { wBrakeSlider = new Fl_Slider(15, 425, 25, 90, "Brake");
      wBrakeSlider->type(4);
    } // Fl_Slider* wBrakeSlider
    { wAccelSlider = new Fl_Slider(60, 425, 25, 90, "Accel");
      wAccelSlider->type(4);
    } // Fl_Slider* wAccelSlider
    { wSteeringWheel = new Fl_Dial(15, 345, 70, 70);
    } // Fl_Dial* wSteeringWheel
    { wExportButtons = new Fl_Group(495, 392, 250, 20);
      wExportButtons->labeltype(FL_NO_LABEL);
      wExportButtons->align(Fl_Align(FL_ALIGN_LEFT));
      { wToPngButton = new Fl_Button(545, 392, 50, 20, "PNG");
      } // Fl_Button* wToPngButton
      { wToSvgButton = new Fl_Button(495, 392, 50, 20, "SVG");
      } // Fl_Button* wToSvgButton
      { wToEpsButton = new Fl_Button(595, 392, 50, 20, "EPS");
      } // Fl_Button* wToEpsButton
      { wToPdfButton = new Fl_Button(645, 392, 50, 20, "PDF");
      } // Fl_Button* wToPdfButton
      { wToScriptButton = new Fl_Button(695, 392, 50, 20, "CS");
      } // Fl_Button* wToScriptButton
      wExportButtons->end();
    } // Fl_Group* wExportButtons
    o->end();
  } // Fl_Double_Window* o
  return w;
}
