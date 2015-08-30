#ifndef CARSVGBOX_H_
#define CARSVGBOX_H_

#include "AbstractSvgBox.h"

class CarSvgBox : public AbstractSvgBox  {
public:
	CarSvgBox(int x, int y, int w, int h, const char * l = NULL);
	virtual ~CarSvgBox();

	virtual void graphic(cairo_t * cr, double, double, double, double);

	bool  RoofClosed()               { return (0 != (flags & FlagRoofClosed));        }
	void  RoofClosed(bool v)         { setFLag(FlagRoofClosed, v);      redraw();     }
	bool  LeftDoorClosed()           { return (0 != (flags & FlagLeftDoorClosed));    }
	void  LeftDoorClosed(bool v)     { setFLag(FlagLeftDoorClosed, v);  redraw();     }
	bool  RightDoorClosed()          { return (0 != (flags & FlagRightDoorClosed));   }
	void  RightDoorClosed(bool v)    { setFLag(FlagRightDoorClosed, v); redraw();     }

	float BrakeLights()              { return brake_lights;                                                  }
	void  BrakeLights(float v)       { brake_lights = ( v < 0.0 ? 0.0 : (v > 1.0 ? 1.0 : v) );     redraw(); }
	float BackwardsLights()          { return backwards_lights;                                              }
	void  BackwardsLights(float v)   { backwards_lights = ( v < 0.0 ? 0.0 : (v > 1.0 ? 1.0 : v) ); redraw(); }
	float LeftHazardLights()         { return left_hazard;                                                   }
	void  LeftHazardLights(float v)  { left_hazard = ( v < 0.0 ? 0.0 : (v > 1.0 ? 1.0 : v) );      redraw(); }
	float RightHazardLights()        { return right_hazard;                                                  }
	void  RightHazardLights(float v) { right_hazard = ( v < 0.0 ? 0.0 : (v > 1.0 ? 1.0 : v) );     redraw(); }
	float InteriorLights()           { return interior_lights;                                               }
	void  InteriorLights(float v)    { interior_lights = ( v < 0.0 ? 0.0 : (v > 1.0 ? 1.0 : v) );  redraw(); }

private:
	void setFLag(unsigned long f, bool v = true);

	static const long int FlagRoofClosed        = 1 << 0;
	static const long int FlagLeftDoorClosed    = 1 << 1;
	static const long int FlagRightDoorClosed   = 1 << 2;

	unsigned long flags;

	float brake_lights;
	float backwards_lights;
	float left_hazard;
	float right_hazard;
	float interior_lights;
};

#endif // CARSVGBOX_H_

