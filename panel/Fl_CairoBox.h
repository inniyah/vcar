#ifndef FL_CAIROBOX_H_
#define FL_CAIROBOX_H_

#include <FL/Fl_Box.H>
#include <cairo.h>

#ifdef WIN32
#  include <cairo-win32.h>
#elif defined (__APPLE__)
#  include <cairo-quartz.h>
#else
#  include <cairo-xlib.h>
#endif

class Fl_CairoBox : public Fl_Box  {
public:
	Fl_CairoBox(int x, int y, int w, int h, const char * l = NULL);

	virtual void graphic(cairo_t * cr, double, double, double, double);

	void exportToPNG(const char * filename, int, int);
	void exportToSVG(const char * filename, int, int);
	void exportToEPS(const char * filename, int, int);
	void exportToPDF(const char * filename, int, int);

private:
	void draw(void);

	cairo_t         * cr;
	cairo_surface_t * surface;
	cairo_surface_t * set_surface(int wo, int ho);
};

#endif // FL_CAIROBOX_H_

