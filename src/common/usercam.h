
#ifndef USERCAM_H
#define USERCAM_H


#include <plib/sg.h>

class UserCam
{
  public:
    UserCam(float x=0, float y=0, float z=0) :
      distance(1.4),
      delta_heading(0),
      delta_pitch(20),
      fixed(false)
    {
      sgSetVec3(tgtpos, x,y,z);
      sgSetVec3(campos, x,y,z);
      sgSetVec3(target_offset,0,0,0);
      sgMakeIdentQuat(tgtori);
      sgMakeIdentMat4(camera_mat);
    }
    ~UserCam()
    {
    }
    void AddTarget(ssgTransform *tgt) { target = tgt; }
    void DelTarget(void) { target = 0; }
    void Update(float dt)
    {
      if (fixed) return;
      sgMat4 mat;
      target->getTransform(mat);

      sgVec3 pos;
      sgCopyVec3(pos, mat[3]);
      sgVec3 offset;
      sgXformVec3(offset, target_offset, mat);
      sgAddVec3(pos, pos, offset);

      // force z straight up
      sgSetVec3(mat[2],0,0,1);
      // y = z * x
      sgVectorProductVec3(mat[1], mat[2], mat[0]);
      sgNormalizeVec3(mat[1]);
      // x = y * z;
      sgVectorProductVec3(mat[0], mat[1], mat[2]);

      sgQuat quat;
      sgMatrixToQuat(quat, mat);

      AdjustTo(quat, pos, 3*dt, 30*dt);
      return;
    }
    void AdjustTo(sgQuat quat, sgVec3 pos, float t0, float t1)
    {
      if (t0 > 1) t0=1;
      if (t1 > 1) t1=1;
      sgQuat newquat;
      sgSlerpQuat(newquat, tgtori, quat, t0);

      sgVec3 newpos;
      sgLerpVec3(newpos, tgtpos, pos, t1);

      sgCopyQuat(tgtori, newquat);
      sgCopyVec3(tgtpos, newpos);

      sgPreRotQuat(newquat, -delta_heading, 0,0,1);
      sgPreRotQuat(newquat,  delta_pitch,   0,1,0);

      sgQuatToMatrix(camera_mat, newquat);

      sgCopyVec3(campos, tgtpos);
      sgAddScaledVec3(campos, camera_mat[0], -distance);
      sgCopyVec3(camera_mat[3], campos);
    }
    void GetCameraPos(sgVec3 dst) { sgCopyVec3(dst, campos); }
    void GetTargetPos(sgVec3 dst) { sgCopyVec3(dst, tgtpos); }
    void SetDistance(float d)   { distance = d; }
    void SetHeading(float h)    { delta_heading = h; }
    void SetPitch(float p)      { delta_pitch = p; }
    void SetOffset(float x, float y,float z) { sgSetVec3(target_offset,x,y,z); }
    void ChangeHeading(float d) 
    {
      delta_heading += d; 
      if (delta_heading<0)   delta_heading+=360;
      if (delta_heading>360) delta_heading-=360;
    }
    void ChangePitch(float d) 
    {
      const float maxp = 80.0;
      const float minp =  0.0;
      delta_pitch += d; 
      if (delta_pitch< minp) delta_pitch=minp;
      if (delta_pitch> maxp) delta_pitch=maxp;
    }
    void ChangeDistance(float d)
    {
      const float mind = 0.1;
      distance += d;
      if (distance < mind) distance=mind;
    }
    void ChangeOffset(float dx, float dy)
    {
      // from camera space to world space
      sgVec3 change={0,0,0};
      sgAddScaledVec3(change, camera_mat[1], dx);
      sgAddScaledVec3(change, camera_mat[2], dy);
      // from world space to car space
      sgMat4 carmat;
      target->getTransform(carmat);
      sgMat4 invcarmat;
      sgTransposeNegateMat4(invcarmat, carmat);
      sgXformVec3(change, invcarmat);
      // Change the offset
      sgAddVec3(target_offset, change);
    }
    bool ToggleFixed(void)
    {
      fixed = !fixed;
      return fixed;
    }

  protected:
    ssgTransform *target;
    sgVec3 target_offset;
    sgVec3 tgtpos;
    sgVec3 campos;
    sgQuat tgtori;
    sgMat4 camera_mat;

    float distance;
    float delta_heading;
    float delta_pitch;
    bool  fixed;
};

#endif

