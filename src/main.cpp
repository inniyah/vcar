/*
 * based on stormbaancoureur
 * (c) 2006,2007,2008 by Bram Stolk
 * bram at gmail.com
 * LICENSED ACCORDING TO THE GPLV3
 */

#include <assert.h>
#include <libgen.h>	// for dirname(), Is this portable?

#define GL_GLEXT_PROTOTYPES 1
#define GLX_GLXEXT_PROTOTYPES 1

#include <GL/glx.h>
#include <GL/glxext.h>	// for the opengl extensions we use for hardware accelerated shadowing

#include <GL/gl.h>
#include <GL/glut.h>

#include <plib/ssg.h>
#include <plib/ul.h>

#include "ogl.h"
#include "staticworldobject.h"
#include "carobject.h"
#include "trackingcam.h"
#include "usercam.h"
#include "dynamicboxobject.h"
#include "dynamiccylobject.h"

#include "stereocontext.h"
#include "controller.h"
#include "soundenginealsa.h"
#include "modelmap.h"
#include "scenarios.h"
#include "starsky.h"

static float aspectratio=1.0;
static int winw=704, winh=300;
//static int winw=480, winh=360;
static ssgRoot           * scene             = NULL;
static ssgContext        * monocontext       = NULL;
static StereoContext     * stereocontext     = NULL;
static ssgContext        * lightviewcontext  = NULL;
static float fps         = 60.0;
static float gametime    = 0.0;
static float spawntime   = 0.0;
static float dt_hist[10] = {0.016,0.016,0.016,0.016,0.016,0.016,0.016,0.016,0.016,0.016};
static std::string dirprefix;

static dJointGroupID contactgroup;
static dSpaceID bigspace;
static dSpaceID staticspace;
static dWorldID world;

static PracticeScenario  * practice_scenario = NULL;
static RallyScenario     * rally_scenario    = NULL;
static RespawnPoint      * respawn_point_hit = NULL;

static ControllerPad     * controller_pad    = NULL;
static ControllerKey     * controller_key    = NULL;
static Controller        * controller        = NULL;
static CarObject         * car               = NULL;
static UserCam           * trackingcam       = NULL;
static int numcars=0;

static SoundEngineAlsa   * sengine           = NULL;
static bool capture=false;

static ModelMap          * modelmap          = NULL;
static StarSky           * starsky           = NULL;

static bool do_shadows;

static void stop_game(void);
static void start_game(const std::string &gametype);
static void restart_game(void);
static bool respawn(void);



static void near_callback(void *data, dGeomID o1, dGeomID o2) {
  assert(o1);
  assert(o2);

  if (dGeomIsSpace(o1) || dGeomIsSpace(o2)) {
    // colliding a space with something
    dSpaceCollide2(o1,o2,data,&near_callback);
    // Note we do not want to test intersections within a space,
    // only between spaces.
    return;
  }

  // Two non space geoms

  WorldObject *wo1 = static_cast<WorldObject*>(dGeomGetData(o1));
  WorldObject *wo2 = static_cast<WorldObject*>(dGeomGetData(o2));

  const int N = 32;
  dContact contact[N];
  int n = dCollide (o1,o2,N,&(contact[0].geom),sizeof(dContact));

  if (n > 0) {
    assert(wo1);
    assert(wo2);

    if (wo1->name == "respawnpoint") {
      respawn_point_hit = dynamic_cast<RespawnPoint*>(wo1);
    }

    if (wo2->name == "respawnpoint") {
      respawn_point_hit = dynamic_cast<RespawnPoint*>(wo2);
    }

    // This section is req'd to see if the backwheels are touching
    // a dynamic object. If so, it could be that our car is riding
    // on top of an object. In this case, it should be possible to
    // engage the reverse-gear when the car is stationary relative
    // to the ground, and yet may be moving in absolute space.
    if (car->IsBackWheelGeometry(o1)) {
      DynamicObject *dynamic_ground = dynamic_cast<DynamicObject*>(wo2);
      if (dynamic_ground) {
        car->SetGround(dynamic_ground);
      } else {
        car->SetGround(0);
      }
    }

    if (car->IsBackWheelGeometry(o2)) {
      DynamicObject *dynamic_ground = dynamic_cast<DynamicObject*>(wo1);
      if (dynamic_ground) {
        car->SetGround(dynamic_ground);
      } else {
        car->SetGround(0);
      }
    }

    bool is_tyre=false;
    sgVec3 wheel_dir={0,0,0};
    float wheel_vel=0.0f;

    if (car->IsWheelGeometry(o1,wheel_vel)) {
      const dReal *m = dGeomGetRotation(o1);
      sgSetVec3(wheel_dir, m[2],m[6],m[10]);
      is_tyre=true;
    }

    if (car->IsWheelGeometry(o2,wheel_vel)) {
      const dReal *m = dGeomGetRotation(o2);
      sgSetVec3(wheel_dir, m[2],m[6],m[10]);
      is_tyre=true;
    }

#if 0
    if (is_tyre)
      fprintf(stderr,"dir %5.2f %5.2f %5.2f\n", wheel_dir[0], wheel_dir[1], wheel_dir[2]);
#endif

    // Create constraints for all the contacts between the geometries.
    for (int i=0; i<n; i++) {
      contact[i].surface.mu = 50.0;
      //contact[i].surface.soft_erp = 0.985;
      //contact[i].surface.soft_cfm = 0.020;
      //contact[i].surface.mode = dContactSoftERP | dContactSoftCFM | dContactApprox1;
      contact[i].surface.mode = dContactApprox1;

      if (is_tyre) {
        contact[i].fdir1[0]=wheel_dir[0];
        contact[i].fdir1[1]=wheel_dir[1];
        contact[i].fdir1[2]=wheel_dir[2];
        contact[i].surface.slip1 = 0.0250f * wheel_vel; // slip perpendicular to roll direction
        contact[i].surface.slip2 = 0.0005f * wheel_vel; // slip in roll direction
        contact[i].surface.mode = contact[i].surface.mode | dContactFDir1 | dContactSlip1 | dContactSlip2;
#if 0
        contact[i].surface.soft_erp = 0.95;
        contact[i].surface.soft_cfm = 0.05;
        contact[i].surface.mode = contact[i].surface.mode | dContactSoftERP | dContactSoftCMF;
#endif
        contact[i].surface.mu  = 340.0;
        contact[i].surface.mu2 = 340.0;
      }

      if ((wo1->name == "ferriswheelcart" || wo2->name == "ferriswheelcart") && is_tyre) {
        contact[i].surface.mu  *= 2;
        contact[i].surface.mu2 *= 2;
        contact[i].surface.slip1 *= 0.01;
        contact[i].surface.slip2 *= 0.01;
      }

      dJointID c = dJointCreateContact (world,contactgroup,&contact[i]);
      dGeomID g1 = contact[i].geom.g1;
      dGeomID g2 = contact[i].geom.g2;
      dBodyID b1 = dGeomGetBody(g1);
      dBodyID b2 = dGeomGetBody(g2);
      assert(b1 || b2);
      dJointAttach (c, b1, b2);
    }
  }
}


static void engine_noise(float dt) {
  float vl = car->GetWheelVelocity(2);
  float vr = car->GetWheelVelocity(3);
  float v = std::max(vl,vr);
  float a = car->GetAccelerator();
  float d = car->GetBrake();
  float e = std::max(a,d);
  v = v/15.0;
  if (v>3) v=3;
  sengine->SetModulation(1+v+e, 1+v/5+e/2);
  sengine->SetLowPass(0.97);
  return;
}

#if 0
void make_menu_choice(void)
{
  const std::string &choice = gui->SelectChoice();
  if (choice == "ATTACK STORMBAAN")
    start_game("full");
  if (choice == "PRACTICE")
    start_game("practice");
  if (choice == "TEST GROUND")
    start_game("rally");
}
#endif

static void idle(void) {
  static ulClock clk;
  static int frameCounter = 0;
  clk.setMaxDelta(1.0);

  float elapsed = clk.getDeltaTime();
  clk.update();

  dt_hist[frameCounter] = elapsed;
  frameCounter = (frameCounter+1)%10;

  float dt=0;
  int i;
  for (i=0; i<10; i++)
    dt += dt_hist[i];
  dt = dt / 10;
  assert(dt>0.0);
  // do not update the fps every time, it is not readable when it is done each tick
  if (frameCounter == 0) 
    fps = 1.0f / dt;
  if (dt > 0.125) dt=0.125; // Do not accomodate systems slower than 8 fps

  if (controller_pad) controller_pad->Sustain(dt);
  if (controller_key) controller_key->Sustain(dt);

  // Feed the controls into the car
  float ste = controller->GetSteer();
  float acc = controller->GetAccel();
  float bra = controller->GetBrake();
  bool  act = controller->GetAction();
  bool  ebr = controller->GetEBrake();

  assert(car!=NULL);
  car->SetSteer(ste);
  car->SetAccelerator(acc);
  car->SetBrake(bra);
  car->SetAction(act);
  car->SetEBrake(ebr);

  gametime += elapsed;
  spawntime += elapsed;
  if (sengine) {
    engine_noise(dt);
  }

  float timestep=0.005555; // Use 5.555ms timesteps
  static float remaining_sim_time=0;
  remaining_sim_time += dt;

  const float SLOWMOTION=1.0f;
  while (remaining_sim_time > timestep) {
    dSpaceCollide (bigspace, 0, &near_callback);
    if (car) car->Sustain(SLOWMOTION*timestep);
    if (practice_scenario)
      practice_scenario->Sustain(SLOWMOTION*timestep, car->GetAction());
    if (rally_scenario)
      rally_scenario->Sustain(SLOWMOTION*timestep, car->GetAction());
    dWorldQuickStep (world, SLOWMOTION*timestep);
    dJointGroupEmpty (contactgroup);
    remaining_sim_time -= timestep;
  }

  // Check lose condition
  sgVec3 carpos;
  car->GetPos(carpos);
  if (carpos[2] < -4) {
    restart_game();
    return;
  }

  if (respawn_point_hit) {
    car->SnapShot(false);
    gametime += 60.0f; // penalty for taking the easy way out
    respawn_point_hit->Disable();
    respawn_point_hit=0;
  }

  trackingcam->Update(dt);
  sgVec3 eye, coi, up;
  sgSetVec3(up,0,0,1);
  trackingcam->GetCameraPos(eye);
  trackingcam->GetTargetPos(coi);

  if (monocontext) {
    monocontext->setCameraLookAt(eye, coi, up);
  }

  if (stereocontext) {
    stereocontext->SetCameraLookAt(eye, coi, up);
    OglErrorCheck("stereocontext->SetCameraLookAt");
  }

  ssgLight *light = ssgGetLight(0);
  if (do_shadows) {
    eye[0]=  0.0;
    eye[1]= 40.0;
    eye[2]= 90.0;
  } else {
    car->GetPos(coi);
    car->GetPos(eye);
    eye[2] += 6.5;
  }

  sgVec3 car_dir;
  car->GetDir(car_dir);
  sgAddVec3(coi, coi, car_dir);
  sgVec3 light_dir;
  sgSubVec3(light_dir, coi, eye);
  light->setPosition(eye[0],eye[1],eye[2]);
  light->setSpotDirection(light_dir[0],light_dir[1],light_dir[2]);
  sgSetVec3(up,1,0,0);
  lightviewcontext->setCameraLookAt(eye, coi, up);
}


static void redraw_single_channel(void) {
  if (scene) {
    if (do_shadows) {
      // 1st pass from light view

      lightviewcontext->makeCurrent();
      OglRenderShadowMap(scene);

      sgMat4 mat_cam, mat_spt;
      if (monocontext) {
        monocontext->makeCurrent();
        monocontext->getModelviewMatrix(mat_cam);
      }
      if (stereocontext) {
        stereocontext->MakeCurrent("last");
        stereocontext->getModelviewMatrix(mat_cam);
      }
      lightviewcontext->getModelviewMatrix(mat_spt);

      OglSetupSecondPass((float*)mat_cam, (float*)mat_spt);
    }

    ssgCullAndDraw(scene);
    OglErrorCheck("ssgCullAndDraw");

    if (do_shadows) {
      glUseProgram(0);
      // Turn off all texture units
      glActiveTextureARB(GL_TEXTURE1_ARB);
      glBindTexture(GL_TEXTURE_2D, 0);
      glDisable(GL_TEXTURE_2D);
      glActiveTextureARB(GL_TEXTURE2_ARB);
      glBindTexture(GL_TEXTURE_2D, 0);
      glDisable(GL_TEXTURE_2D);
      glActiveTextureARB(GL_TEXTURE0_ARB);
      glBindTexture(GL_TEXTURE_2D, 0);
      glDisable(GL_TEXTURE_2D);
      OglErrorCheck("Turn off texture units");
    }
  }
}


static void redraw(void) {
  glClearColor(0.2,0.1,0.35,1);

  if (stereocontext) {
    // passive stereo: clear buffers only once
    if (!stereocontext->IsQuadBuffered())
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT ) ;
    stereocontext->MakeCurrent("left");
    OglErrorCheck("MakeCurrent");
#if 1
    // quad buffer stereo: clear buffers twice
    if (stereocontext->IsQuadBuffered())
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT ) ;
#endif
    redraw_single_channel();
    stereocontext->MakeCurrent("right");
    OglErrorCheck("MakeCurrent");
#if 1
    // quad buffer stereo: clear buffers twice
    if (stereocontext->IsQuadBuffered())
      glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT ) ;
#endif
    redraw_single_channel();
  }

  if (monocontext) {
    monocontext->makeCurrent();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT ) ;
    redraw_single_channel();
  }

  glutSwapBuffers () ;
  OglErrorCheck("glutSwapBuffers");

  if (capture) {
    static char *buf = new char[winw * winh * 3];
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glReadBuffer(GL_BACK_LEFT);
    glReadPixels(0,0,winw,winh,GL_RGB,GL_UNSIGNED_BYTE,(GLvoid*)(buf));
    OglErrorCheck("glReadPixels");
    static FILE *f=0;

    if (!f) {
      f=fopen("capture.out", "wb");
      assert(f);
 //        fprintf(f,"P6 %d %d 255\n", winw,winh);
    }
    unsigned char *revbuf = new unsigned char[winw*winh*3];
    for (int y=0; y<winh; y++) {
      int chunk = 3 * winw;
      memcpy(revbuf + y*chunk, buf + (winh - 1 - y)*chunk, chunk);
    }
    fwrite(revbuf,winw*winh*3,1,f);
    fflush(f);
    delete revbuf;
  }
  // We want to render the next frame immediately
  glutPostRedisplay () ;
}


static void reshape(int w, int h) {
  winw=w; winh=h;
  if (monocontext) {
    glViewport ( 0, 0, w, h ) ;
    aspectratio = w / (float) h;
    float fovy = 40.0 * M_PI / 180.0;
    float nearPlaneDistance = 0.28;
    float farPlaneDistance  = 2500.0;
    float y = tan(0.5 * fovy) * nearPlaneDistance;
    float x = aspectratio * y;
    monocontext->setFrustum(-x,x,-y,y,nearPlaneDistance,farPlaneDistance);
  }

  if (stereocontext) {
    stereocontext->SetWindowSize(winw, winh);
  }

#if 1
  int nsamples = glutGet(GLUT_WINDOW_NUM_SAMPLES);
#else
  int nsamples;
  glGetIntegerv(GL_SAMPLES, &nsamples);
#endif
  fprintf(stderr,"Number of samples per pixel: %d\n", nsamples);
}


static void keyboardUp(unsigned char k, int, int) {
  if (controller_key) controller_key->FeedKey(k, false);
}


static void special(int k, int, int)
{
  if (k == GLUT_KEY_UP) {
    if (controller_key) controller_key->FeedAction("ACCEL", true);
  }
  if (k == GLUT_KEY_DOWN) {
    if (controller_key) controller_key->FeedAction("BRAKE", true);
  }
  if (k == GLUT_KEY_LEFT) {
    if (controller_key) controller_key->FeedAction("LEFT", true);
  }
  if (k == GLUT_KEY_RIGHT) {
     if (controller_key) controller_key->FeedAction("RIGHT", true);
  }
}


static void specialUp(int k, int, int)
{
  if (k == GLUT_KEY_UP) {
     if (controller_key) controller_key->FeedAction("ACCEL", false);
  }
  if (k == GLUT_KEY_DOWN) {
    if (controller_key) controller_key->FeedAction("BRAKE", false);
  }
  if (k == GLUT_KEY_LEFT) {
    if (controller_key) controller_key->FeedAction("LEFT", false);
  }
  if (k == GLUT_KEY_RIGHT) {
    if (controller_key) controller_key->FeedAction("RIGHT", false);
  }
}


static int mouse_x, mouse_y, mouse_but;
static void mouse(int button, int state, int x, int y) {
  mouse_x = x;
  mouse_y = y;
  if (state == GLUT_DOWN) mouse_but = button;
}


static void motion(int x, int y) {
  if (!trackingcam) return;
  float dx = (x - mouse_x) / (float) winw;
  float dy = (y - mouse_y) / (float) winh;

  if (mouse_but == GLUT_LEFT_BUTTON) {
    trackingcam->ChangeHeading(100.0 * dx);
    trackingcam->ChangePitch(100.0 * dy);
  }
  if (mouse_but == GLUT_MIDDLE_BUTTON) {
    trackingcam->ChangeOffset(dx, dy);
  }
  if (mouse_but == GLUT_RIGHT_BUTTON) {
    trackingcam->ChangeDistance(-10.0 * dy);
  }

  mouse_x = x;
  mouse_y = y;
}


static void start_game(const std::string &gametype) {
  gametime = 0.0;
  spawntime = 0.0;

  // Create ODE world
  dInitODE();
  world = dWorldCreate();
  bigspace = dHashSpaceCreate(0);
  staticspace = dSimpleSpaceCreate(bigspace);
  contactgroup = dJointGroupCreate (0);
#if 0
  dWorldSetERP(world, 0.95f);
  dWorldSetCFM(world, 0.001f);
#endif
  dWorldSetGravity(world,0,0,-9.8);
  dWorldSetAutoDisableFlag(world, true);
  dWorldSetAutoDisableLinearThreshold(world, 0.04);
  dWorldSetAutoDisableAngularThreshold(world, 0.04);
  dWorldSetQuickStepNumIterations(world, 16);

  // Create PLIB world

  scene = new ssgRoot();

  if (gametype=="practice") {
    practice_scenario = new PracticeScenario(world, bigspace, staticspace, scene, modelmap, dirprefix);
  }

  if (gametype=="rally") {
    rally_scenario = new RallyScenario(world, bigspace, staticspace, scene, modelmap, dirprefix);
  }

  // Create a car
  sgVec3 carpos;

  if (practice_scenario)
    sgSetVec3(carpos, -4, 0, 1.5);

  if (rally_scenario)
    sgSetVec3(carpos, -28, 0, 1.5);

  car = new CarObject (
    modelmap->Get("frame.3ds"),
    modelmap->Get("fivespoke.3ds"),
    modelmap->Get("wishbone.3ds"),
    modelmap->Get("spindle.3ds"),
    modelmap->Get("coilspring.3ds"),
    modelmap->Get("leafspring.3ds"),
    modelmap->Get("rearaxle.3ds"),
    world,
    bigspace,
    carpos
  );
  scene->addKid(car->GetEntity());
  numcars = 1;

  // Setup camera
  trackingcam = new UserCam(0,0,0);
  trackingcam->AddTarget(car->GetTransform());
  trackingcam->SetPitch(16.5f);
  trackingcam->SetDistance(4.0f);

  if (sengine) {
    sengine->SetMode("engine");
    sengine->Play(dirprefix + "/sounds/detonationnorm_s16_le.wav",0);
  }
}


static void stop_game(void) {
  delete car;
  delete trackingcam;

  if (sengine) {
    sengine->SetMode("none");
  }

  delete practice_scenario;
  practice_scenario=NULL;
  delete rally_scenario;
  rally_scenario=NULL;

  // Delete ODE stuff
  dJointGroupDestroy(contactgroup);
  dSpaceDestroy(staticspace);
  dSpaceDestroy(bigspace);
  dWorldDestroy(world);

  dCloseODE();
}


static bool respawn(void) {
  spawntime=0.0f;
  bool restored = car->SnapShot(true);
  if (!restored) {
    return false;
  }
  return true;
}


static void restart_game(void) {
  const char *gametype="";
  if (practice_scenario) gametype="practice";
  if (rally_scenario) gametype="rally";
  bool respawned = respawn();
  if (!respawned) {
    stop_game();
    start_game(gametype);
  }
}


static void keyboard(unsigned char k, int, int) {
  if (k==27) {
    if (spawntime>1.0f)
      restart_game();
  }

  if (controller_key) controller_key->FeedKey(k, true);
}

int main(int argc, char *argv[]) {
  std::string displaymode="monoscopic";

  dirprefix = DATADIR;

  if (getenv("PLODE_DATADIR")) {
    dirprefix = getenv("PLODE_DATADIR");
  }

  if (getenv("PLODE_DISPLAYMODE")) {
    displaymode = getenv("PLODE_DISPLAYMODE");
  }

  assert(displaymode == "monoscopic" || displaymode == "quadbufferstereoscopic" || displaymode == "passivestereoscopic");
  modelmap = new ModelMap(dirprefix);
  glutInit(&argc, argv);
  int flags = GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH | GLUT_MULTISAMPLE;
  if (displaymode == "quadbufferstereoscopic") flags = flags | GLUT_STEREO;
  glutInitDisplayMode(flags);
  glutInitWindowSize(winw, winh);
  glutInitWindowPosition(0,0);
  int winid=glutCreateWindow ("Virtual Car");
  assert(winid>=1);
  glutDisplayFunc(redraw);
  glutReshapeFunc(reshape);
  glutKeyboardFunc(keyboard);
  glutKeyboardUpFunc(keyboardUp);
  glutSpecialFunc(special);
  glutSpecialUpFunc(specialUp);
  glutMouseFunc(mouse);
  glutMotionFunc(motion);
  glutIdleFunc(idle);

#if 0
  glutFullScreen();
#else
  if (1)
  {
    int width = glutGameModeGet( GLUT_GAME_MODE_WIDTH );
    int height = glutGameModeGet( GLUT_GAME_MODE_HEIGHT );
    int pixelDepth = glutGameModeGet( GLUT_GAME_MODE_PIXEL_DEPTH );
    int refreshRate = glutGameModeGet( GLUT_GAME_MODE_REFRESH_RATE );
    fprintf(stderr,"%dx%d-%d@%d\n", width, height, pixelDepth, refreshRate);
    //glutEnterGameMode();
  }
#endif

  do_shadows = OglCanDoShadowing();
  fprintf
  (
    stderr, 
    "This platform %s all required GL extensions to do hardware accelerated shadowing.\n",
    (do_shadows) ? "supports" : "does not support"
  );

  ssgInit();

  if (do_shadows) {
    OglInitShadowing(dirprefix);
  }

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_CULL_FACE);

  float amb[4]={0.0, 0.0, 0.0, 1};
  glLightModelfv(GL_LIGHT_MODEL_AMBIENT, amb);

  starsky = new StarSky();
  modelmap->Put("starsky", starsky->GetEntity());

  controller_pad = new ControllerPad("/dev/input/js0");
  if (controller_pad->IsWorking()) {
    fprintf(stderr,"Using gamepad\n");
    controller = controller_pad;
  } else {
    delete controller_pad;
    controller_pad = 0;
    char *ev = getenv("HOME");
    controller_key = new ControllerKey(std::string((ev)?ev:"~") + "/.stormbaancoureur.keys");
    controller = controller_key;
    fprintf(stderr,"Using keyboard\n");
  }

  if (displaymode != "monoscopic") {
    stereocontext = new StereoContext(displaymode == "quadbufferstereoscopic");
  } else {
    sgVec3 eye={5.2,1.8,3.4};
    sgVec3 coi={0,0,0};
    sgVec3 up= {0,0,1};
    monocontext = new ssgContext();
    monocontext->setCameraLookAt(eye, coi, up);
    monocontext->makeCurrent();
  }
  lightviewcontext = new ssgContext();
  float fov = spot_fov * M_PI / 180.0;
  float nearPlaneDistance = 1.0f;
  float farPlaneDistance  = 150.0f;
  float s = tan(0.5 * fov) * nearPlaneDistance;
  lightviewcontext->setFrustum(-s,s,-s,s, nearPlaneDistance, farPlaneDistance);

  ssgLight *light=ssgGetLight(0);
  const float ka = (do_shadows) ? 0.15f : 0.30f;
  const float kd = (do_shadows) ? 1.00f : 2.50f;
  const float ks = 1.0f;
  light->setColour(GL_DIFFUSE,  kd,kd,kd);
  light->setColour(GL_AMBIENT,  ka,ka,ka);
  light->setColour(GL_SPECULAR, ks,ks,ks);
  light->setPosition(0,0,8);
  light->setSpotlight(true);
  light->setSpotDirection(0,0,-1);
  light->setSpotDiffusion(100,180);
  light->setSpotAttenuation(1, 0.1, 0.04);
  light->on();

  sgCoord campos ;
  sgSetCoord (&campos, 0.0f, -13.0f,  6.0f, 0.0, -34.0f, 0.0f) ;

  bool nosound = getenv("PLODE_NO_SOUND");
  if (!nosound) {
    sengine = new SoundEngineAlsa(5000);
    if (!sengine->opened) {
      delete sengine;
      sengine = NULL;
    }
  }

  start_game("rally");

  glutMainLoop();
  delete sengine;
}

