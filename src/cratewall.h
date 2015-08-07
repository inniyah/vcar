#ifndef CRATEWALL_H
#define CRATEWALL_H

#include <plib/ssg.h>

#include "dynamicboxobject.h"

class CrateWall
{
  public:
    CrateWall(sgVec3 pos, int w, int h, dWorldID world, dSpaceID space, ssgEntity *model) :
      cnt(w*h)
    {
      branch = new ssgBranch();
      branch->setName("cratewall branch");
      boxes = new DynamicBoxObject* [cnt];
      MakeDisplayLists(model);
      float dx = 0.55;
      float dz = 0.40;
      int i=0;
      for (int z=0; z<h; z++)
      {
        for (int x=0; x<w; x++)
        {
          sgVec3 boxpos, boxscl;
//          sgSetVec3(boxpos, pos[0]+(x-0.5*(w-1))*dx, pos[1], pos[2]+z*dz);
          sgSetVec3(boxpos, pos[0], pos[1]+(x-0.5*(w-1))*dx, pos[2]+z*dz);
          boxpos[0] += 0.0006*((rand()%128)-64.0);
          boxpos[1] += 0.0006*((rand()%128)-64.0);
          sgSetVec3(boxscl,0.45,0.45,0.4);
          boxes[i] = new DynamicBoxObject(model, world, space, boxpos, boxscl);
          boxes[i]->name = "crate";
          boxes[i]->SetMass(1.0);
          branch->addKid(boxes[i]->GetEntity());
          i++;
        }
      }
    }
    ~CrateWall()
    {
      delete [] boxes;
    }
    void Sustain(float dt)
    {
      for (int i=0; i<cnt; i++)
        boxes[i]->Sustain(dt);
    }
    ssgEntity *GetEntity(void) { return branch; }
  protected:
    int cnt;
    DynamicBoxObject **boxes;
    ssgBranch *branch;
};

#endif
