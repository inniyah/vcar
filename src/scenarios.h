#ifndef SCENARIOS_H
#define SCENARIOS_H

#include <plib/ssg.h>
#include <plib/ssgaBillboards.h>


#include "cratewall.h"
#include "brickwall.h"
#include "doorstand.h"
#include "carpettrack.h"
#include "spikejump.h"
#include "ferriswheel.h"
#include "turntable.h"
#include "respawnpoint.h"


class PracticeScenario
{
  public :

    PracticeScenario(dWorldID world, dSpaceID bigspace, dSpaceID staticspace, ssgRoot *scene, ModelMap *modelmap, const std::string &prefix)
    {
      if (!modelmap->LowDetail())
        scene->addKid(modelmap->Get("starsky"));

      sgVec3 gridpos={0,0,0};
      grid = new StaticWorldObject(modelmap->Get("grid.3ds"), staticspace, gridpos);
      scene->addKid(grid->GetEntity());

      sgVec3 ramppos={-5,6,0};
      ramp[0] = new StaticWorldObject(modelmap->Get("ramp.3ds"), staticspace, ramppos);
      scene->addKid(ramp[0]->GetEntity());
      sgSetVec3(ramppos, -5,-4,0);
      ramp[1] = new StaticWorldObject(modelmap->Get("ramp.3ds"), staticspace, ramppos);
      scene->addKid(ramp[1]->GetEntity());

      sgVec3 spkpos={-1,6,0};
      spikejump = new SpikeJump(modelmap->Get("jumpboard.3ds"), modelmap->Get("spikegate.3ds"), world, staticspace, spkpos);
      scene->addKid(spikejump->GetEntity());

      sgVec3 tablepos = { 6.25, 6, 0.75 };
      ssgEntity *turnrampmodel = modelmap->Get("turntable_ramp.3ds");
      ssgEntity *turnwheelmodel = modelmap->Get("turntable_wheel.3ds");
      turntable = new TurnTable(2.0, 0.2, turnrampmodel, turnwheelmodel, world, bigspace, staticspace, tablepos);
      scene->addKid(turntable->GetEntity());

      sgVec3 ctpos={-2,-4,0};
      carpettrack = new CarpetTrack(modelmap->Get("carpet.3ds"), modelmap->Get("track.3ds"), world, staticspace, ctpos);
      scene->addKid(carpettrack->GetEntity());

      sgVec3 doorpos;
      sgSetVec3(doorpos, 2, 0 ,0.65);
      doorstand[0] = new DoorStand(modelmap->Get("door.3ds"), modelmap->Get("doorstand.3ds"), world, bigspace, doorpos);
      scene->addKid(doorstand[0]->GetEntity());
      sgSetVec3(doorpos, 6, 0, 0.65);
      doorstand[1] = new DoorStand(modelmap->Get("door.3ds"), modelmap->Get("doorstand.3ds"), world, bigspace, doorpos);
      scene->addKid(doorstand[1]->GetEntity());

      sgVec3 wallpos = {11, 0, 0.2 };
      ssgEntity *cratemodel = (modelmap->LowDetail())?modelmap->Get("crate_low.3ds"):modelmap->Get("crate.3ds");
      cratewall = new CrateWall(wallpos, 4, 5, world, bigspace, cratemodel);
      scene->addKid(cratewall->GetEntity());

      static ssgTexture *tex_jump = 0;
      std::string fname = prefix+"/images/info_jump_smooth.rgb";
      if (!tex_jump)
        tex_jump = new ssgTexture(fname.c_str(), FALSE, FALSE);
      static ssgaBillboards *bb_jump = 0;
      if (!bb_jump)
      {
        bb_jump = new ssgaBillboards();
        bb_jump->setTexture(tex_jump);
        bb_jump->setWidth(-4.0f);
        bb_jump->setHeight(1.0f);
        bb_jump->add(0, 4, 1, 1);
        ssgSimpleState *ss = dynamic_cast<ssgSimpleState*>(bb_jump->getState()); assert(ss); ss->setMaterial(GL_EMISSION, 1,1,1,1);
      }
      scene->addKid(bb_jump);

      static ssgTexture *tex_carpet = 0;
      fname = prefix+"/images/info_carpet_smooth.rgb";
      if (!tex_carpet)
        tex_carpet = new ssgTexture(fname.c_str(), FALSE, FALSE);
      static ssgaBillboards *bb_carpet = 0;
      if (!bb_carpet)
      {
        bb_carpet = new ssgaBillboards();
        bb_carpet->setTexture(tex_carpet);
        bb_carpet->setWidth(-4.0f);
        bb_carpet->setHeight(1.0f);
        bb_carpet->add(2, -4, 1, 1);
        ssgSimpleState *ss = dynamic_cast<ssgSimpleState*>(bb_carpet->getState()); assert(ss); ss->setMaterial(GL_EMISSION, 1,1,1,1);
      }
      scene->addKid(bb_carpet);

      static ssgTexture *tex_door = 0;
      fname = prefix+"/images/info_door_smooth.rgb";
      if (!tex_door)
        tex_door = new ssgTexture(fname.c_str(), FALSE, FALSE);
      static ssgaBillboards *bb_door = 0;
      if (!bb_door)
      {
        bb_door = new ssgaBillboards();
        bb_door->setTexture(tex_door);
        bb_door->setWidth(-4.0f);
        bb_door->setHeight(1.0f);
        bb_door->add(0, 0, 1, 1);
        ssgSimpleState *ss = dynamic_cast<ssgSimpleState*>(bb_door->getState()); assert(ss); ss->setMaterial(GL_EMISSION, 1,1,1,1);
      }
      scene->addKid(bb_door);
    }

    ~PracticeScenario()
    {
      delete spikejump;
      delete carpettrack;
      delete cratewall;
      delete doorstand[0];
      delete doorstand[1];
      delete ramp[0];
      delete ramp[1];
      delete turntable;
      delete grid;
    }

    bool Sustain(float dt, bool action)
    {
      carpettrack->Sustain(dt);
      doorstand[0]->Sustain(dt);
      doorstand[1]->Sustain(dt);
      spikejump->Sustain(dt);
      cratewall->Sustain(dt);
      turntable->Sustain(dt);
      if (action)
        spikejump->Boost(dt);
      return false;
    }

  protected :
    SpikeJump *spikejump;
    CarpetTrack *carpettrack;
    CrateWall *cratewall;
    DoorStand *doorstand[2];
    StaticWorldObject *ramp[2];
    StaticWorldObject *grid;
    TurnTable *turntable;
};


class RallyScenario
{
  public:
    RallyScenario(dWorldID world, dSpaceID bigspace, dSpaceID staticspace, ssgRoot *scene, ModelMap *modelmap, const std::string &prefix)
    {
      sgVec3 terrainpos = {0,0,0};
      terrain = new StaticWorldObject(modelmap->Get("terrain3.3ds"), staticspace, terrainpos);
      terrain->name = "ground";
      scene->addKid(terrain->GetEntity());
    }
    ~RallyScenario()
    {
      if (terrain) delete terrain;
    }
    bool Sustain(float dt, bool action)
    {
      return false;
    }
  protected:
    StaticWorldObject *terrain;
};


class CrashScenario
{
  public :

    CrashScenario(dWorldID world, dSpaceID bigspace, dSpaceID staticspace, ssgRoot *scene, ModelMap *modelmap, const std::string &prefix)
    {
      sgVec3 gridpos={0,0,0};
      grid = new StaticWorldObject(modelmap->Get("testground.3ds"), staticspace, gridpos);
      grid->name = "ground";
      scene->addKid(grid->GetEntity());

      sgVec3 wallpos = {13, 0, 0.25};
      brickwall = new BrickWall(wallpos, 4, 3, world, bigspace, modelmap->Get("brick_full.3ds"), modelmap->Get("brick_half.3ds"));
      scene->addKid(brickwall->GetEntity());
    }

    ~CrashScenario()
    {
      if (brickwall) delete brickwall;
      if (grid) delete grid;
    }

    bool Sustain(float dt)
    {
      if (brickwall) brickwall->Sustain(dt);
      return false;
    }

    BrickWall *brickwall;

  protected :

    StaticWorldObject *grid;
};

#endif

