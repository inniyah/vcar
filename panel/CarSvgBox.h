#ifndef CARSVGBOX_H_
#define CARSVGBOX_H_

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

class CarSvgBox : public Fl_Box  {
public:
	CarSvgBox(int x, int y, int w, int h, const char * l = NULL);
	virtual ~CarSvgBox();

	virtual void graphic(cairo_t * cr, double, double, double, double);

	void exportToPNG(const char * filename, int, int);
	void exportToSVG(const char * filename, int, int);
	void exportToEPS(const char * filename, int, int);
	void exportToPDF(const char * filename, int, int);

	bool RoofClosed()              { return (0 != (flags & FlagRoofClosed)); }
	void RoofClosed(bool v)        { setFLag(FlagRoofClosed, v);             }
	bool LeftDoorClosed()          { return (0 != (flags & FlagLeftDoorClosed)); }
	void LeftDoorClosed(bool v)    { setFLag(FlagLeftDoorClosed, v);             }
	bool RightDoorClosed()         { return (0 != (flags & FlagRightDoorClosed)); }
	void RightDoorClosed(bool v)   { setFLag(FlagRightDoorClosed, v);             }
	bool BrakeLights()             { return (0 != (flags & FlagBrakeLights)); }
	void BrakeLights(bool v)       { setFLag(FlagBrakeLights, v);             }
	bool LeftHazardLights()        { return (0 != (flags & FlagLeftHazardLights));  }
	void LeftHazardLights(bool v)  { setFLag(FlagLeftHazardLights, v);              }
	bool RightHazardLights()       { return (0 != (flags & FlagRightHazardLights)); }
	void RightHazardLights(bool v) { setFLag(FlagRightHazardLights, v);             }

private:
	void draw(void);
	cairo_surface_t * set_surface(int wo, int ho);
	void setFLag(unsigned long f, bool v = true);

	cairo_t         * cr;
	cairo_surface_t * surface;
	RsvgHandle      * rsvg_handle;

	static const long int FlagRoofClosed        = 1 << 0;
	static const long int FlagLeftDoorClosed    = 1 << 1;
	static const long int FlagRightDoorClosed   = 1 << 2;
	static const long int FlagBrakeLights       = 1 << 3;
	static const long int FlagLeftHazardLights  = 1 << 4;
	static const long int FlagRightHazardLights = 1 << 5;

	unsigned long flags;
};

#endif // CARSVGBOX_H_

