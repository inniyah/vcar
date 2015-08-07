#ifndef STATICWORLDOBJECT_H
#define STATICWORLDOBJECT_H

#include <string>
#include <vector>

#define _isnan isnan
#include <ode/ode.h>
#include <plib/ssg.h>

#include "worldobject.h"

class StaticWorldObject : public WorldObject
{
  public:
    StaticWorldObject(ssgEntity *model, dSpaceID bigspace, sgVec3 initialpos);
    virtual ~StaticWorldObject();
    void WalkTree(ssgEntity *e, sgVec3 initialpos);
    void AddLeaf(ssgLeaf *leaf, sgVec3 initialpos);

  protected:
    std::vector<dGeomID> geomids;
    std::vector<dTriMeshDataID> dataids;
    dSpaceID space;
};
#endif
