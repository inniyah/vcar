#ifndef CARPETTRACK_H
#define CARPETTRACK_H

#include <plib/ssg.h>

#include "staticworldobject.h"
#include "dynamicboxobject.h"

class CarpetTrack
{
  public:

    CarpetTrack(ssgEntity *carpetmodel, ssgEntity *trackmodel, dWorldID world, dSpaceID staticspace, sgVec3 initialpos)
    {
      trackspace = dSimpleSpaceCreate(staticspace);
      track  = new StaticWorldObject(trackmodel, trackspace, initialpos);
      sgVec3 sz = { 2.0, 1.2, 0.08 };
      sgVec3 cpos = { initialpos[0]+5, initialpos[1], initialpos[2]+0.6f };
      carpet = new DynamicBoxObject(carpetmodel, world, trackspace, cpos, sz);
      carpet->name = "carpet";
      carpet->SetMass(1.0);
      slider = dJointCreateSlider(world, 0);
      dJointAttach(slider, carpet->body, 0);
      dJointSetSliderAxis(slider, 1,0,0);
      dJointSetSliderParam(slider, dParamLoStop, 0.0);
      dJointSetSliderParam(slider, dParamHiStop, 4.0);
      branch = new ssgBranch();
      branch->addKid(track->GetEntity());
      branch->addKid(carpet->GetEntity());
    }

    ssgEntity *GetEntity(void) const { return branch; }

    ~CarpetTrack()
    {
      dJointDestroy(slider);
      delete track;
      delete carpet;
      dSpaceDestroy(trackspace);
    }

    void Sustain(float dt) 
    {
      carpet->Sustain(dt);
    }

  protected:

    dSpaceID            trackspace;
    dJointID            slider;
    StaticWorldObject  *track;
    DynamicBoxObject   *carpet;
    ssgBranch          *branch;
};
#endif
