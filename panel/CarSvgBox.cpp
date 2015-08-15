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
	Fl_Box(x,y,w,h,l),
	cr(NULL),
	surface(NULL),
	rsvg_handle(NULL),
	flags(0)
{
	const gchar * filename = "svg/BMW_Z4.svg";
	GError **crap = NULL;
	rsvg_handle = rsvg_handle_new_from_file(filename, crap);
	flags |= FlagLeftDoorClosed;
	flags |= FlagRightDoorClosed;
}

CarSvgBox::~CarSvgBox() {
	if (rsvg_handle) {
		g_object_unref(rsvg_handle);
		rsvg_handle = NULL;
	}
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

cairo_surface_t * CarSvgBox::set_surface(int wo, int ho) {
#ifdef WIN32
#warning win32 mode
	/* Get a Cairo surface for the current DC */
	HDC dc = fl_gc;                                     /* Exported by fltk */
	return cairo_win32_surface_create(dc);
#elif defined (__APPLE__)
#warning Apple Quartz mode
	/* Get a Cairo surface for the current CG context */
	CGContext *ctx = fl_gc;
	return cairo_quartz_surface_create_for_cg_context(ctx, wo, ho);
#else
	/* Get a Cairo surface for the current display */
	return cairo_xlib_surface_create(fl_display, fl_window, fl_visual->visual, wo, ho);
#endif
}

void CarSvgBox::exportToPNG (const char* filename, int wpix, int hpix) {
  cairo_surface_t * tmp_surface = cairo_image_surface_create (CAIRO_FORMAT_ARGB32, wpix, hpix);
  cairo_t         * tmp_cr      = cairo_create (tmp_surface);
  cairo_translate(tmp_cr, 0.5, 0.5);       // for anti-aliasing
  cairo_set_source_rgb (tmp_cr, 0, 0, 0);  // drawing color set to black
  graphic(tmp_cr, 0, 0, wpix, hpix); 
  cairo_surface_write_to_png (tmp_surface, filename);
  cairo_destroy (tmp_cr);
  cairo_surface_destroy (tmp_surface);
}

void CarSvgBox::exportToSVG (const char* filename, int wpts, int hpts) {
  cairo_surface_t * tmp_surface = cairo_svg_surface_create (filename, wpts, hpts);
  cairo_t         * tmp_cr      = cairo_create (tmp_surface);
  cairo_set_source_rgb (tmp_cr, 0, 0, 0);
  graphic(tmp_cr, 0, 0, wpts, hpts);
  cairo_destroy (tmp_cr);
  cairo_surface_destroy (tmp_surface);
}

void CarSvgBox::exportToEPS (const char* filename, int wpts, int hpts) {
	cairo_surface_t * tmp_surface = cairo_ps_surface_create (filename, wpts, hpts);
	cairo_ps_surface_set_eps(tmp_surface, 1);
	cairo_t         * tmp_cr      = cairo_create (tmp_surface);
	cairo_set_source_rgb (tmp_cr, 0, 0, 0);
	graphic(tmp_cr, 0, 0, wpts, hpts);
	cairo_show_page (tmp_cr);
	cairo_destroy (tmp_cr);
	cairo_surface_destroy (tmp_surface);
}

void CarSvgBox::exportToPDF (const char* filename, int wpts, int hpts) {
	cairo_surface_t * tmp_surface = cairo_pdf_surface_create(filename, wpts, hpts);
	cairo_t         * tmp_cr      = cairo_create (tmp_surface);
	cairo_set_source_rgb (tmp_cr, 0, 0, 0);
	graphic(tmp_cr, 0, 0, wpts, hpts);
	cairo_show_page(tmp_cr);
	cairo_destroy (tmp_cr);
	cairo_surface_destroy(tmp_surface);
}

void CarSvgBox::exportToCS (const char * filename, int wpts, int hpts) {
	cairo_device_t  * tmp_device  = cairo_script_create(filename);
	cairo_surface_t * tmp_surface = cairo_script_surface_create(tmp_device, cairo_surface_get_content (surface), wpts, hpts);
	cairo_t         * tmp_cr      = cairo_create (tmp_surface);
	cairo_set_source_rgb (tmp_cr, 0, 0, 0);
	graphic(tmp_cr, 0, 0, wpts, hpts);
	cairo_show_page(tmp_cr);
	cairo_destroy (tmp_cr);
	cairo_surface_destroy(tmp_surface);
	cairo_device_destroy(tmp_device);
}

void CarSvgBox::draw(void) {
	// using fltk functions, set up white background with thin black frame
	fl_push_no_clip(); /* remove any clipping region set by the expose events... */
	fl_push_clip(x(), y(), w(), h());

	fl_color(parent()->color());
	fl_rectf(x(), y(), w(), h());
	//fl_color(FL_BLACK);
	//fl_rect(x(), y(), w(), h());

	// set up cairo structures
	surface = set_surface(window()->w(), window()->h());
	cr      = cairo_create(surface);
	/* All Cairo co-ordinates are shifted by 0.5 pixels to correct anti-aliasing */
	cairo_translate(cr, 0.5, 0.5);
	cairo_set_source_rgb(cr, 0.0, 0.0, 0.0); // set drawing color to black
	cairo_new_path(cr);

	// virtual function
	graphic(cr, x(), y(), w(), h());

	// release the cairo context
	cairo_destroy(cr);
	cairo_surface_destroy(surface);
	cr = NULL;

	// remove clip regions
	fl_pop_clip(); // local clip region
	fl_pop_clip(); // "no_clip" region
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
		if (flags & FlagLeftHazardLights) {
			rsvg_handle_render_cairo_sub(rsvg_handle, cr, "#layer_hazard_left");
		}
		if (flags & FlagRightHazardLights) {
			rsvg_handle_render_cairo_sub(rsvg_handle, cr, "#layer_hazard_right");
		}
	}
}
