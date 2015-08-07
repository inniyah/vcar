#ifndef FERRISWHEEL_H
#define FERRISWHEEL_H

#include <ode/ode.h>
#include <plib/ssg.h>

#include "worldobject.h"

class FerrisWheelCart : public DynamicObject
{
  public:

    FerrisWheelCart(ssgEntity *cartmodel, dWorldID world, dSpaceID space, dReal *initialpos) :
      DynamicObject(cartmodel, world, space)
    {
      name = "ferriswheelcart";
      // Take care of bodies
      body = dBodyCreate(world);
      dBodySetAutoDisableFlag(body, false);
      dBodySetPosition (body,initialpos[0],initialpos[1],initialpos[2]-1.45);

      dReal sizes[2][3] = {{ 2.0, 2.9, 0.1 },  {0.2, 0.3, 1.5}};
      dReal xlats[2][4] = {{initialpos[0], initialpos[1], initialpos[2]-1.45}, {0,0,0.70}};

      dMass m;
      dMassSetZero(&m);

      box_geom[0] = dCreateBox(space, sizes[0][0], sizes[0][1], sizes[0][2]);
      dGeomSetData (box_geom[0], this);
      dGeomSetBody (box_geom[0], body);
      dGeomSetPosition(box_geom[0], xlats[0][0],xlats[0][1],xlats[0][2]);

      trf_geom = dCreateGeomTransform(space);
      dGeomSetData (trf_geom, this);
      dGeomSetBody(trf_geom, body);
      box_geom[1] = dCreateBox(0, sizes[1][0], sizes[1][1], sizes[1][2]);
      dGeomSetData (box_geom[1], this);
      dGeomTransformSetInfo(trf_geom, 1);
      dGeomTransformSetGeom(trf_geom, box_geom[1]);
      dGeomSetPosition(box_geom[1], xlats[1][0], xlats[1][1], xlats[1][2]);

      dMassSetBox(&m, 10, sizes[0][0], sizes[0][1], sizes[0][2]);
      dBodySetMass (body, &m);
    }

    ~FerrisWheelCart()
    {
      dGeomDestroy(box_geom[0]);
      dGeomDestroy(box_geom[1]);
      dGeomDestroy(trf_geom);
    }

    dGeomID trf_geom;
    dGeomID box_geom[2];
};


class FerrisWheelWheel : public WorldObject
{
  public:

    FerrisWheelWheel(float radius, ssgEntity *wheelmodel, dWorldID world, dSpaceID space, sgVec3 initialpos) :
      WorldObject(wheelmodel)
    {
      name = "ferriswheelwheel";
      // Take care of plib scene graph
      wheel_trf = new ssgTransform();
      wheel_trf->setName("wheel_trf");
      wheel_trf->addKid(wheelmodel);
      entity = wheel_trf;

      // Take care of bodies
      body = dBodyCreate(world);
      dBodySetAutoDisableFlag(body, false);
      dBodySetPosition (body,initialpos[0],initialpos[1],initialpos[2]);

      dReal sizes[2][3] = { {2*radius, 0.1,  0.2},  { 0.2, 0.1, 2*radius} };
      dReal xlats[2][3] = { {0,0,0}, {0,0,0} };

      // Take care of geoms
      dMass m_total;
      dMassSetZero(&m_total);
      int i;
      for (i=0; i<2; i++)
      {
        dMass m;
        dMassSetZero(&m_total);
        box_geom[i] = dCreateBox(0, sizes[i][0], sizes[i][1], sizes[i][2]);
        // do mass
        dMassSetBox(&m, 50, sizes[i][0], sizes[i][1], sizes[i][2]);
        dMassTranslate(&m, xlats[i][0], xlats[i][1], xlats[i][2]);
        dMassAdd (&m_total, &m);
        dGeomSetData (box_geom[i], this);
        trf_geom[i] = dCreateGeomTransform(space);
        dGeomSetData (trf_geom[i], this);
        dGeomSetBody (trf_geom[i], body);
        dGeomTransformSetInfo(trf_geom[i], 1);
        dGeomTransformSetGeom(trf_geom[i], box_geom[i]);
        dGeomSetPosition(trf_geom[i], initialpos[0],initialpos[1],initialpos[2]);
        dGeomSetPosition(box_geom[i], xlats[i][0], xlats[i][1], xlats[i][2]);
      }

      dReal *o = m_total.c;
      dMassTranslate(&m_total, -o[0], -o[1], -o[2]);
      dBodySetMass (body, &m_total);
    }

    ~FerrisWheelWheel()
    {
      dGeomDestroy(box_geom[0]);
      dGeomDestroy(box_geom[1]);
      dGeomDestroy(trf_geom[0]);
      dGeomDestroy(trf_geom[1]);
    }

    void Sustain(float dt)
    {
      SetTransformFromBody(wheel_trf, body);
    }

    ssgTransform *wheel_trf;
    dBodyID body;
    dGeomID trf_geom[2];
    dGeomID box_geom[2];
};


class FerrisWheel
{
  public:

    FerrisWheel(float radius, ssgEntity *cartmodel, ssgEntity *wheelmodel, ssgEntity *standmodel, dWorldID world, dSpaceID space, dSpaceID staticspace, sgVec3 initialpos) 
    {
      int i;
      for (i=0; i<4; i++) carts[i]=0;

      // Take care of space
      wheelspace = dSimpleSpaceCreate(staticspace);
      dSpaceSetCleanup(wheelspace,0);

      dReal rad=radius-0.05;
      dReal ps[5][3] = 
      {
        { initialpos[0]+rad, initialpos[1], initialpos[2] },
        { initialpos[0]-rad, initialpos[1], initialpos[2] },
        { initialpos[0], initialpos[1], initialpos[2]+rad },
        { initialpos[0], initialpos[1], initialpos[2]-rad },
        { initialpos[0], initialpos[1], initialpos[2] }
      };

      wheel = new FerrisWheelWheel(radius, wheelmodel, world, wheelspace, initialpos);
      for (i=0; i<4; i++)
      {
        carts[i] = new FerrisWheelCart(cartmodel, world, wheelspace, ps[i]);
        // Connect cart and wheel
        cart_hinges[i] = dJointCreateHinge (world, 0);
        dJointAttach(cart_hinges[i], wheel->body, carts[i]->body);
        dJointSetHingeAnchor(cart_hinges[i], ps[i][0], ps[i][1], ps[i][2]);
        dJointSetHingeAxis(cart_hinges[i], 0,1,0);
        dJointSetHingeParam(cart_hinges[i], dParamVel,  0.00);
        dJointSetHingeParam(cart_hinges[i], dParamFMax, 4.00);
      }

      // Connect wheel and world
      wheel_hinge = dJointCreateHinge (world, 0);
      dJointAttach(wheel_hinge, wheel->body, 0);
      dJointSetHingeAnchor(wheel_hinge, initialpos[0], initialpos[1], initialpos[2]);
      dJointSetHingeAxis(wheel_hinge, 0,1,0);
      dJointSetHingeParam(wheel_hinge, dParamVel,    0.20);
      dJointSetHingeParam(wheel_hinge, dParamFMax, 400.00);
      dJointSetHingeParam(wheel_hinge, dParamCFM,  0.000);

      stand = new StaticWorldObject(standmodel, staticspace, initialpos);
      stand->name = "ferriswheelstand";

      branch = new ssgBranch();
      branch->addKid(wheel->wheel_trf);
      branch->addKid(stand->GetEntity());
      for (i=0; i<4; i++)
        if (carts[i]) branch->addKid(carts[i]->trf);
    }

    ~FerrisWheel()
    {
      delete stand;
      dJointDestroy(wheel_hinge);
      for (int i=0; i<4; i++)
      {
        dJointDestroy(cart_hinges[i]);
        delete carts[i];
      }
      delete wheel;
      dSpaceDestroy(wheelspace);
    }

    void Sustain(float dt)
    {
      wheel->Sustain(dt);
      for (int i=0; i<4; i++)
        if (carts[i]) carts[i]->Sustain(dt);
    }

    ssgBranch *GetEntity(void) const { return branch; }

  protected:

    ssgBranch *branch;
    dSpaceID wheelspace;
    FerrisWheelWheel *wheel;
    FerrisWheelCart  *carts[4];
    StaticWorldObject *stand;
    dJointID cart_hinges[4];
    dJointID wheel_hinge;

};

#endif


