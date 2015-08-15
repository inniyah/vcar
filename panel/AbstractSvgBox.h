#ifndef ABSTRACTSVGBOX_H_
#define ABSTRACTSVGBOX_H_

#include <FL/Fl_Box.H>

#include <cairo.h>

#ifdef WIN32
#  include <cairo-win32.h>
#elif defined (__APPLE__)
#  include <cairo-quartz.h>
#else
#  include <cairo-xlib.h>
#endif

#include <librsvg/rsvg.h>

class AbstractSvgBox : public Fl_Box  {
public:
	AbstractSvgBox(int x, int y, int w, int h, const char * l = NULL);
	virtual ~AbstractSvgBox();

	virtual void graphic(cairo_t * cr, double, double, double, double) = 0;

	void exportToPNG (const char * filename, int, int);
	void exportToSVG (const char * filename, int, int);
	void exportToEPS (const char * filename, int, int);
	void exportToPDF (const char * filename, int, int);
	void exportToCS  (const char * filename, int, int);

protected:
	void draw(void);
	cairo_surface_t * set_surface(int wo, int ho);

	cairo_t         * cr;
	cairo_surface_t * surface;
	RsvgHandle      * rsvg_handle;
};

#endif // ABSTRACTSVGBOX_H_

