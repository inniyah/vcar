#ifndef DYNAMICCYLOBJECT_H
#define DYNAMICCYLOBJECT_H

#include <string>

#include <ode/ode.h>
#include <plib/ssg.h>

#include "dynamicobject.h"


class DynamicCylObject : public DynamicObject
{
  public:
    DynamicCylObject(ssgEntity *model, dWorldID world, dSpaceID bigspace, sgVec3 pos, float radius, float length) :
    DynamicObject(model, world, bigspace)
    {
      body = dBodyCreate (world);
      dBodySetPosition (body,pos[0],pos[1],pos[2]);
      dQuaternion q;
      dQFromAxisAndAngle (q,1,0,0,M_PI*0.5);
      dBodySetQuaternion (body,q);
      dMass m;
      dMassSetSphere (&m, 1, radius);
//      dMassAdjust (&m,1.0);
      dBodySetMass (body,&m);
      geom = dCreateCylinder(bigspace, radius, length);
      dGeomSetBody (geom,body);
      dGeomSetData (geom,this);
    }
    virtual ~DynamicCylObject()
    { 
    }
};

#endif
