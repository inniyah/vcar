#ifndef DYNAMICBOXOBJECT_H
#define DYNAMICBOXOBJECT_H

#include <string>

#include <ode/ode.h>
#include <plib/ssg.h>

#include "dynamicobject.h"


class DynamicBoxObject : public DynamicObject
{
  public:
    DynamicBoxObject(ssgEntity *model, dWorldID world, dSpaceID bigspace, sgVec3 pos, sgVec3 scl) :
    DynamicObject(model, world, bigspace)
    {
      body = dBodyCreate (world);
      dBodySetPosition (body,pos[0],pos[1],pos[2]);
      dim[0]=scl[0]; dim[1]=scl[1]; dim[2]=scl[2];
      geom = dCreateBox (bigspace,scl[0],scl[1],scl[2]);
      SetMass();
      dGeomSetBody (geom,body);
      dGeomSetData (geom,this);
    }
    void SetMass(float density=1000.0)
    {
      dMass m;
      dMassSetBox (&m,density,dim[0],dim[1],dim[2]);
      dBodySetMass (body,&m);
    }
    virtual ~DynamicBoxObject()
    { 
      // geom and body are destroyed by parent's destructor
    }
  protected:
    dReal dim[3];
};

#endif
