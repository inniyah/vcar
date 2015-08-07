#ifndef STEREOCONTEXT_H
#define STEREOCONTEXT_H


class StereoContext
{
  public:

    StereoContext(bool do_quad_buffered_stereo=false) :
      quadbuffered(do_quad_buffered_stereo),
      aperture(90),
      focallength(1.22),
      eyeseparation(0.07),
      near(0.4),
      far(250),
      winw(1024),
      winh(768),
      last_context(0)
    {
      contexts[0] = new ssgContext();
      contexts[1] = new ssgContext();
      contexts[0]->setNearFar(near, far);
      contexts[1]->setNearFar(near, far);
    }

    ~StereoContext()
    {
      delete contexts[0];
      delete contexts[1];
    }

    void MakeCurrent(const std::string &channel)
    {
      assert(channel=="left" || channel=="right" || channel=="center" || channel=="last");

      ssgContext *context=0;
      if (channel=="last")  context=last_context;
      if (channel=="left")  context=contexts[0];
      if (channel=="right") context=contexts[1];
      assert(context);
      last_context = context;
 
      if (quadbuffered)
      {
        if (channel=="left")  glDrawBuffer(GL_BACK_LEFT);
        if (channel=="right") glDrawBuffer(GL_BACK_RIGHT);
        glViewport(0,0,winw,winh);
      }
      else
      {
        // Do viewport
        if (channel=="right")
          glViewport(winw, 0, winw, winh);
        if (channel=="left")
          glViewport(0,0, winw, winh);
        if (channel=="center")
          glViewport(0,0, winw*2, winh);
      }

      // Do Frustum
      float ratio  = winw / (float) winh;
      float wd2    = near * tan(0.5 * aperture * M_PI / 180);
      float ndfl   = near / focallength;
      float left   = -ratio * wd2;
      float right  =  ratio * wd2;
      float top    =  wd2;
      float bottom = -wd2;
      // For the left camera, the frustum should be shifted to the right.
      float separation_in_near_plane = eyeseparation * ndfl;
      if (channel == "left")
      {
        left  = -ratio * wd2 + 0.5 * separation_in_near_plane;
        right =  ratio * wd2 + 0.5 * separation_in_near_plane;
      }
      if (channel == "right")
      {
        left  = -ratio * wd2 - 0.5 * separation_in_near_plane;
        right =  ratio * wd2 - 0.5 * separation_in_near_plane;
      }
      context->setFrustum(left, right, bottom, top, near, far);

      // Do camera
      sgVec3 r={0,0,0};
      sgVectorProductVec3(r, cam_dir, cam_up);

      if (channel=="left")
        sgScaleVec3(r, -0.5 * eyeseparation);
      if (channel=="right")
        sgScaleVec3(r,  0.5 * eyeseparation);

      sgVec3 eye = { cam_eye[0] + r[0], cam_eye[1] + r[1], cam_eye[2] + r[2] };
      sgVec3 coi = { cam_eye[0] + cam_dir[0] + r[0], cam_eye[1] + cam_dir[1] + r[1], cam_eye[2] + cam_dir[2] + r[2] };
      sgVec3 up  = { cam_up[0], cam_up[1], cam_up[2] };

      context->setCameraLookAt(eye, coi, up);
      context->makeCurrent();
    }

    void SetAperture(float a) { aperture = a; }
    void SetFocalLength(float f) { focallength = f; }
    void SetEyeSeparation(float e) { eyeseparation = e; }
    void SetNear(float n) { near = n; }
    void SetFar(float f) { far = f; }
    void SetWindowSize(int w, int h) 
    { 
      winw=w; 
      winh=h; 
    }
    void SetCameraLookAt(const sgVec3 eye, const sgVec3 coi, const sgVec3 up)
    {
      sgCopyVec3(cam_up,  up);
      sgCopyVec3(cam_eye, eye);
      sgCopyVec3(cam_dir, coi);
      sgSubVec3(cam_dir,  eye);
      sgNormalizeVec3(cam_dir);
    }
    bool IsQuadBuffered(void) const { return quadbuffered; }
    void getModelviewMatrix(sgMat4 m) { last_context->getModelviewMatrix(m); }

  protected:

    bool  quadbuffered;		// When false, do passive stereo
    float aperture;		// degrees
    float focallength;		// meters
    float eyeseparation;	// meters
    float near;			// meters
    float far;			// meters
    int   winw;			// pixels
    int   winh;			// pixels
    sgVec3 cam_eye;		// position
    sgVec3 cam_dir;		// unit vector
    sgVec3 cam_up;		// unit vector

    ssgContext *contexts[2];	// 0 is L, 1 is R
    ssgContext *last_context;
};

#endif

