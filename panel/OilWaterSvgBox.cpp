#include "OilWaterSvgBox.h"

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

OilWaterSvgBox::OilWaterSvgBox(int x, int y, int w, int h, const char * l) :
	AbstractSvgBox(x,y,w,h,l),
	svg_filename(NULL),
	max_angle( 120.0 * (M_PI/180.0) ),
	oil_dial_value(1.0),
	water_dial_value(1.0)
{
	if (NULL != svg_filename) {
		GError **crap = NULL;
		rsvg_handle = rsvg_handle_new_from_file(svg_filename, crap);
	}
}

OilWaterSvgBox::~OilWaterSvgBox() {
}

void OilWaterSvgBox::setSvgFilename(const gchar * filename) {
	if (rsvg_handle) {
		g_object_unref(rsvg_handle);
		rsvg_handle = NULL;
	}

	svg_filename = filename;

	if (NULL != svg_filename) {
		GError **crap = NULL;
		rsvg_handle = rsvg_handle_new_from_file(svg_filename, crap);
	}

	redraw();
}

void OilWaterSvgBox::setOilDialValue(double value) {
	if (value <= 0.0) {
		oil_dial_value = 0.0;
	} else if (value >= 1.0) {
		oil_dial_value = 1.0;
	} else {
		oil_dial_value = value;
	}
	redraw();
}

void OilWaterSvgBox::setWaterDialValue(double value) {
	if (value <= 0.0) {
		water_dial_value = 0.0;
	} else if (value >= 1.0) {
		water_dial_value = 1.0;
	} else {
		water_dial_value = value;
	}
	redraw();
}

void OilWaterSvgBox::graphic(cairo_t * cr, double x, double y, double w, double h) {
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

		rsvg_handle_render_cairo_sub(rsvg_handle, cr, "#layer_low");

		cairo_save(cr);
		RsvgPositionData pointer_position_a;
		if (rsvg_handle_get_position_sub(rsvg_handle, &pointer_position_a, "#pointer_a")) {
			double angle = max_angle * oil_dial_value;
			cairo_identity_matrix(cr);
			cairo_scale(cr, scale, scale);
			cairo_translate(cr,
				x / scale + pointer_position_a.x,
				y / scale + pointer_position_a.y
			);
			cairo_rotate(cr, angle);
			cairo_translate(cr,
				- pointer_position_a.x,
				- pointer_position_a.y
			);
			rsvg_handle_render_cairo_sub(rsvg_handle, cr, "#layer_main_a");
		}
		cairo_restore(cr);

		cairo_save(cr);
		RsvgPositionData pointer_position_b;
		if (rsvg_handle_get_position_sub(rsvg_handle, &pointer_position_b, "#pointer_b")) {
			double angle = - max_angle * water_dial_value;
			cairo_identity_matrix(cr);
			cairo_scale(cr, scale, scale);
			cairo_translate(cr,
				x / scale + pointer_position_b.x,
				y / scale + pointer_position_b.y
			);
			cairo_rotate(cr, angle);
			cairo_translate(cr,
				- pointer_position_b.x,
				- pointer_position_b.y
			);
			rsvg_handle_render_cairo_sub(rsvg_handle, cr, "#layer_main_b");
		}
		cairo_restore(cr);

		rsvg_handle_render_cairo_sub(rsvg_handle, cr, "#layer_high");
	}
}
