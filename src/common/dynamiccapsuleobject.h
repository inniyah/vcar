#ifndef DYNAMICAPSULEOBJECT_H
#define DYNAMICAPSULEOBJECT_H

#include <string>

#include <ode/ode.h>
#include <plib/ssg.h>

#include "dynamicobject.h"


class DynamicCapsuleObject : public DynamicObject
{
  public:
    DynamicCapsuleObject(ssgEntity *model, dWorldID world, dSpaceID bigspace, const dReal *pos, float radius, float length) :
    DynamicObject(model, world, bigspace)
    {
      body = dBodyCreate (world);
      dMass m;
      dMassSetCapsule(&m, 1000, 1, radius, length);
      dBodySetMass (body,&m);
      geom = dCreateCapsule(bigspace, radius, length);
      dGeomSetBody (geom,body);
      dGeomSetData (geom,this);
      dQuaternion q;
      dQFromAxisAndAngle(q,0,1,0,M_PI*0.5);
      dGeomSetOffsetQuaternion(geom, q);
      dBodySetPosition (body,pos[0],pos[1],pos[2]);
      name="unnamed capsule object";
    }
    virtual ~DynamicCapsuleObject()
    { 
    }
};

#endif
