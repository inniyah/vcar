/*
 * sturmbahnfahrer
 * (c) 2006-2008 by Bram Stolk
 * bram at gmail.com
 * LICENSED ACCORDING TO THE GPLV3
 */

#include "dynamicobject.h"
#include "carobject.h"

#define CLENGTH 1.50		// chassis length
//#define CWIDTH 0.34		// chassis width
#define CWIDTH 0.68		// chassis width
#define CHEIGHT 0.05		// chassis height
#define CMASS 4			// chassis mass

#define WMASS 0.20		// wheel mass
#define WHEELW 0.2		// wheel width
#define WHEELRAD 0.2		// wheel radius
#define WHEELDROP 0.07		// dist from axle to chassis
#define HWHEELBASE 0.6		// half the wheelbase
#define HTRACK 0.45	 	// half the track (distance L/R wheels)

#define WISHBONEY 0.15		// how far outward is wishbone from chassis axis
#define WISHBONEZ 0.06		// how far up/dn is wishbone from chassis axis
#define WISHBONELEN 0.195	// how far tip of wishbone from pivot (spindle to chassis)
#define SPINDLEY 0.35		// how far outward is wheel spindle from chassis axis
#define AXLELEN 0.76		// length of rear axle
#define AXLERADIUS 0.03		// radius of rear axle, at thick end
#define COILLEN 0.22		// length of coil sping


CarObject::CarObject
(
  ssgEntity *bodymodel,
  ssgEntity *wheelmodel,
  ssgEntity *wishbonemodel,
  ssgEntity *spindlemodel,
  ssgEntity *coilspringmodel,
  ssgEntity *leafspringmodel,
  ssgEntity *rearaxlemodel,
  dWorldID world, 
  dSpaceID bigspace,
  sgVec3 initialpos
) :
WorldObject(bodymodel),
brakelightstate(0),
accelerator(0),
brake(0),
steer(0),
downtime(0),
stationarytime(0),
wheelspintime(0),
action(false),
reversegear(false),
ebrake(false),
ground_object(0),
snapshot_available(false)
{
  name = "car";
  brakelightstate = FindState("brakelight", bodymodel);
  // load visible model as plib entity
  ssgBranch *branch = new ssgBranch();
  chassis_trf = new ssgTransform();
  chassis_trf->setName("chassis_trf");

  branch->addKid(chassis_trf);
  chassis_trf->addKid(bodymodel);
  int i;
  for (i=0; i<4; i++)
  {
    wheel_trfs[i] = new ssgTransform();
    branch->addKid(wheel_trfs[i]);
    wheel_trfs[i]->addKid(wheelmodel);
  }
  for (i=0; i<4; i++)
  {
    wishbone_trfs[i] = new ssgTransform();
    wishbone_trfs[i]->setName("wishbone_trf");
    wishbone_trfs[i]->addKid(wishbonemodel);
    chassis_trf->addKid(wishbone_trfs[i]);
  }
  // Left front spindle
  spindle_trfs[0] = new ssgTransform();
  spindle_trfs[0]->setName("spindle_trf L");
  spindle_trfs[0]->addKid(spindlemodel);
  chassis_trf->addKid(spindle_trfs[0]);
  // Right front spindle
  spindle_trfs[1] = new ssgTransform();
  spindle_trfs[1]->setName("spindle_trf R");
  spindle_trfs[1]->addKid(spindlemodel);
  chassis_trf->addKid(spindle_trfs[1]);
  // Left front spring
  coilspring_trfs[0] = new ssgTransform();
  coilspring_trfs[0]->setName("spring_trf L");
  coilspring_trfs[0]->addKid(coilspringmodel);
  chassis_trf->addKid(coilspring_trfs[0]);
  // Right front spring
  coilspring_trfs[1] = new ssgTransform();
  coilspring_trfs[1]->setName("spring_trf R");
  coilspring_trfs[1]->addKid(coilspringmodel);
  chassis_trf->addKid(coilspring_trfs[1]);
  // rear axle
  rearaxle_trf = new ssgTransform();
  rearaxle_trf->setName("rearaxle_trf");
  rearaxle_trf->addKid(rearaxlemodel);
  chassis_trf->addKid(rearaxle_trf);
  // Left leaf spring
  leafspring_trfs[0] = new ssgTransform();
  leafspring_trfs[0]->setName("leafspring trf L");
  leafspring_trfs[0]->addKid(leafspringmodel);
  chassis_trf->addKid(leafspring_trfs[0]);
  // Right leaf spring
  leafspring_trfs[1] = new ssgTransform();
  leafspring_trfs[1]->setName("leafspring trf R");
  leafspring_trfs[1]->addKid(leafspringmodel);
  chassis_trf->addKid(leafspring_trfs[1]);

  entity = branch;

  // chassis
  chassis_body = dBodyCreate(world);
  dBodySetAutoDisableFlag(chassis_body, false);
  dBodySetPosition (chassis_body,initialpos[0],initialpos[1],initialpos[2]);
  dMass m;
  dMassSetBox (&m,1,CLENGTH,CWIDTH,CHEIGHT);
  dMassAdjust (&m,CMASS);
  dBodySetMass (chassis_body,&m);

  chassis_geom = dCreateBox (0,CLENGTH,CWIDTH,CHEIGHT);
  dGeomSetData (chassis_geom,this);
  dGeomSetBody (chassis_geom,chassis_body);

  axle_geom = dCreateBox (0,AXLERADIUS*2,AXLELEN,AXLERADIUS*2);
  dGeomSetData (axle_geom,this);
  dGeomSetBody (axle_geom,chassis_body);

#if 0
  frontend_geom = dCreateBox (0,0.24,0.68,0.10);
  dGeomSetData (frontend_geom,this);
  dGeomSetBody (frontend_geom,chassis_body);
  dGeomSetOffsetPosition (frontend_geom,HWHEELBASE-0.1,0,0);
#endif

  // wheel bodies
  for (i=0; i<4; i++)
  {
    wheel_bodies[i] = dBodyCreate(world);
    dBodySetAutoDisableFlag(wheel_bodies[i], false);
    dQuaternion q;
    dQFromAxisAndAngle (q,1,0,0,M_PI*0.5);
    dBodySetQuaternion (wheel_bodies[i],q);
    dMassSetSphere (&m,1,WHEELRAD);
    dMassAdjust (&m,WMASS);
    dBodySetMass (wheel_bodies[i],&m);
    if (i<=2 && 0) 
    {
      dBodySetFiniteRotationMode(wheel_bodies[i], true); // Improve accuracy for fast spinning bodies
      dBodySetFiniteRotationAxis(wheel_bodies[i], 0,0,0);
    }
    wheel_geoms[i] = dCreateCylinder(0, WHEELRAD, WHEELW);
    dGeomSetBody (wheel_geoms[i],wheel_bodies[i]);
    dGeomSetData (wheel_geoms[i],this);
  }
  dBodySetPosition (wheel_bodies[0],initialpos[0]+HWHEELBASE,initialpos[1]+HTRACK,initialpos[2]-WHEELDROP); // left front
  dBodySetPosition (wheel_bodies[1],initialpos[0]+HWHEELBASE,initialpos[1]-HTRACK,initialpos[2]-WHEELDROP); // right front
  dBodySetPosition (wheel_bodies[2],initialpos[0]-HWHEELBASE,initialpos[1]+HTRACK,initialpos[2]-WHEELDROP); // left rear
  dBodySetPosition (wheel_bodies[3],initialpos[0]-HWHEELBASE,initialpos[1]-HTRACK,initialpos[2]-WHEELDROP); // right rear

  // front and back wheel hinges
  for (i=0; i<4; i++)
  {
    joint[i] = dJointCreateHinge2 (world,0);
    dJointAttach (joint[i],chassis_body,wheel_bodies[i]);
    const dReal *a = dBodyGetPosition (wheel_bodies[i]);
    dReal y = (i%2) ? a[1]+0.1 : a[1]-0.1;
    dJointSetHinge2Anchor (joint[i],a[0],y,a[2]);
    dJointSetHinge2Axis1 (joint[i],0,0,1);
    dJointSetHinge2Axis2 (joint[i],0,1,0);
  }

  // set joint suspension
  for (i=0; i<4; i++)
  {
//    dJointSetHinge2Param (joint[i],dParamSuspensionERP,0.35);
//    dJointSetHinge2Param (joint[i],dParamSuspensionCFM,0.02);
    dJointSetHinge2Param (joint[i],dParamSuspensionERP,0.22);
    dJointSetHinge2Param (joint[i],dParamSuspensionCFM,0.06);
  }

  // lock back wheels along the steering axis
  for (i=2; i<=3; i++)
  {
    // set stops to make sure wheels always stay in alignment
    dJointSetHinge2Param (joint[i],dParamLoStop,0);
    dJointSetHinge2Param (joint[i],dParamHiStop,0);
    // Do this twice... weird ode bug! see mailing list and Jon's carworld
    dJointSetHinge2Param (joint[i],dParamLoStop,0);
    dJointSetHinge2Param (joint[i],dParamHiStop,0);
    dJointSetHinge2Param (joint[i],dParamStopERP,0.99);
    dJointSetHinge2Param (joint[i],dParamStopCFM,0.01);
  }

  space = dSimpleSpaceCreate(bigspace);
  dSpaceSetCleanup(space,0);
  dSpaceAdd (space,chassis_geom);
  dSpaceAdd (space,axle_geom);
//  dSpaceAdd (space,frontend_geom);
  for (i=0; i<4; i++)
    dSpaceAdd (space,wheel_geoms[i]);
}


CarObject::~CarObject()
{
  int i;
  for (i=0; i<4; i++)
  {
    dGeomDestroy(wheel_geoms[i]);
    dJointDestroy(joint[i]);
  }
  dGeomDestroy(chassis_geom);
  dGeomDestroy(axle_geom);
//  dGeomDestroy(frontend_geom);
  dSpaceDestroy(space);
}


float CarObject::GetWheelVelocity(int nr) const
{
  return fabs(*dBodyGetAngularVel(wheel_bodies[nr]));
}


bool CarObject::IsStationary(void) const
{
  // Are we stationary?
  const dReal *vel = dBodyGetLinearVel(chassis_body);
  dReal v[3]={vel[0],vel[1],vel[2]};

  if (ground_object)
  {
    const dReal *gvel = dBodyGetLinearVel(ground_object->body);
    v[0] -= gvel[0];
    v[1] -= gvel[1];
    v[2] -= gvel[2];
  }

  dReal magnitude = dSqrt(v[0]*v[0]+v[1]*v[1]+v[2]*v[2]);
  bool stationary = magnitude < 0.18;

  return stationary;
}


void CarObject::Simulate(float dt)
{
  // Get velocity of rear wheels.
  dReal v0 = -dJointGetHinge2Param (joint[2],dParamVel2);
  dReal v1 = -dJointGetHinge2Param (joint[2],dParamVel2);
  dReal currentv = (v0+v1)/2;

  dReal steerv = steer * 0.6;
  dReal speedv = 0.0;
  dReal fmax   = 0.0;

  // Are we in reverse gear?
  if (IsStationary())
  {
    stationarytime += dt;
    if (!ebrake && (accelerator > 0.7f || brake > 0.7))
      wheelspintime += dt;
    else
      wheelspintime = 0;
  }
  else
  {
    stationarytime = 0;
    wheelspintime = 0;
  }
  // HACK: Turn Table has rotational velocity, which is hard to interpret.
  // We simply assume we are stationary, so reverse gear is not blocked.
  bool hack = (ground_object && ground_object->name == "turntablewheel");
  if ((stationarytime>0.15||hack) && brake>0)
    reversegear=true;
  if (brake==0)
    reversegear=false;

  // Are we free rolling?
  if (brake==0 && accelerator==0 && ebrake==false)
  { 
    // apply a little engine braking
    speedv = 0.90 * currentv;
    fmax = 0.5;
  }
  else
  {
    // Are we braking?
    if ((brake>0 && !reversegear) || (accelerator>0 && reversegear) || ebrake)
    {
      speedv = 0;
      fmax = 50;
    }
    else
    {
      if (reversegear)
      {
        speedv = -70.0 * brake;
        fmax = 1.0 + 3.0 * brake;
      }
      else
      {
        speedv =  70.0 * accelerator;
        fmax = 1.0 + 3.0 * accelerator;
      }
    }
  }

  // motor: drive rear wheels.
  for (int jnr=2; jnr<=3; jnr++)
  {
    dJointSetHinge2Param (joint[jnr],dParamVel2,-speedv);
    dJointSetHinge2Param (joint[jnr],dParamFMax2,fmax);
  }

  // steering: turn front wheels.
  dReal curr0 = dJointGetHinge2Angle1(joint[0]);
  dReal curr1 = dJointGetHinge2Angle1(joint[1]);
  v0 = steerv - curr0;
  v1 = steerv - curr1;
  if (v0 > 0.1) v0 = 0.1;
  if (v0 < -0.1) v0 = -0.1;
  v0 *= 30.0;
  if (v1 > 0.1) v1 = 0.1;
  if (v1 < -0.1) v1 = -0.1;
  v1 *= 30.0;
  dJointSetHinge2Param (joint[0],dParamVel,v0);
  dJointSetHinge2Param (joint[0],dParamFMax,195);
  dJointSetHinge2Param (joint[0],dParamLoStop,-0.75);
  dJointSetHinge2Param (joint[0],dParamHiStop,0.75);
  dJointSetHinge2Param (joint[0],dParamFudgeFactor,0.1);
  dJointSetHinge2Param (joint[1],dParamVel,v1);
  dJointSetHinge2Param (joint[1],dParamFMax,295);
  dJointSetHinge2Param (joint[1],dParamLoStop,-0.75);
  dJointSetHinge2Param (joint[1],dParamHiStop,0.75);
  dJointSetHinge2Param (joint[1],dParamFudgeFactor,0.1);

  dBodySetFiniteRotationAxis(wheel_bodies[0],0,0,1);
  dBodySetFiniteRotationAxis(wheel_bodies[1],0,0,1);
}


void CarObject::Recover(float dt)
{
  sgVec3 carz;
  sgMat4 mat;
  chassis_trf->getTransform(mat);
  sgCopyVec3(carz, mat[2]);
  if (carz[2]<0.3)
  {
    downtime += dt;
  }
  else
  {
    downtime = 0.0;
  }
  if (downtime>2.8)
  {
    static float sign=1.0;
    float magnitude = 4.0 / dt;
    sgVec3 f;
    sgScaleVec3(f, carz, sign*magnitude);
    downtime=0.0;
    dBodyAddForce(chassis_body, 0,0,2*magnitude);
    dBodyAddForce(wheel_bodies[0],  f[0], f[1], f[2]);
    dBodyAddForce(wheel_bodies[2],  f[0], f[1], f[2]);
    dBodyAddForce(wheel_bodies[1], -f[0],-f[1],-f[2]);
    dBodyAddForce(wheel_bodies[3], -f[0],-f[1],-f[2]);
    sign=-sign;
  }

  if (wheelspintime > 1.0f)
  {
    wheelspintime = 0.0f;
    float x = ((rand()&255)-128) / 64.0f / dt;
    float y = ((rand()&255)-128) / 64.0f / dt;
    float z = 16.0 / dt;
    dBodyAddForce(chassis_body, x,y,z);
  }
}


void CarObject::UpdateSpindles(float travel_l, float travel_r)
{
  sgMat4 m;
  sgVec3 axis;

  dReal angle_l = -dJointGetHinge2Angle1(joint[0]);
  dReal angle_r =  dJointGetHinge2Angle1(joint[1]) + M_PI;
  sgSetVec3(axis, 0,0,1);

  sgMakeRotMat4(m, 180.0 * angle_l / M_PI, axis);
  sgSetVec3(m[3], HWHEELBASE, SPINDLEY, -travel_l );
  spindle_trfs[0]->setTransform(m);

  sgMakeRotMat4(m, -180.0 * angle_r / M_PI, axis);
  sgSetVec3(m[3], HWHEELBASE, -SPINDLEY, -travel_r );
  spindle_trfs[1]->setTransform(m);
}


void CarObject::UpdateCoilSprings(float travel_l, float travel_r)
{
  float angle_l = atan((-travel_l - 0.12) / WISHBONELEN);
  float angle_r = M_PI - atan((-travel_r - 0.12) / WISHBONELEN);

  float len_l = WISHBONELEN / cosf(angle_l);
  float len_r = WISHBONELEN / -cosf(angle_r);
  float scl_l = len_l / COILLEN;
  float scl_r = len_r / COILLEN;

  sgMat4 m;
  sgMat4 m_scale;

  sgVec3 x; sgSetVec3(x, 1,0,0);

  // Left
  sgMakeRotMat4(m, angle_l * 180.0 / M_PI, x);
  sgSetVec3(m[3], HWHEELBASE, WISHBONEY, WISHBONEZ);
  sgMakeIdentMat4(m_scale);
  m_scale[1][1] = scl_l;
  sgPreMultMat4(m, m_scale);
  coilspring_trfs[0]->setTransform(m);

  // Right
  sgMakeRotMat4(m, angle_r * 180.0 / M_PI, x);
  sgSetVec3(m[3], HWHEELBASE, -WISHBONEY, WISHBONEZ);
  sgMakeIdentMat4(m_scale);
  m_scale[1][1] = scl_r;
  sgPreMultMat4(m, m_scale);
  coilspring_trfs[1]->setTransform(m);
}


void CarObject::UpdateWishBones(float travel_l, float travel_r)
{
  float angle_l = -atan(travel_l / WISHBONELEN);
  float angle_r = M_PI + atan(travel_r / WISHBONELEN);

  sgMat4 m;
  sgVec3 x; sgSetVec3(x, 1,0,0);

  // upper left
  sgMakeRotMat4(m, angle_l * 180.0 / M_PI, x);
  sgSetVec3(m[3], HWHEELBASE, WISHBONEY, WISHBONEZ);
  wishbone_trfs[0]->setTransform(m);

  // lower left
  sgMakeRotMat4(m, angle_l * 180.0 / M_PI, x);
  sgSetVec3(m[3], HWHEELBASE, WISHBONEY, -WISHBONEZ);
  wishbone_trfs[1]->setTransform(m);

  // upper right
  sgMakeRotMat4(m, angle_r * 180.0 / M_PI, x);
  sgSetVec3(m[3], HWHEELBASE, -WISHBONEY, WISHBONEZ);
  wishbone_trfs[2]->setTransform(m);

  // lower right
  sgMakeRotMat4(m, angle_r * 180.0 / M_PI, x);
  sgSetVec3(m[3], HWHEELBASE, -WISHBONEY, -WISHBONEZ);
  wishbone_trfs[3]->setTransform(m);
}


void CarObject::UpdateRearSuspension(float travel_l, float travel_r)
{
  float t_l = travel_l + WHEELDROP;
  float t_r = travel_r + WHEELDROP;
  float t_avg = (travel_l + travel_r)/2.0;
  float lean = atan((t_r - t_l) / AXLELEN);

  sgMat4 m;
  sgVec3 axis;
  sgSetVec3(axis, 1,0,0);
  sgMakeRotMat4(m, 180 * lean / M_PI, axis);
  sgSetVec3(m[3], -HWHEELBASE, 0.0, -t_avg);

  rearaxle_trf->setTransform(m);

  dGeomSetOffsetPosition(axle_geom, m[3][0], m[3][1], m[3][2]);

  sgMakeIdentMat4(m);
  float sx,sz;

  float leaf_travel_l = 0.66 * travel_l + 0.34 * travel_r;
  float leaf_travel_r = 0.34 * travel_l + 0.66 * travel_r;

  sx = 1.07 - t_l;
  sz = (0.01 + leaf_travel_l) / 0.065;
  m[0][0] = sx;
  m[2][2] = sz;
  sgSetVec3(m[3], -HWHEELBASE, WISHBONEY, -0.025);
  leafspring_trfs[0]->setTransform(m);

  sx = 1.07 - t_r;
  sz = (0.01 + leaf_travel_r) / 0.065;
  m[0][0] = sx;
  m[2][2] = sz;
  sgSetVec3(m[3], -HWHEELBASE, -WISHBONEY, -0.025);
  leafspring_trfs[1]->setTransform(m);
}


void CarObject::Sustain(float dt)
{
  Simulate(dt);
  Recover(dt);
  SetTransformFromBody(chassis_trf, chassis_body);
  for (int i=0; i<4; i++)
    SetTransformFromBody(wheel_trfs[i], wheel_bodies[i]);
  sgMat4 m;
  chassis_trf->getTransform(m);

  float travel[4];

  for (int i=0; i<4; i++)
  {
    dVector3 a,b;
    dJointGetHinge2Anchor (joint[i], a);
    dJointGetHinge2Anchor2(joint[i], b);
    sgVec3 trav;
    sgSetVec3(trav, b[0]-a[0], b[1]-a[1], b[2]-a[2]);
    sgAddScaledVec3(trav, m[2], -WHEELDROP);
    travel[i] = -sgScalarProductVec3(m[2],trav);
  }

  UpdateWishBones(travel[0], travel[1]);
  UpdateCoilSprings(travel[0], travel[1]);
  UpdateSpindles(travel[0], travel[1]);
  UpdateRearSuspension(travel[2], travel[3]);

  SetBrakeLight();
}


ssgSimpleState *CarObject::FindState(const std::string &statename, ssgEntity *ent)
{
  if (!ent) return 0;
  if (ent->isAKindOf(ssgTypeLeaf()))
  {
    ssgLeaf *leaf = (ssgLeaf*) ent;
    ssgState *st = leaf->getState();
    if (st->getName() == statename && st->isAKindOf(ssgTypeSimpleState()))
      return (ssgSimpleState*) st;
  }
  if (ent->isAKindOf(ssgTypeBranch()))
  {
    ssgBranch *branch = (ssgBranch*) ent;
    for (int i=0; i<branch->getNumKids(); i++)
    {
      ssgEntity *kid = branch->getKid(i);
      assert(kid);
      ssgSimpleState *state = FindState(statename, kid);
      if (state) return state;
    }
  }
  return 0;
}


void CarObject::SetBrakeLight(void)
{
  if (!brakelightstate) return;
  if (reversegear)
  {
    brakelightstate->setMaterial(GL_EMISSION, 1,1,1,1);
    return;
  }
  if (ebrake || brake>0.0f)
  {
    brakelightstate->setMaterial(GL_EMISSION, 1,0,0,1);
    return;
  }
  if (!reversegear && !ebrake && !brake)
  {
    brakelightstate->setMaterial(GL_EMISSION, 0,0,0,1);
  }
}


bool CarObject::SnapShot(bool revert)
{
  static dVector3 cpos;
  static dQuaternion crot;
  static dVector3 wpos[4];
  static dQuaternion wrot[4];

  if (revert)
  {
    if (!snapshot_available) return false;
    dBodySetPosition (chassis_body, cpos[0], cpos[1], cpos[2]);
    dBodySetQuaternion (chassis_body, crot);
    dBodySetLinearVel (chassis_body, 0,0,0);
    dBodySetAngularVel (chassis_body, 0,0,0);
    for (int i=0; i<4; i++)
    {
      dBodySetPosition (wheel_bodies[i], wpos[i][0], wpos[i][1], wpos[i][2]);
      dBodySetQuaternion (wheel_bodies[i], wrot[i]);
      dBodySetLinearVel (wheel_bodies[i], 0,0,0);
      dBodySetAngularVel (wheel_bodies[i], 0,0,0);
    }
  }
  else
  {
    memcpy(cpos, dBodyGetPosition(chassis_body), sizeof(cpos));
    memcpy(crot, dBodyGetQuaternion(chassis_body), sizeof(crot));
    for (int i=0; i<4; i++)
    {
      memcpy(wpos[i], dBodyGetPosition(wheel_bodies[i]), sizeof(wpos[i]));
      memcpy(wrot[i], dBodyGetQuaternion(wheel_bodies[i]), sizeof(wrot[i]));
    }
    snapshot_available = true;
  }
  return true;
}


bool CarObject::IsWheelGeometry(dGeomID id, float &velocity) const
{
  if (id == wheel_geoms[0]) { velocity = GetWheelVelocity(0); return true; }
  if (id == wheel_geoms[1]) { velocity = GetWheelVelocity(1); return true; }
  if (id == wheel_geoms[2]) { velocity = GetWheelVelocity(2); return true; }
  if (id == wheel_geoms[3]) { velocity = GetWheelVelocity(3); return true; }
  return false;
}

