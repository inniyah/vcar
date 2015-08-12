#ifndef SPIKEJUMP_H
#define SPIKEJUMP_H

#include <plib/ssg.h>

#include "staticworldobject.h"
#include "dynamicboxobject.h"

#define IMPULSE_FRAMES 6

class SpikeJump
{
  public:

    SpikeJump(ssgEntity *boardmodel, ssgEntity *spikemodel, dWorldID world, dSpaceID staticspace, sgVec3 initialpos)
    {
      spikespace = dSimpleSpaceCreate(staticspace);
      spikegate = new StaticWorldObject(spikemodel, spikespace, initialpos);
      sgVec3 sz = { 2.0, 1.2, 0.08 };
      sgVec3 cpos = { initialpos[0], initialpos[1], initialpos[2]+0.6f };
      jumpboard = new DynamicBoxObject(boardmodel, world, spikespace, cpos, sz);
      jumpboard->name = "jumpboard";
      jumpboard->SetMass(1.0);
      dBodySetAutoDisableFlag(jumpboard->body, false);
      slider = dJointCreateSlider(world, 0);
      dJointAttach(slider, jumpboard->body, 0);
      dJointSetSliderAxis(slider, 0,0,1);
      dJointSetSliderParam(slider, dParamLoStop, 0.0);
      dJointSetSliderParam(slider, dParamHiStop, 0.5);
      dJointSetSliderParam(slider, dParamVel,  0.00);
      dJointSetSliderParam(slider, dParamFMax, 0.08);
      branch = new ssgBranch();
      branch->addKid(spikegate->GetEntity());
      branch->addKid(jumpboard->GetEntity());
      rechargedelay = 0.0;
      impulseframes = 0;
    }

    ssgEntity *GetEntity(void) const { return branch; }

    ~SpikeJump()
    {
      dJointDestroy(slider);
      delete spikegate;
      delete jumpboard;
      dSpaceDestroy(spikespace);
    }

    bool Boost(float dt)
    {
      if (rechargedelay>0) return false;
      impulseframes = IMPULSE_FRAMES;
      rechargedelay = 1.4;
      return true;
    }

    void Sustain(float dt) 
    {
      if (impulseframes)
      {
        // apply boost
        float magnitude = (26.0 / IMPULSE_FRAMES) / dt;
        dBodyAddForce(jumpboard->body, 0,0,magnitude);
        impulseframes--;
      }
      jumpboard->Sustain(dt);
      if (rechargedelay>0)
        rechargedelay -= dt;
    }

  protected:

    dSpaceID            spikespace;
    dJointID            slider;
    StaticWorldObject  *spikegate;
    DynamicBoxObject   *jumpboard;
    ssgBranch          *branch;
    float               rechargedelay;
    int                 impulseframes;
};
#endif
