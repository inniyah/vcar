#ifndef DOORSTAND_H
#define DOORSTAND_H

#include <ode/ode.h>
#include <plib/ssg.h>

#include "worldobject.h"


class DoorStand : public WorldObject
{
  public:

    DoorStand(ssgEntity *doormodel, ssgEntity *standmodel, dWorldID world, dSpaceID bigspace, sgVec3 initialpos) :
      WorldObject(doormodel)
    {
      name = "doorstand";

      // Take care of plib scene graph
      chassis_trf = new ssgTransform();
      chassis_trf->setName("chassis_trf");
      chassis_trf->addKid(standmodel);

      door_trf = new ssgTransform();
      door_trf->setName("door_trf");
      door_trf->addKid(doormodel);

      ssgBranch *branch = new ssgBranch();
      branch->addKid(chassis_trf);
      branch->addKid(door_trf);
      entity = branch;

      // Take care of bodies
      chassis_body = dBodyCreate(world);
      dBodySetAutoDisableFlag(chassis_body, false);
      dBodySetPosition (chassis_body,initialpos[0],initialpos[1],initialpos[2]+0.3);

      dReal sizes[4][3] = { {0.1, 0.1, 1.0}, {0.1, 0.1, 1.0}, {1.0, 0.1, 0.1}, {1.0, 0.1, 0.1} };
      dReal xlats[4][3] = { {0,-0.75,0.20}, {0,0.75,0.20}, {0,-0.75,-0.25}, {0,0.75,-0.25} };

      // Take care of geoms
      dMass m_total;
      dMassSetZero(&m_total);
      int i;
      for (i=0; i<4; i++)
      {
        box_geom[i] = dCreateBox(0, sizes[i][0], sizes[i][1], sizes[i][2]);
        dGeomSetData(box_geom[i], this);
        dGeomSetBody(box_geom[i], chassis_body);
        dGeomSetOffsetPosition(box_geom[i], xlats[i][0], xlats[i][1], xlats[i][2]);
        // do mass
        dMass m;
        dMassSetZero(&m_total);
        dMassSetBox(&m, 28, sizes[i][0], sizes[i][1], sizes[i][2]);
        dMassTranslate(&m, xlats[i][0], xlats[i][1], xlats[i][2]);
        dMassAdd (&m_total, &m);
      }

      dReal *o = m_total.c;
      dMassTranslate(&m_total, -o[0], -o[1], -o[2]);
      dMassAdjust(&m_total, 3.0);
      dBodySetMass(chassis_body, &m_total);

      // Do the door
      dReal sz[3] = { 0.1, 1.4, 1.6 };
      dReal ps[3] = { initialpos[0], initialpos[1], initialpos[2]+0.85 };
      door_body = dBodyCreate(world);
      dBodySetAutoDisableFlag(door_body, false);
      dBodySetPosition(door_body, ps[0], ps[1], ps[2]);
      dMass m;
      dMassSetBox(&m, 1, sz[0], sz[1], sz[2]);
      dBodySetMass(door_body, &m);
      door_geom = dCreateBox(0, sz[0], sz[1], sz[2]);
      dGeomSetData(door_geom, this);
      dGeomSetBody(door_geom, door_body);
      dGeomSetPosition(door_geom, ps[0], ps[1], ps[2]);


      // Connect door and stand
      hinge = dJointCreateHinge (world, 0);
      dJointAttach(hinge, chassis_body, door_body);
      dJointSetHingeAnchor(hinge, initialpos[0], initialpos[1], initialpos[2]+0.9);
      dJointSetHingeAxis(hinge, 0,1,0);
      dJointSetHingeParam(hinge, dParamVel,  0.00);
      dJointSetHingeParam(hinge, dParamFMax, 0.20);


      // Take care of space
      space = dSimpleSpaceCreate(bigspace);
      dSpaceSetCleanup(space,0);
      for (i=0; i<4; i++)
        dSpaceAdd (space, box_geom[i]);
      dSpaceAdd(space, door_geom);
    }

    ~DoorStand()
    {
    }

    void Sustain(float dt)
    {
      SetTransformFromBody(chassis_trf, chassis_body);
      SetTransformFromBody(door_trf, door_body);
    }

    float GetAngle(void) const
    {
      return dJointGetHingeAngle(hinge);
    }

    ssgTransform *GetTransform(void) { return chassis_trf; }
    void GetPos(sgVec3 dst) const { sgMat4 m; chassis_trf->getTransform(m); sgCopyVec3(dst, m[3]); }


  protected:

    dSpaceID space;
    dJointID hinge;

    // For the stand
    ssgTransform *chassis_trf;
    dBodyID  chassis_body;
    dGeomID  box_geom[4];

    // For the door
    ssgTransform *door_trf;
    dGeomID  door_geom;
    dBodyID  door_body;
};

#endif


