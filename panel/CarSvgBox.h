#ifndef CARSVGBOX_H_
#define CARSVGBOX_H_

#include "AbstractSvgBox.h"

class CarSvgBox : public AbstractSvgBox  {
public:
	CarSvgBox(int x, int y, int w, int h, const char * l = NULL);
	virtual ~CarSvgBox();

	virtual void graphic(cairo_t * cr, double, double, double, double);

	bool RoofClosed()              { return (0 != (flags & FlagRoofClosed));        }
	void RoofClosed(bool v)        { setFLag(FlagRoofClosed, v);                    }
	bool LeftDoorClosed()          { return (0 != (flags & FlagLeftDoorClosed));    }
	void LeftDoorClosed(bool v)    { setFLag(FlagLeftDoorClosed, v);                }
	bool RightDoorClosed()         { return (0 != (flags & FlagRightDoorClosed));   }
	void RightDoorClosed(bool v)   { setFLag(FlagRightDoorClosed, v);               }
	bool BrakeLights()             { return (0 != (flags & FlagBrakeLights));       }
	void BrakeLights(bool v)       { setFLag(FlagBrakeLights, v);                   }
	bool BackwardsLights()         { return (0 != (flags & FlagBackwardsLights));   }
	void BackwardsLights(bool v)   { setFLag(FlagBackwardsLights, v);               }
	bool LeftHazardLights()        { return (0 != (flags & FlagLeftHazardLights));  }
	void LeftHazardLights(bool v)  { setFLag(FlagLeftHazardLights, v);              }
	bool RightHazardLights()       { return (0 != (flags & FlagRightHazardLights)); }
	void RightHazardLights(bool v) { setFLag(FlagRightHazardLights, v);             }

private:
	void setFLag(unsigned long f, bool v = true);

	static const long int FlagRoofClosed        = 1 << 0;
	static const long int FlagLeftDoorClosed    = 1 << 1;
	static const long int FlagRightDoorClosed   = 1 << 2;
	static const long int FlagBrakeLights       = 1 << 3;
	static const long int FlagBackwardsLights   = 1 << 4;
	static const long int FlagLeftHazardLights  = 1 << 5;
	static const long int FlagRightHazardLights = 1 << 6;

	unsigned long flags;
};

#endif // CARSVGBOX_H_

