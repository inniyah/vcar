/*
 * sturmbahnfahrer
 * (c) 2006 by Bram Stolk
 * bram at gmail.com
 * LICENSED ACCORDING TO THE GPLV3
 */

#include "dynamicobject.h"


DynamicObject::DynamicObject
(
  ssgEntity *model,
  dWorldID world, 
  dSpaceID bigspace
) :
WorldObject(model),
body(0),
geom(0)
{
  // load visible model as plib entity
  trf = new ssgTransform();
  trf->addKid(model);
  name = "unnamed dynamicobject";
  entity = trf;
}


DynamicObject::~DynamicObject()
{
  if (geom) dGeomDestroy(geom);
  if (body) dBodyDestroy(body);

  trf->removeAllKids();
  ssgBranch *parent;
  do
  {
    parent = trf->getParent(0);
    if (parent) parent->removeKid(trf);
  } while(parent);
  // trf is autodeleted because of refcnt
  trf=0;
}


bool DynamicObject::Sustain(float dt)
{
  SetTransformFromBody(trf, body);
  return true;
}

