#ifndef RESPAWNPOINT_H
#define RESPAWNPOINT_H

#include <worldobject.h>

class RespawnPoint : public WorldObject
{
  public:

    RespawnPoint(ssgEntity *model, dSpaceID space, sgVec3 pos) :
      WorldObject(model)
    {
      name = "respawnpoint";
      trf = new ssgTransform();
      trf->setTransform(pos);
      trf->addKid(model);
      entity = trf;
      geom = dCreateBox(space, 0.05, 1.0, 1.0);
      dGeomSetPosition(geom, pos[0], pos[1], pos[2]);
      dGeomSetData (geom,this);
      angle = 0.0f;
    }

    ~RespawnPoint()
    {
      if (geom)
        Disable();
    }

    void Disable(void)
    {
      dGeomDestroy(geom);
      geom=0;
      trf->removeAllKids();
      ssgBranch *parent;
      do
      {
        parent = trf->getParent(0);
        if (parent) parent->removeKid(trf);
      } while (parent);
      trf = 0;
    }

    ssgEntity *GetEntity(void)
    {
      return trf;
    }

    virtual bool Sustain(float dt)
    {
      if (!trf || !geom) return false; // we may have been disabled
      angle += 2*dt;
      sgMat4 mat;
      trf->getTransform(mat);
      sgMakeCoordMat4
      (
        mat, 
        mat[3][0], mat[3][1], mat[3][2], 	// pos
        angle*180/M_PI, 0, 0			// hpr rotation
      );
      trf->setTransform(mat);
      dQuaternion q;
      dQFromAxisAndAngle(q, 0,0,1, angle*180/M_PI);
      dGeomSetQuaternion(geom, q);
      return true;
    }

  protected:
    ssgTransform *trf;
    dGeomID geom;
    float angle;
};

#endif

