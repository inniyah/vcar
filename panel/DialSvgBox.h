#ifndef DIALSVGBOX_H_
#define DIALSVGBOX_H_

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

class DialSvgBox : public Fl_Box  {
public:
	DialSvgBox(int x, int y, int w, int h, const char * l = NULL);
	virtual ~DialSvgBox();

	virtual void graphic(cairo_t * cr, double, double, double, double);

	void exportToPNG (const char * filename, int, int);
	void exportToSVG (const char * filename, int, int);
	void exportToEPS (const char * filename, int, int);
	void exportToPDF (const char * filename, int, int);
	void exportToCS  (const char * filename, int, int);

	void setSvgFilename(const gchar * filename);
	void setDialValue(double value);

private:
	void draw(void);
	cairo_surface_t * set_surface(int wo, int ho);

	const gchar     * svg_filename;

	cairo_t         * cr;
	cairo_surface_t * surface;
	RsvgHandle      * rsvg_handle;

	double          max_angle;  /* in radians */
	double          dial_value; /* between 0.0 and 1.0 */
};

#endif // CARSVGBOX_H_

