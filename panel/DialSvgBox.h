#ifndef DIALSVGBOX_H_
#define DIALSVGBOX_H_

#include "AbstractSvgBox.h"

class DialSvgBox : public AbstractSvgBox  {
public:
	DialSvgBox(int x, int y, int w, int h, const char * l = NULL);
	virtual ~DialSvgBox();

	virtual void graphic(cairo_t * cr, double, double, double, double);

	void setSvgFilename(const gchar * filename);
	void setDialValue(double value);

private:
	const gchar     * svg_filename;

	double          max_angle;  /* in radians */
	double          dial_value; /* between 0.0 and 1.0 */
};

#endif // CARSVGBOX_H_

