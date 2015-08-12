#ifndef CAROBJECT_H
#define CAROBJECT_H

#include <string>
#include <set>

#include <ode/ode.h>
#include <plib/ssg.h>

#include "worldobject.h"


class DynamicObject;

class CarObject : public WorldObject
{
  public:
    CarObject
    (
      ssgEntity * bodymodel,
      ssgEntity * wheelmodel,
      ssgEntity * wishbone_model,
      ssgEntity * spindle_model,
      ssgEntity * coilspring_model,
      ssgEntity * leafspring_model,
      ssgEntity * rearaxle_model,
      dWorldID world,
      dSpaceID bigspace,
      sgVec3 initialpos
    );
    virtual ~CarObject();
    void Recover(float dt);
    void Simulate(float dt);
    void Sustain(float dt);
    void UpdateWishBones(float travel_l, float travel_r);
    void UpdateCoilSprings(float travel_l, float travel_r);
    void UpdateSpindles(float travel_l, float travel_r);
    void UpdateRearSuspension(float travel_l, float travel_r);
    void SetAccelerator(float v) { accelerator=v; }
    void SetSteer(float v) { steer = v; }
    void SetBrake(float v) { brake = v; }
    void SetEBrake(bool b) { ebrake = b; }
    float GetAccelerator(void) { return accelerator; }
    float GetBrake(void) { return brake; }
    void SetAction(bool a) { action = a; }
    bool GetAction(void) { return action; }
    ssgTransform * GetTransform(void) { return chassis_trf; }
    void GetPos(sgVec3 dst) const { sgMat4 m; chassis_trf->getTransform(m); sgCopyVec3(dst, m[3]); }
    void GetDir(sgVec3 dst) const { sgMat4 m; chassis_trf->getTransform(m); sgCopyVec3(dst, m[0]); }
    float GetWheelVelocity(int nr) const;
    bool IsBackWheelGeometry(dGeomID id) const { return id==wheel_geoms[2] || id==wheel_geoms[3]; }
    bool IsWheelGeometry(dGeomID id, float &velocity) const;
    void SetGround(DynamicObject * dwo) { ground_object = dwo; }
    bool IsStationary(void) const;
    bool SnapShot(bool revert);
  protected:
    ssgSimpleState * FindState(const std::string &statename, ssgEntity *ent);
    void SetBrakeLight(void);
    ssgSimpleState * brakelightstate;
    ssgTransform * chassis_trf;
    ssgTransform * sink_trf; // for lowering center of mass
    ssgTransform * wheel_trfs[4];
    ssgTransform * wishbone_trfs[4];
    ssgTransform * spindle_trfs[2];
    ssgTransform * coilspring_trfs[2];
    ssgTransform * leafspring_trfs[2];
    ssgTransform * rearaxle_trf;
    dSpaceID space;
    dBodyID  chassis_body;
    dBodyID  wheel_bodies[4];
    dGeomID  chassis_geom;
    dGeomID  axle_geom;
    dGeomID  frontend_geom;
    dGeomID  wheel_geoms[4];
    dJointID joint[4]; // joint[0..1] are the front wheels
    float accelerator;
    float brake;
    float steer;
    float downtime;
    float stationarytime;
    float wheelspintime;
    bool  action;
    bool  reversegear;
    bool  ebrake;
    DynamicObject * ground_object;
    bool  snapshot_available;
};

#endif
