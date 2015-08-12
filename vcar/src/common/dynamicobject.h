#ifndef DYNAMICOBJECT_H
#define DYNAMICOBJECT_H

#include <string>

#include <ode/ode.h>
#include <plib/ssg.h>

#include "worldobject.h"


class DynamicObject : public WorldObject
{
  public:
    DynamicObject(ssgEntity *model, dWorldID world, dSpaceID bigspace);
    virtual ~DynamicObject();
    ssgTransform *GetTransform(void) { return trf; }
    virtual bool Sustain(float dt);

    ssgTransform *trf;
    dBodyID  body;
    dGeomID  geom;
};

#endif
