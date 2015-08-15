#include "CarSvgBox.h"

#include <FL/fl_draw.H>

#include <cairo.h>

#ifdef WIN32
#  include <cairo-win32.h>
#elif defined (__APPLE__)
#  include <cairo-quartz.h>
#else
#  include <cairo-xlib.h>
#endif

#include <cairo-svg.h>
#include <cairo-ps.h>
#include <cairo-pdf.h>
#include <cairo-script.h>

#include <librsvg/rsvg.h>

#include <cmath>

CarSvgBox::CarSvgBox(int x, int y, int w, int h, const char * l) :
	AbstractSvgBox(x,y,w,h,l),
	flags(0)
{
	const gchar * filename = "svg/BMW_Z4.svg";
	GError **crap = NULL;
	rsvg_handle = rsvg_handle_new_from_file(filename, crap);
	flags |= FlagLeftDoorClosed;
	flags |= FlagRightDoorClosed;
}

CarSvgBox::~CarSvgBox() {
}

void CarSvgBox::setFLag(unsigned long f, bool v) {
	bool p = (0 != (flags & f));
	if (v) {
		flags |= f;
	} else {
		flags &= ~f;
	}

	if (p != v) {
		redraw();
	}
}

void CarSvgBox::graphic(cairo_t * cr, double x, double y, double w, double h) {
	if (rsvg_handle) {
		RsvgDimensionData dimension_data;
		rsvg_handle_get_dimensions (rsvg_handle, &dimension_data);
		double scale_w = w / dimension_data.width;
		double scale_h = h / dimension_data.height;
		double scale = scale_w;
		if (scale_h < scale_w) {
			scale = scale_h;
		}

		cairo_identity_matrix(cr);
		cairo_translate(cr,
			x + w/2.0 - dimension_data.width * scale / 2.0,
			y + h/2.0 - dimension_data.height * scale / 2.0
		);
		cairo_scale(cr, scale, scale);

		rsvg_handle_render_cairo_sub(rsvg_handle, cr, "#layer_car_lower");

		if (flags & FlagLeftDoorClosed) {
			rsvg_handle_render_cairo_sub(rsvg_handle, cr, "#layer_leftdoor_closed");
		} else {
			rsvg_handle_render_cairo_sub(rsvg_handle, cr, "#layer_leftdoor_open");
		}
		if (flags & FlagRightDoorClosed) {
			rsvg_handle_render_cairo_sub(rsvg_handle, cr, "#layer_rightdoor_closed");
		} else {
			rsvg_handle_render_cairo_sub(rsvg_handle, cr, "#layer_rightdoor_open");
		}

		rsvg_handle_render_cairo_sub(rsvg_handle, cr, "#layer_car_upper");

		if (flags & FlagRoofClosed) {
			rsvg_handle_render_cairo_sub(rsvg_handle, cr, "#layer_top");
		}
		if (flags & FlagBrakeLights) {
			rsvg_handle_render_cairo_sub(rsvg_handle, cr, "#layer_brake_lights");
		}
		if (flags & FlagBackwardsLights) {
			rsvg_handle_render_cairo_sub(rsvg_handle, cr, "#layer_backwards_lights");
		}
		if (flags & FlagLeftHazardLights) {
			rsvg_handle_render_cairo_sub(rsvg_handle, cr, "#layer_hazard_left");
		}
		if (flags & FlagRightHazardLights) {
			rsvg_handle_render_cairo_sub(rsvg_handle, cr, "#layer_hazard_right");
		}
	}
}
