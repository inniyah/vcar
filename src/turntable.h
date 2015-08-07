#ifndef TURNTABLE_H
#define TURNTABLE_H

#include <math.h>
#include <ode/ode.h>
#include <plib/ssg.h>

#include "worldobject.h"



class TurnTableWheel : public DynamicObject
{
  public:

    TurnTableWheel(float radius, float width, ssgEntity *wheelmodel, dWorldID world, dSpaceID space, sgVec3 initialpos) :
      DynamicObject(wheelmodel, world, space)
    {
      name = "turntablewheel";

      // Take care of bodies
      body = dBodyCreate(world);
      dBodySetAutoDisableFlag(body, false);
      dBodySetPosition (body,initialpos[0],initialpos[1],initialpos[2]);

      dQuaternion q;
      dQFromAxisAndAngle (q, 1,0,0, 0.0);
      dBodySetQuaternion (body,q);
      dMass m;
      dMassSetCylinder (&m, 2.5, 2, radius, width);
      dBodySetMass (body,&m);

      static float vertices[33][3];
      static int   indices[32][3];
      for (int i=0; i<32; i++)
      {
        vertices[i+1][0] = radius*cosf(i*M_PI*2/32);
        vertices[i+1][1] = radius*sinf(i*M_PI*2/32);
        vertices[i+1][2] = 0.0;
        indices[i][0] = 1+i;
        indices[i][1] = 1+(i+1)%32;
        indices[i][2] = 0;
      }
      vertices[0][0]=0;
      vertices[0][1]=0;
      vertices[0][2]=0;
      dataid = dGeomTriMeshDataCreate();
      dGeomTriMeshDataBuildSingle
      (
        dataid,
        vertices,
        3*sizeof(float),
        33,
        indices,
        32*3,
        3*sizeof(int)
      );
      geom = dCreateTriMesh(space, dataid, 0,0,0);
      dGeomSetPosition(geom, initialpos[0],initialpos[1],initialpos[2]);
      dMatrix3 R;
      dRFromAxisAndAngle (R, 0,1,0, 0.0);
      dGeomSetRotation(geom, R);

      dGeomSetBody (geom,body);
      dGeomSetData (geom,this);
    }

    ~TurnTableWheel()
    {
      dGeomTriMeshDataDestroy(dataid);
    }

    dTriMeshDataID dataid;
};


class TurnTable
{
  public:

    TurnTable
    (
      float wheelradius, 
      float wheelwidth, 
      ssgEntity *rampmodel, 
      ssgEntity *wheelmodel, 
      dWorldID world,
      dSpaceID space, 
      dSpaceID staticspace, 
      sgVec3 initialpos
    )
    {
      // Take care of space
      wheelspace = dSimpleSpaceCreate(staticspace);
      dSpaceSetCleanup(wheelspace,0);

      sgVec3 ps[2] = 
      {
        { initialpos[0]+4, initialpos[1]-2, initialpos[2] },
        { initialpos[0]+4, initialpos[1]+2, initialpos[2] }
      };


      for (int i=0; i<2; i++)
      {
        wheels[i] = new TurnTableWheel(wheelradius, wheelwidth, wheelmodel, world, wheelspace, ps[i]);
        // Connect wheel and world
        wheel_hinges[i] = dJointCreateHinge (world, 0);
        dJointAttach(wheel_hinges[i], wheels[i]->body, 0);
        dJointSetHingeAnchor(wheel_hinges[i], ps[i][0], ps[i][1], ps[i][2]);
        dJointSetHingeAxis(wheel_hinges[i], 0,0,1);
        dJointSetHingeParam(wheel_hinges[i], dParamVel, 1.40);
        dJointSetHingeParam(wheel_hinges[i], dParamFMax, 400.00);
        dJointSetHingeParam(wheel_hinges[i], dParamCFM, 0.000);
      }

      ramp = new StaticWorldObject(rampmodel, staticspace, initialpos);
      ramp->name = "turntableramp";

      branch = new ssgBranch();
      branch->addKid(wheels[0]->trf);
      branch->addKid(wheels[1]->trf);
      branch->addKid(ramp->GetEntity());
    }

    ~TurnTable()
    {
      delete ramp;
      dJointDestroy(wheel_hinges[0]);
      dJointDestroy(wheel_hinges[1]);
      delete wheels[0];
      delete wheels[1];
      dSpaceDestroy(wheelspace);
    }

    void Sustain(float dt)
    {
      wheels[0]->Sustain(dt);
      wheels[1]->Sustain(dt);
    }

    ssgBranch *GetEntity(void) const { return branch; }

  protected:

    ssgBranch *branch;
    dSpaceID wheelspace;
    TurnTableWheel *wheels[2];
    StaticWorldObject *ramp;
    dJointID wheel_hinges[2];
};

#endif


