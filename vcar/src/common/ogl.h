#ifndef OGL_H
#define OGL_H

#include <string>	// for std::string

extern const float spot_fov;


class ssgRoot;

extern void OglErrorCheck(const std::string &context);

extern bool OglCanDoShadowing(void);

extern void OglInitShadowing(const std::string &dirprefix);

extern void OglRenderShadowMap(ssgRoot *scene);

extern void OglSetupSecondPass(float *mat_cam, float *mat_spt);

#endif

