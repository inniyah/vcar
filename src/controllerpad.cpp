/*
 * stormbaancoureur main program.
 * (c) 2006,2007,2008 by Bram Stolk
 * bram at gmail.com
 * LICENSED ACCORDING TO THE GPLV3
 */

#include <assert.h>
#include <math.h>
#include <errno.h>
#include <fcntl.h>
#include <string.h>

#ifdef WIN32
#include <windows.h>
#include <mmsystem.h>
#else
#include <linux/joystick.h>
#include <sys/ioctl.h>  // for ioctl()
#include <unistd.h>     // for close()
#include <stdio.h>      // for perror()
#endif

#include "controller.h"
#include "joydb.h"


#ifdef WIN32
/* implementation functions */
void SetMMerror(char *function, int code) {
  static char *error;
  static char  errbuf[BUFSIZ];

  errbuf[0] = 0;
  switch (code) {
    case MMSYSERR_NODRIVER:
      error = "Joystick driver not present";
      break;

    case MMSYSERR_INVALPARAM:
    case JOYERR_PARMS:
      error = "Invalid parameter(s)";
      break;

    case MMSYSERR_BADDEVICEID:
      error = "Bad device ID";
      break;

    case JOYERR_UNPLUGGED:
      error = "Joystick not attached";
      break;

    case JOYERR_NOCANDO:
      error = "Can't capture joystick input";
      break;

    default:
      sprintf(errbuf, "%s: Unknown Multimedia system error: 0x%x",
        function, code);
      break;
  }

  if(!errbuf[0]) {
    sprintf(errbuf, "%s: %s", function, error);
  }

  fprintf(stderr, "%s\n", errbuf);
}
#endif


ControllerPad::ControllerPad(const std::string &devfile) :
opened(false),
fd(0),
DeadMargin(0.05),
joydb_entry(0),
steer_axis(-1),
brake_axis(-1),
brake_button(-1),
ebrake_button(-1),
action_button(-1),
accel_axis(-1),
accel_button(-1)
{
#ifdef WIN32
  JOYINFOEX joyinfo;
  JOYCAPS joycaps;
  MMRESULT result;
  int axisCounter;

  id = 0;

  joyinfo.dwSize = sizeof(joyinfo);
  joyinfo.dwFlags = JOY_RETURNALL;

  result = joyGetPosEx(id, &joyinfo);
  if(result != JOYERR_NOERROR) {
    fprintf(stderr, "No input devices found\n");
    return;
  }
  else {
    result = joyGetDevCaps(id, &joycaps, sizeof(joycaps));
    if(result != JOYERR_NOERROR) {
      fprintf(stderr, "No input devices found\n");
      return;
    }
  }

  opened = true;

  fprintf(stderr, "x min %d max %d\n", joycaps.wXmin, joycaps.wXmax);
  buttoncount = joycaps.wNumButtons;
  axiscount = joycaps.wNumAxes;
#else
  fd = open(devfile.c_str(), O_RDONLY | O_NONBLOCK);
  if (fd==-1)
  {
    if (errno != ENODEV)
      perror("open() on joystick device failed");
    return;
  }
  opened = true;

  int retval;

  int version;
  retval = ioctl(fd, JSIOCGVERSION, &version);
  if (retval == -1)
    perror("ioctl JSIOCGVERSION failed");

  unsigned char lo = version;
  unsigned char md = (version>>8);
  unsigned char hi = (version>>16);

  retval = ioctl(fd, JSIOCGAXES, &axiscount);
  if (retval == -1)
    perror("ioctl JSIOCGAXES failed");

  retval = ioctl(fd, JSIOCGBUTTONS, &buttoncount);
  if (retval == -1)
    perror("ioctl JSIOCGBUTTONS failed");

  char n[128];
  retval = ioctl(fd, JSIOCGNAME(128), n);
  if (retval == -1)
    perror("ioctl JSIOCGNAME failed");

  name = n;
  fprintf(stderr,"joystick name: %s\n", name.c_str());
  fprintf(stderr,"driver version: %d.%d.%d\n", hi,md,lo);
  fprintf(stderr,"button count: %d, axiscount: %d\n", buttoncount, axiscount);
#endif

  // search db
  assert(sizeof(joydb_names) == sizeof(joydb_descs));
  int cnt = sizeof(joydb_names) / sizeof(void *) - 1;
  for (int i=0; i<cnt && !joydb_entry; i++)
  {
    if (!strcmp(joydb_names[i], name.c_str()))
      joydb_entry = joydb_descs[i];
  }

  fprintf
  ( 
    stderr,
    "Known in joystick description database: %s\n",
    (joydb_entry)?"yes":"no"
  );

  for (int i=0; i<axiscount; i++)
    AxisValues.push_back(0);
  for (int i=0; i<buttoncount; i++)
  {
    ButtonValues.push_back(false);
    ButtonChanged.push_back(false);
  }

  Introspect();
}


ControllerPad::~ControllerPad()
{
  if (opened)
  {
#ifdef WIN32
#else
    int retval = close(fd);
    if (retval==-1)
      perror("close() failed");
#endif
  }
}


void ControllerPad::Sustain(float dt)
{
#ifdef WIN32
  MMRESULT result;
  DWORD flags[6 /*MAX_AXES*/] = { JOY_RETURNX, JOY_RETURNY, JOY_RETURNZ, 
    JOY_RETURNR, JOY_RETURNU, JOY_RETURNV };
  DWORD pos[6 /*MAX_AXES*/];
  JOYINFOEX joyinfo;

  joyinfo.dwSize = sizeof(joyinfo);
  joyinfo.dwFlags = JOY_RETURNALL | JOY_RETURNPOVCTS;

#if 0
  if(!hats) {
    joyinfo.dwFlags &= ~(JOY_RETURNPOV | JOY_RETURNPOVCTS);
  }
#endif

  // get the actual data
  result = joyGetPosEx(id, &joyinfo);

  if(result != JOYERR_NOERROR) {
    SetMMerror("joyGetPosEx", result);
    return;
  }

  /* joystick motion events */
  pos[0] = joyinfo.dwXpos;
  pos[1] = joyinfo.dwYpos;
  pos[2] = joyinfo.dwZpos;
  pos[3] = joyinfo.dwRpos;
  pos[4] = joyinfo.dwUpos;
  pos[5] = joyinfo.dwVpos;

  for(int axisCounter = 0; axisCounter < axiscount; axisCounter++) {
    if(joyinfo.dwFlags & flags[axisCounter]) {
      AxisValues[axisCounter] = float(pos[axisCounter]);
    }
  }
#else
  struct js_event ev;
  int retval;
  do
  {
    retval = read(fd, &ev, sizeof(ev));
    if (retval == -1)
    {
      if (errno != EAGAIN)
      {
        perror("read() on joystick failed");
        fprintf(stderr,"joystick read failure\n");
      }
    }
    int type = ev.type;
    switch(type)
    {
      case JS_EVENT_BUTTON:
        if (ev.number < ButtonValues.size())
        {
          ButtonValues[ev.number] = ev.value;
          ButtonChanged[ev.number] = true;
        }
        else
          fprintf(stderr,"Illegal buttonnr %d\n", ev.number);
        break;
      case JS_EVENT_AXIS:
        if (ev.number < AxisValues.size())
          AxisValues[ev.number] = ev.value;
        else
          fprintf(stderr,"Illegal axisnr %d", ev.number);
        break;
      default:
        break;
    }
  } while (retval>0);
#endif
}


float ControllerPad::GetAxisValue(int nr) const
{
  assert(nr < (int) AxisValues.size());
  assert(nr >= 0);

  float x=AxisValues[nr]/32768.0;
  if (fabsf(x)<DeadMargin)
    x=0;
  else
    x -= (x>0)?DeadMargin:-DeadMargin;

  return x / (1.0 - DeadMargin);
}


bool ControllerPad::GetButtonValue(int nr) const
{
  assert(nr < (int) ButtonValues.size());
  assert(nr >= 0);
  return ButtonValues[nr];
}


bool ControllerPad::GetButtonChanged(int nr)
{
  bool retval = ButtonChanged[nr];
  ButtonChanged[nr]=false;
  return retval;
}


bool ControllerPad::HasAnyButtonChanged(bool down)
{
  int num_generic_buttons;
  const int *gbuttons = joydb_get(JOYDB_GENERIC_BUTTONS, joydb_entry, &num_generic_buttons);

  for (int i=0; i<num_generic_buttons; i++)
  {
    int idx = gbuttons[i];
    if (ButtonChanged[idx])
      if (!down || ButtonValues[idx])
      {
         ButtonChanged[idx] = false;
         return true;
      }
  }
  return false;
}


float ControllerPad::GetSteer(void) const
{
  return GetAxisValue(steer_axis);
}


float ControllerPad::GetAccel(void) const
{
  if (accel_axis != -1)
  {
    float v = accel_polarity * GetAxisValue(accel_axis);
    if (v<0) v=0;
    return v;
  }

  if (accel_button != -1)
    return (GetButtonValue(accel_button)?1.0:0.0);

  assert(0);
  return 0.0;
}


float ControllerPad::GetBrake(void) const
{
  if (brake_axis != -1)
  {
    float v = brake_polarity * GetAxisValue(brake_axis);
    if (v<0) v=0;
    return v;
  }

  if (brake_button != -1)
    return (GetButtonValue(brake_button)?1.0:0.0);

  assert(0);
  return 0.0;
}


bool ControllerPad::GetEBrake(void) const
{
  if (ebrake_button != -1)
    return GetButtonValue(ebrake_button);
  return false;
}


bool ControllerPad::GetAction(void) const
{
  if (action_button != -1)
    return GetButtonValue(action_button);
  return false;
}


void ControllerPad::Introspect(void)
{
  steer_axis = 0;
  accel_polarity = 1;
  brake_polarity = 1;
 
  int num_foot_paddles=0;
  int num_shoulder_buttons=0;
  int num_generic_buttons=0;
  int num_shoulder_axes=0;

  if (joydb_entry)
  {
    const int *axes = joydb_get(JOYDB_FOOT_PADDLE_AXES, joydb_entry, &num_foot_paddles);
    if (num_foot_paddles >= 2)
    {
      accel_axis = axes[1];
      brake_axis = axes[0];
    }

    if (num_foot_paddles == 1)
    {
      accel_axis = brake_axis = axes[0];
      accel_polarity = -1;
    }

    const int *sbuttons = joydb_get(JOYDB_SHOULDER_BUTTONS, joydb_entry, &num_shoulder_buttons);
    if (num_shoulder_buttons >= 2)
    {
      brake_button = sbuttons[0];
      accel_button = sbuttons[num_shoulder_buttons/2];
    }

    const int *gbuttons = joydb_get(JOYDB_GENERIC_BUTTONS, joydb_entry, &num_generic_buttons);
    assert(num_generic_buttons >= 2);
    action_button = gbuttons[0];
    ebrake_button = gbuttons[1];

    axes = joydb_get(JOYDB_SHOULDER_AXES, joydb_entry, &num_shoulder_axes);
    if (num_shoulder_axes >= 2)
    {
      brake_axis = axes[0];
      accel_axis = axes[num_shoulder_axes/2];
    }
  }
  else
  {
    // We know nothing about this gamepad/joystick/wheel
    fprintf
    ( 
      stderr, 
      "Your joystick device called '%s' is not in this game's joystickdatabase.\n"
      "Please submit information on your gamepad (see JOYSTICKS file).\n"
      "In the mean while, you'll have to play via keyboard (a,z,<,>,SPACE,b)\n",
      name.c_str()
    );
    opened = false;
    return;
  }

  fprintf(stderr,"Nr of shoulder buttons = %d\n", num_shoulder_buttons);
  fprintf(stderr,"Nr of shoulder axes = %d\n", num_shoulder_axes);
  fprintf(stderr,"Nr of foot paddles = %d\n", num_foot_paddles);
  fprintf(stderr,"Nr of generic buttons = %d\n", num_generic_buttons);
}

