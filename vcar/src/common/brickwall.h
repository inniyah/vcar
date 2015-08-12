#ifndef BRICKWALL_H
#define BRICKWALL_H

#include <plib/ssg.h>

#include "dynamicboxobject.h"

class BrickWall
{
  public:
    BrickWall(sgVec3 pos, int w, int h, dWorldID world, dSpaceID space, ssgEntity *model_full, ssgEntity *model_half)
    {
      int i;
      cnt=0;
      for (i=0; i<h; i++) cnt += (i&1)?w+1:w;
      branch = new ssgBranch();
      branch->setName("brickwall branch");
      boxes = new DynamicBoxObject* [cnt];
      MakeDisplayLists(model_half);
      MakeDisplayLists(model_full);
      float dy = 1.00;
      float dz = 0.50;
      i=0;
      sgVec3 boxscl_half;
      sgVec3 boxscl_full;
      sgVec3 boxpos;;
      sgSetVec3(boxscl_half, 0.5, 0.5, 0.5);
      sgSetVec3(boxscl_full, 0.5, 1.0, 0.5);
      for (int z=0; z<h; z++)
      {
        if (z&1)
        {
          float y0 = pos[1] - (0.5*w - 0.25)*dy;
          float y1 = pos[1] + (0.5*w - 0.25)*dy;
          sgSetVec3(boxpos, pos[0], y0, pos[2]+z*dz);
          boxes[i] = new DynamicBoxObject(model_half, world, space, boxpos, boxscl_half);
          boxes[i]->name = "halfbrick";
          branch->addKid(boxes[i]->GetEntity());
          i++;
          sgSetVec3(boxpos, pos[0], y1, pos[2]+z*dz);
          boxes[i] = new DynamicBoxObject(model_half, world, space, boxpos, boxscl_half);
          boxes[i]->name = "halfbrick";
          branch->addKid(boxes[i]->GetEntity());
          i++;
          for (int x=0; x<w-1; x++)
          {
            float y = pos[1]+(x-0.5*(w-2))*dy;
            sgSetVec3(boxpos, pos[0], y, pos[2]+z*dz);
            boxes[i] = new DynamicBoxObject(model_full, world, space, boxpos, boxscl_full);
            boxes[i]->name = "crate";
            branch->addKid(boxes[i]->GetEntity());
            i++;
          }
        }
        else
        {
          for (int x=0; x<w; x++)
          {
            float y=pos[1]+(x-0.5*(w-1))*dy;
            sgSetVec3(boxpos, pos[0], y, pos[2]+z*dz);
            boxes[i] = new DynamicBoxObject(model_full, world, space, boxpos, boxscl_full);
            boxes[i]->name = "crate";
            branch->addKid(boxes[i]->GetEntity());
            i++;
          }
        }
      }
      assert(i==cnt);
    }
    ~BrickWall()
    {
      delete [] boxes;
    }
    void Sustain(float dt)
    {
      for (int i=0; i<cnt; i++)
        boxes[i]->Sustain(dt);
    }
    void SetMass(float density)
    {
      for (int i=0; i<cnt; i++)
        boxes[i]->SetMass(density);
    }
    ssgEntity *GetEntity(void) { return branch; }
  protected:
    int cnt;
    DynamicBoxObject **boxes;
    ssgBranch *branch;
};

#endif
