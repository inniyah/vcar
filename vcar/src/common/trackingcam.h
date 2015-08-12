
#ifndef TRACKCAM_H
#define TRACKCAM_H

#include <list>

#include <plib/sg.h>

class TrackingCam
{
  public:
    TrackingCam(float x=0, float y=0, float z=0) :
      distance(3.0),
      tiltangle(0.15 * M_PI),
      yoff(0),
      makejump(false),
      autocam(false)
    {
      sgSetVec3(tgtpos, x,y,z);
      sgSetVec3(campos, x,y,z);
      sgMakeIdentQuat(tgtori);
    }
    ~TrackingCam()
    {
    }
    void AddTarget(ssgTransform *tgt) { targets.push_back(tgt); }
    void DelTarget(ssgTransform *tgt) { targets.remove(tgt); }
    void Update(float dt)
    {
      float t=5.0*dt;
      if (t>1) t=1;

      sgQuat quats[4];
      sgVec3 posis[4];
      int i=0;
      std::list<ssgTransform*>::iterator ip;
      sgVec3 avgpos;
      sgZeroVec3(avgpos);
      for (ip=targets.begin(); ip!=targets.end(); ++ip)
      {
        sgMat4 mat;
        ssgTransform *target = *ip;
        target->getTransform(mat);

        // force z straight up
        sgSetVec3(mat[2],0,0,1);
        // y = z * x
        sgVectorProductVec3(mat[1], mat[2], mat[0]);
        sgNormalizeVec3(mat[1]);
        // x = y * z;
        sgVectorProductVec3(mat[0], mat[1], mat[2]);

        sgMatrixToQuat(quats[i], mat);
        sgCopyVec3(posis[i], mat[3]);
        sgAddVec3(avgpos, avgpos, mat[3]);
        i++;
      }
      assert(i>=1 && i<=4);
      sgScaleVec3(avgpos, avgpos, 1.0 / i);
      int j;
      if (autocam)
      {
        // What car has longest distance from avg pos?
        float longest=0;
        for (j=0; j<i; j++)
        {
          sgVec3 d;
          sgSubVec3(d, posis[j], avgpos);
          float dist=sgLengthVec3(d);
          if (dist > longest) longest = dist;
        }

        distance = 1.5 * longest;
        if (distance<2.0) distance=2.0;

        tiltangle = 0.32 * longest;
        if (tiltangle < 0.25) tiltangle=0.25;
        if (tiltangle > 0.60) tiltangle=0.60;
      }

      for (j=0; j<i; j++)
        AdjustTo(quats[j], avgpos, t);
      return;
    }
    void AdjustTo(sgQuat quat, sgVec3 pos, float t)
    {
      sgQuat newquat;
      sgSlerpQuat(newquat, tgtori, quat, t);

      float parm=t;
      if (makejump)
      {
        parm=1.0;
        makejump=false;
      }
      sgVec3 newpos;
      sgLerpVec3(newpos, tgtpos, pos, parm);

      sgCopyQuat(tgtori, newquat);
      sgCopyVec3(tgtpos, newpos);

      sgMat4 mat;
      sgQuatToMatrix(mat, tgtori);
      sgCopyVec3(mat[3], tgtpos);

      float camdist = cos(tiltangle) * distance;
      float camheight = sin(tiltangle) * distance;

      sgCopyVec3(campos, tgtpos);
      sgAddScaledVec3(campos, mat[0], -camdist);
      campos[2] += camheight;
      campos[1] += yoff;

      sgCopyVec3(mat[3], campos);
    }
    void GetCameraPos(sgVec3 dst) { sgCopyVec3(dst, campos); }
    void GetTargetPos(sgVec3 dst) { sgCopyVec3(dst, tgtpos); }

    void SetTiltAngle(float a) { tiltangle = a; }
    void SetDistance(float d) { distance = d; }
    void SetYOff(float y) { yoff = y; }

    void RequestJump(void) { makejump=true; }
    void SetAutoCam(bool a) { autocam=a; }

  protected:
    std::list<ssgTransform *> targets;
    sgVec3 tgtpos;
    sgVec3 campos;
    sgQuat tgtori;
    float distance;
    float tiltangle;
    float yoff;
    bool makejump;
    bool autocam;
};

#endif
