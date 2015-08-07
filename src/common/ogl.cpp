/*
 * stormbaancoureur
 * (c) 2006,2007,2008 by Bram Stolk
 * bram at gmail.com
 * LICENSED ACCORDING TO THE GPLV3
 */

#include "ogl.h"	// Our own interface

#include <assert.h>

#define GL_GLEXT_PROTOTYPES 1
#define GLX_GLXEXT_PROTOTYPES 1

#include <GL/glx.h>
#include <GL/glxext.h>	// for the opengl extensions we use for hardware accelerated shadowing

#include <GL/gl.h>
#include <GL/glut.h>

#include <plib/ssg.h>	// For ssgTexture


// Variables used for hardware accelerated shadow mapping
const float spot_fov = 3.250f;

static GLuint frame_buffer_object;
static GLuint light_view_depth_texture;

static ssgTexture *spotlight_texture=0;

static const int shadowmapsize = 512;
static GLuint fragment_shader, vertex_shader;
static GLuint program;
static GLint uniform_loc_tex0, uniform_loc_tex1, uniform_loc_tex2;


void OglErrorCheck(const std::string &context)
{
  GLenum error = glGetError();
  while (error != GL_NO_ERROR)
  {
    fprintf(stderr, "plode OpenGL error (%s): %s\n", context.c_str(),gluErrorString(error));
    error = glGetError();
  }
}


/*
 * This helper function checks wether we have all req'd extensions for shadow mapping
 */
bool OglCanDoShadowing(void)
{
  if (getenv("PLODE_NO_SHADOWS")) return false;
  const char *vers = (const char*) glGetString(GL_VERSION);
  const char *rend = (const char*) glGetString(GL_RENDERER);
  fprintf(stderr, "OpenGL is version %s\n", vers);
  fprintf(stderr, "OpenGL renderer %s\n", rend);
  const char *exts = (const char*) glGetString(GL_EXTENSIONS);
  //fprintf(stderr, "OpenGL has extensions %s\n", exts);
  if (!strstr(exts, "GL_ARB_multitexture")) return false;
  if (!strstr(exts, "GL_ARB_depth_texture")) return false;
  if (!strstr(exts, "GL_ARB_shadow")) return false;
  if (!strstr(exts, "GL_EXT_framebuffer_object")) return false;
  if (!strstr(exts, "GL_ARB_vertex_shader")) return false;
  if (!strstr(exts, "GL_ARB_fragment_shader")) return false;
  if (!strstr(exts, "GL_ARB_shader_objects")) return false;
  return true;
}


static void OglPrepareShaders(const std::string &dirprefix)
{
  PFNGLCREATESHADERPROC  glCreateShader  = (PFNGLCREATESHADERPROC)  glXGetProcAddressARB((const GLubyte *) "glCreateShader");
  PFNGLSHADERSOURCEPROC  glShaderSource  = (PFNGLSHADERSOURCEPROC)  glXGetProcAddressARB((const GLubyte *) "glShaderSource");
  PFNGLCOMPILESHADERPROC glCompileShader = (PFNGLCOMPILESHADERPROC) glXGetProcAddressARB((const GLubyte *) "glCompileShader");
  PFNGLGETSHADERIVPROC   glGetShaderiv   = (PFNGLGETSHADERIVPROC)   glXGetProcAddressARB((const GLubyte *) "glGetShaderiv");
  PFNGLCREATEPROGRAMPROC glCreateProgram = (PFNGLCREATEPROGRAMPROC) glXGetProcAddressARB((const GLubyte *) "glCreateProgram");
  PFNGLATTACHSHADERPROC  glAttachShader  = (PFNGLATTACHSHADERPROC)  glXGetProcAddressARB((const GLubyte *) "glAttachShader");
  PFNGLLINKPROGRAMPROC   glLinkProgram   = (PFNGLLINKPROGRAMPROC)   glXGetProcAddressARB((const GLubyte *) "glLinkProgram");
  PFNGLGETPROGRAMIVPROC  glGetProgramiv  = (PFNGLGETPROGRAMIVPROC)  glXGetProcAddressARB((const GLubyte *) "glGetProgramiv");
  PFNGLGETSHADERINFOLOGPROC glGetShaderInfoLog = (PFNGLGETSHADERINFOLOGPROC) glXGetProcAddressARB((const GLubyte *) "glGetShaderInfoLog");
  PFNGLGETPROGRAMINFOLOGPROC glGetProgramInfoLog = (PFNGLGETPROGRAMINFOLOGPROC) glXGetProcAddressARB((const GLubyte *) "glGetProgramInfoLog");
  PFNGLGETUNIFORMLOCATIONPROC glGetUniformLocation = (PFNGLGETUNIFORMLOCATIONPROC) glXGetProcAddressARB((const GLubyte *) "glGetUniformLocation");

  assert(glCreateShader);
  assert(glShaderSource);
  assert(glCompileShader);
  assert(glGetShaderiv);
  assert(glCreateProgram);
  assert(glLinkProgram);
  assert(glAttachShader);
  assert(glGetProgramiv);
  assert(glGetShaderInfoLog);
  assert(glGetUniformLocation);

  static GLchar fragment_program_text[8192];
  static GLchar vertex_program_text[8192];
  const char *fp = fragment_program_text;
  const char *vp = vertex_program_text;
  char log_text[4096];
  int sz;
  FILE *f;
  std::string fname;
  GLint status;

  fname = dirprefix + "/shaders/bramlight.fp";
  f = fopen(fname.c_str(),"rb");
  assert(f);
  sz = fread(fragment_program_text, 1, 8192, f);
  assert(sz>0);
  fclose(f);
  sz = strlen(fragment_program_text);
  fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragment_shader, 1, &fp, 0);
  glCompileShader(fragment_shader);
  glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &status);
  if (status != GL_TRUE) {
    glGetShaderInfoLog(fragment_shader, sizeof(log_text), 0, log_text);
    fprintf(stderr,"%s\n",log_text);
    exit(1);
  }

  fname = dirprefix + "/shaders/bramlight.vp";
  f = fopen(fname.c_str(),"rb");
  assert(f);
  sz = fread(vertex_program_text, 1, 8192, f);
  assert(sz>0);
  fclose(f);
  sz = strlen(vertex_program_text);
  vertex_shader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertex_shader, 1, &vp, 0);
  glCompileShader(vertex_shader);
  glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &status);
  if (status != GL_TRUE) {
    glGetShaderInfoLog(vertex_shader, sizeof(log_text), 0, log_text);
    fprintf(stderr,"%s\n",log_text);
    exit(1);
  }

  program = glCreateProgram();
  glAttachShader(program, vertex_shader);
  glAttachShader(program, fragment_shader);
  glLinkProgram(program);
  glGetProgramiv(program, GL_LINK_STATUS, &status);
  if (status != GL_TRUE)
  {
    glGetProgramInfoLog(program, sizeof(log_text), 0, log_text);
    fprintf(stderr,"%s\n",log_text);
    exit(1);
  }

  // Find the locations of the uniforms that we are using.
  uniform_loc_tex0 = glGetUniformLocation(program, "texture0");
  uniform_loc_tex1 = glGetUniformLocation(program, "texture1");
  uniform_loc_tex2 = glGetUniformLocation(program, "texture2");
  assert(uniform_loc_tex0 != -1);
  assert(uniform_loc_tex1 != -1);
  assert(uniform_loc_tex2 != -1);

  OglErrorCheck("prepare fragment shader");
}


/*
 * This piece of code sets up a frame buffer object, associated with a texture.
 * We can render a depth-only image as seen from the light, into this texture.
 */
void OglInitShadowing(const std::string &dirprefix)
{
  // create texture for shadow map
  glGenTextures(1, &light_view_depth_texture);
  glBindTexture(GL_TEXTURE_2D, light_view_depth_texture);
  glEnable(GL_DEPTH_TEST);
  GLint bits;
  glGetIntegerv(GL_DEPTH_BITS, &bits);
  glTexImage2D
  (
    GL_TEXTURE_2D,
    0,
    (bits==16) ? GL_DEPTH_COMPONENT16_ARB : GL_DEPTH_COMPONENT24_ARB,
    shadowmapsize, shadowmapsize, 0,
    GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, 0
  );
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE_ARB, GL_COMPARE_R_TO_TEXTURE_ARB);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC_ARB, GL_LEQUAL);
  OglErrorCheck("creating shadow texture");

  // create texture for spotlight image
  spotlight_texture = new ssgTexture(std::string(dirprefix+"/images/spot.tga").c_str(), false, false, false);

  OglErrorCheck("creating spotlight texture");

  // Get the addresses of the extended functionality.
  PFNGLGENFRAMEBUFFERSEXTPROC glGenFramebuffers = (PFNGLGENFRAMEBUFFERSEXTPROC) glXGetProcAddressARB((const GLubyte *) "glGenFramebuffersEXT");
  PFNGLBINDFRAMEBUFFEREXTPROC glBindFramebuffer = (PFNGLBINDFRAMEBUFFEREXTPROC) glXGetProcAddressARB((const GLubyte *) "glBindFramebufferEXT");
  PFNGLFRAMEBUFFERTEXTURE2DEXTPROC glFramebufferTexture2D = (PFNGLFRAMEBUFFERTEXTURE2DEXTPROC) glXGetProcAddressARB((const GLubyte *) "glFramebufferTexture2DEXT");
  PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC glCheckFramebufferStatus = (PFNGLCHECKFRAMEBUFFERSTATUSEXTPROC) glXGetProcAddressARB((const GLubyte *) "glCheckFramebufferStatusEXT");

  assert(glGenFramebuffers);
  assert(glBindFramebuffer);
  assert(glFramebufferTexture2D);
  assert(glCheckFramebufferStatus);

  // create fbo
  glGenFramebuffers(1, &frame_buffer_object);
  glBindFramebuffer(GL_FRAMEBUFFER_EXT, frame_buffer_object);
  // Attach texture to fbo
  glFramebufferTexture2D
  (
    GL_FRAMEBUFFER_EXT,
    GL_DEPTH_ATTACHMENT_EXT,
    GL_TEXTURE_2D,
    light_view_depth_texture,
   0
  );

  // We use depth render only
  glDrawBuffer(GL_NONE);
  glReadBuffer(GL_NONE);

  GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER_EXT);
  assert(status != GL_FRAMEBUFFER_UNSUPPORTED_EXT);
  //fprintf(stderr,"status %x\n", status);
  assert(status == GL_FRAMEBUFFER_COMPLETE_EXT);

  // disable the frame buffer object
  glBindFramebuffer(GL_FRAMEBUFFER_EXT, 0);
  OglErrorCheck("creating frame buffer object");

  OglPrepareShaders(dirprefix);
}


static void OglSetLinearTexGen(GLenum plane, GLenum mode)
{
  // eye linear
  GLfloat xx[4]={1,0,0,0};
  GLfloat yy[4]={0,1,0,0};
  GLfloat zz[4]={0,0,1,0};
  GLfloat ww[4]={0,0,0,1};
  glTexGenfv(GL_S, plane, xx);
  glTexGenfv(GL_T, plane, yy);
  glTexGenfv(GL_R, plane, zz);
  glTexGenfv(GL_Q, plane, ww);
  glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, mode);
  glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, mode);
  glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, mode);
  glTexGeni(GL_Q, GL_TEXTURE_GEN_MODE, mode);
  glEnable(GL_TEXTURE_GEN_S);
  glEnable(GL_TEXTURE_GEN_T);
  glEnable(GL_TEXTURE_GEN_R);
  glEnable(GL_TEXTURE_GEN_Q);
}


void OglRenderShadowMap(ssgRoot *scene)
{
  // Prepare shadow rendering
  int port[4];
  glGetIntegerv(GL_VIEWPORT, port);
  PFNGLBINDFRAMEBUFFEREXTPROC glBindFramebuffer = (PFNGLBINDFRAMEBUFFEREXTPROC) glXGetProcAddressARB((const GLubyte *) "glBindFramebufferEXT");
  glBindFramebuffer(GL_FRAMEBUFFER_EXT, frame_buffer_object);
  glViewport(0,0, shadowmapsize,shadowmapsize);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // depth only?
//  glPolygonOffset(2.5, 10);
  glPolygonOffset(1.1, 4.0);
  glEnable(GL_POLYGON_OFFSET_FILL);
  OglErrorCheck("prepare shadow rendering");

  // render scene from light view

  glActiveTextureARB(GL_TEXTURE1_ARB);

  glPushMatrix();
  OglSetLinearTexGen(GL_EYE_PLANE, GL_EYE_LINEAR);
  glPopMatrix();

  glMatrixMode(GL_TEXTURE);
  glLoadIdentity();
  glTranslatef(0.5f, 0.5f, 0.5f);
  glScalef(0.5f, 0.5f, 0.5f);
  gluPerspective(spot_fov, 1, 0.1, 150.0);
  glMatrixMode(GL_MODELVIEW);

  //spotlight_texture->Activate();
  glBindTexture(GL_TEXTURE_2D, spotlight_texture->getHandle());

  glEnable(GL_TEXTURE_2D);
  glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

  // Render the scene
  glActiveTextureARB(GL_TEXTURE0_ARB);
  OglSetLinearTexGen(GL_OBJECT_PLANE, GL_OBJECT_LINEAR);
  ssgCullAndDraw(scene);
  OglErrorCheck("ssgCullAndDraw");

  glActiveTextureARB(GL_TEXTURE1_ARB);
  glBindTexture(GL_TEXTURE_2D, 0);
  glDisable(GL_TEXTURE_2D);
  glActiveTextureARB(GL_TEXTURE0_ARB);
  glBindTexture(GL_TEXTURE_2D, 0);
  glDisable(GL_TEXTURE_2D);

  // reset viewport and polygon offset to original values.
  glDisable(GL_POLYGON_OFFSET_FILL);
  glBindFramebuffer(GL_FRAMEBUFFER_EXT, 0);
  glViewport(port[0], port[1], port[2], port[3]);
  OglErrorCheck("render_shadow_map");
}


void OglSetupSecondPass(float *mat_cam, float *mat_spt)
{
  static PFNGLUSEPROGRAMPROC glUseProgram = (PFNGLUSEPROGRAMPROC) glXGetProcAddressARB((const GLubyte *) "glUseProgram");
  static PFNGLUNIFORM1IPROC  glUniform1i  = (PFNGLUNIFORM1IPROC)  glXGetProcAddressARB((const GLubyte *) "glUniform1i");

  assert(glUseProgram);

  // 2nd pass: with shadow texture
  glUseProgram(program);
  glActiveTextureARB(GL_TEXTURE1_ARB);
  glUniform1i(uniform_loc_tex1, 1);
  glPushMatrix();
  glMultMatrixf((GLfloat*) mat_cam);
  OglSetLinearTexGen(GL_EYE_PLANE, GL_EYE_LINEAR);
  glPopMatrix();
  glMatrixMode(GL_TEXTURE);
  glLoadIdentity();
  glTranslatef(0.5f,0.5f,0.5f);
  glScalef(0.5f,0.5f,0.5f);
  gluPerspective(spot_fov, 1, 1.0, 150.0);
  glMultMatrixf((GLfloat*) mat_spt);
  glMatrixMode(GL_MODELVIEW);
  //spotlight_texture->Activate();
  glBindTexture(GL_TEXTURE_2D, spotlight_texture->getHandle());

  glEnable(GL_TEXTURE_2D);
  glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

  glActiveTextureARB(GL_TEXTURE2_ARB);
  glUniform1i(uniform_loc_tex2, 2);
  glPushMatrix();
  glMultMatrixf((GLfloat*) mat_cam);
  OglSetLinearTexGen(GL_EYE_PLANE, GL_EYE_LINEAR);
  glPopMatrix();
  glMatrixMode(GL_TEXTURE);
  glLoadIdentity();
  glTranslatef(0.5f, 0.5f, 0.5f); // r coord scale
  glScalef(0.5f, 0.5f, 0.5f); // r coord bias
  gluPerspective(spot_fov, 1, 1.0f, 150.0f);
  glMultMatrixf((GLfloat*) mat_spt);
  OglErrorCheck("setup texture matrix");

  glMatrixMode(GL_MODELVIEW);
  glBindTexture(GL_TEXTURE_2D, light_view_depth_texture);
  glEnable(GL_TEXTURE_2D);
  glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
 
  glActiveTextureARB(GL_TEXTURE0_ARB);
  glUniform1i(uniform_loc_tex0, 0);
  OglSetLinearTexGen(GL_OBJECT_PLANE, GL_OBJECT_LINEAR);
}

