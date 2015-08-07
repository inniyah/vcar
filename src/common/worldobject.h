#ifndef WORLDOBJECT_H
#define WORLDOBJECT_H

#include <string>

#include <plib/ssg.h>

#define _isnan isnan
#ifndef dVALIDMAT3
#define dVALIDMAT3 dVALIDMAT
#endif


inline void MakeDisplayLists(ssgEntity *e)
{
  static bool disabled = getenv("PLODE_NO_DISPLAY_LISTS");
  if (!e || disabled) return;
  if (e->isAKindOf(ssgTypeBranch()))
  {
    ssgBranch *branch = (ssgBranch*) e ;
    for (int i=0; i<branch->getNumKids(); i++)
    {
      ssgEntity *kid = branch->getKid(i);
      assert(kid) ;
      MakeDisplayLists(kid);
    }
  }
  if (e->isAKindOf (ssgTypeLeaf()))
  {
    ssgLeaf *leaf = (ssgLeaf*) e;
    leaf->makeDList();
  }
}


class WorldObject
{
  public:
    WorldObject(ssgEntity *model) : 
      name("unnamed worldobject"),
      entity(0)
    {
    }
    virtual ~WorldObject()
    {
    }
    virtual void Collide(WorldObject *other) {}
    ssgEntity *GetEntity(void) 
    { 
      return entity; 
    }

    std::string name;

  protected:
    // Interfaces between ODE API and PLIB API by converting transformations.
    void SetTransformFromBody(ssgTransform *trf, dBodyID body)
    {
      const dReal *p=dBodyGetPosition(body);
      assert(p);
      if (!dVALIDVEC3(p)) fprintf(stderr,"Position of %s body is invalid!\n", name.c_str());
      sgVec3 pos;
      sgSetVec3(pos,p[0],p[1],p[2]);

      const dReal *m=dBodyGetRotation(body);
      assert(m);
      if (!dVALIDMAT3(p)) fprintf(stderr,"Orientation of %s body is invalid!\n", name.c_str());
      sgMat4 mat;
      sgMakeIdentMat4(mat);

      mat[0][0]=m[0];
      mat[0][1]=m[4];
      mat[0][2]=m[8];
      mat[1][0]=m[1];
      mat[1][1]=m[5];
      mat[1][2]=m[9];
      mat[2][0]=m[2];
      mat[2][1]=m[6];
      mat[2][2]=m[10];

      sgCopyVec3(mat[3],pos);

      trf->setTransform(mat);
    }

    ssgEntity *entity;
};

#endif
