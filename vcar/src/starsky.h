#ifndef STARSKY_H
#define STARSKY_H

#include <stdlib.h>	// for rand()
#include <assert.h>
#include <GL/gl.h>

class StarSky
{
  public:
    StarSky(int n=1024, float r=100.0) :
    size(n)
    {
      state = new ssgSimpleState();
      state->setMaterial(GL_EMISSION, 1,1,1,1);
      verts = new ssgVertexArray(size);
      for (int i=0; i<size; i++)
      {
        float longitude = rand() * 2.0 * M_PI / RAND_MAX;
        float latitude  = rand() * M_PI / RAND_MAX;
        float z = r * cos(latitude);
        float x = r * cos(longitude) * sin(latitude);
        float y = r * sin(longitude) * sin(latitude);
        sgVec3 p={x,y,z};
        verts->add(p);
      }
      vtable = new ssgVtxTable(GL_POINTS, verts, 0, 0, 0);
      vtable->setState(state);
    }
    ~StarSky()
    {
      delete verts;
      delete vtable;
    }
    ssgEntity *GetEntity(void) { return vtable; }

  protected:
    int   size;
    ssgSimpleState *state;
    ssgVertexArray *verts;
    ssgVtxTable    *vtable;
};

#endif
