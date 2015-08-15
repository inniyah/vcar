#include "panel_cb.h"
#include "panel.h"

#include <FL/Fl_Choice.H>

#include <iostream>
#include <sstream>
#include <string>

void cbKeyOff (Fl_Widget *w, void *) {
}

void cbKeyOn (Fl_Widget *w, void *) {
}

void cbEngineOn (Fl_Widget *w, void *) {
}

static const int wpts = 175; // width in points
static const int hpts = 175; // height in points
static const int wpix = 175; // width in pixels
static const int hpix = 175; // height in pixels

void cbCarBoxToPng (Fl_Widget *, void *) {
	char filename[] = "pngtest.png";
	fprintf(stderr, "Output in %s\n", filename);
	wCarBox->exportToPNG(filename, wpix, hpix);
	return;
}

void cbCarBoxToSvg(Fl_Widget *, void *) {
	char filename[] = "svgtest.svg";
	fprintf(stderr, "Output in %s\n", filename);
	wCarBox->exportToSVG(filename, wpts, hpts);
	return;
}

void cbCarBoxToEps(Fl_Widget *, void *) {
	char filename[] = "epstest.eps";
	fprintf(stderr, "Output in %s\n", filename);
	wCarBox->exportToEPS(filename, wpts, hpts);
	return;
}

void cbCarBoxToPdf(Fl_Widget *, void *) {
	char filename[] = "pdftest.pdf";
	fprintf(stderr, "Output in %s\n", filename);
	wCarBox->exportToPDF(filename, wpts, hpts);
	return;
}

void cbCarBoxToCairoScript(Fl_Widget *, void *) {
	char filename[] = "cstest.cs";
	fprintf(stderr, "Output in %s\n", filename);
	wCarBox->exportToCS(filename, wpts, hpts);
	return;
}
