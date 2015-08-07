/*
 * sturmbahnfahrer
 * (c) 2006 by Bram Stolk
 * bram at gmail.com
 * LICENSED ACCORDING TO THE GPLV3
 */

#include "staticworldobject.h"


StaticWorldObject::StaticWorldObject(ssgEntity *model, dSpaceID bigspace, sgVec3 initialpos) :
WorldObject(model)
{
  space = dSimpleSpaceCreate(bigspace);
  ssgTransform *trf = new ssgTransform();
  trf->setTransform(initialpos);
  trf->addKid(model);
  entity = trf;
  name = "staticworldobject";
  WalkTree(entity, initialpos);
}


StaticWorldObject::~StaticWorldObject()
{
  unsigned int i;
  // destroy the trimesh data
  for (i=0; i<dataids.size(); i++)
    dGeomTriMeshDataDestroy(dataids[i]);
  dataids.clear();

  // destroy the trimeshes
  for (i=0; i<geomids.size(); i++)
    dGeomDestroy(geomids[i]);
  geomids.clear();

  dSpaceDestroy(space);
}


void StaticWorldObject::WalkTree(ssgEntity *e, sgVec3 initialpos)
{
  if (!e) return;
  if (e->isAKindOf (ssgTypeBranch()) )
  {
    ssgBranch *branch = (ssgBranch*) e ;
    for (int i=0; i<branch->getNumKids(); i++)
    {
      ssgEntity *kid = branch->getKid(i);
      assert(kid!=NULL);
      WalkTree(kid, initialpos);
    }
  }
  if ( e->isAKindOf ( ssgTypeLeaf () ) )
  {
    AddLeaf((ssgLeaf*)e, initialpos);
  }
}


void StaticWorldObject::AddLeaf(ssgLeaf *leaf, sgVec3 initialpos)
{
  // traverse the triangles
  int cnt = leaf->getNumTriangles() ;
  int nv  = leaf->getNumVertices() ;
//  int nn  = leaf->getNumNormals() ;

  float *vertices = new float[3*nv];
  int   *indices  = new int[3*cnt];

  int i;
  for (i=0; i<nv; i++)
  {
    float *v = leaf->getVertex( i ) ;
    assert(v);
    memcpy(vertices+3*i, v, 3*sizeof(float));
  }
  for (i=0; i<cnt; i++)
  {
    short idx0, idx1, idx2 ;
    leaf->getTriangle( i, &idx0, &idx1, &idx2 ) ;
    indices[3*i+0]=idx0;
    indices[3*i+1]=idx1;
    indices[3*i+2]=idx2;
  }

  dTriMeshDataID data = dGeomTriMeshDataCreate();
  dataids.push_back(data);
  dGeomTriMeshDataBuildSingle
  (
    data, 
    vertices,
    3*sizeof(float), 
    nv, 
    indices,
    cnt*3, 
    3*sizeof(int)
  );
  //fprintf(stderr,"Adding trimesh with %d verts, %d indices\n", nv, cnt*3);
  dGeomID trimesh = dCreateTriMesh(space, data, 0,0,0);
  geomids.push_back(trimesh);
  dGeomSetPosition(trimesh, initialpos[0], initialpos[1], initialpos[2]);
  dMatrix3 R;
  dRFromAxisAndAngle (R, 0,1,0, 0.0);
  dGeomSetRotation (trimesh, R);
  dGeomSetData(trimesh, this);
}

