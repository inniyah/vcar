#ifndef PANEL_CB_H_
#define PANEL_CB_H_

#include "config.h"
#include <FL/Fl.H>

extern void cbKeyOff   (Fl_Widget *, void *);
extern void cbKeyOn    (Fl_Widget *, void *);
extern void cbEngineOn (Fl_Widget *, void *);

extern void cbCarBoxToPng (Fl_Widget *, void *);
extern void cbCarBoxToSvg (Fl_Widget *, void *);
extern void cbCarBoxToEps (Fl_Widget *, void *);
extern void cbCarBoxToPdf (Fl_Widget *, void *);
extern void cbCarBoxToCairoScript (Fl_Widget *, void *);

#endif // PANEL_CB_H_

