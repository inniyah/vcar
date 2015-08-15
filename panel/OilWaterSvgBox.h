#ifndef OILWATERSVGBOX_H_
#define OILWATERSVGBOX_H_

#include "AbstractSvgBox.h"

class OilWaterSvgBox : public AbstractSvgBox  {
public:
	OilWaterSvgBox(int x, int y, int w, int h, const char * l = NULL);
	virtual ~OilWaterSvgBox();

	virtual void graphic(cairo_t * cr, double, double, double, double);

	void setSvgFilename(const gchar * filename);
	void setOilDialValue(double value);
	void setWaterDialValue(double value);

private:
	const gchar     * svg_filename;

	double          max_angle;  /* in radians */
	double          oil_dial_value; /* between 0.0 and 1.0 */
	double          water_dial_value; /* between 0.0 and 1.0 */
};

#endif // OILWATERSVGBOX_H_

