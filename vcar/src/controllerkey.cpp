/*
 * stormbaancoureur
 * (c) 2006,2007,2008 by Bram Stolk
 * bram at gmail.com
 * LICENSED ACCORDING TO THE GPLV3
 */

#include <assert.h>
#include <math.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>

#include "controller.h"


ControllerKey::ControllerKey(const std::string &map_fname) :
Controller(),
key_steerl(false),
key_steerr(false),
key_accel(false),
key_brake(false),
key_action(false),
key_ebrake(false)
{
  name = "keyboard controller";
  axes[0]=axes[1]=axes[2]=0.0;

  FILE *f = fopen(map_fname.c_str(), "r");

  if (f)
  {
    char line[256], *s;
    do
    {
      s = fgets(line, sizeof(line), f);
      if (s && s[0]!='#')
      {
        int ascii;
        char func[128];
        int rv=sscanf(line, "%d %s", &ascii, func);
        if (rv==2)
          mapping[ascii] = func;
      }
    } while(s);
    fprintf(stderr, "Custom key mapping in %s contains %d entries\n", map_fname.c_str(), (int) mapping.size());
    assert(mapping.size()>=6);
  }
  else
  {
    fprintf(stderr, "Cannot open %s\n", map_fname.c_str());
    mapping['a'] = "ACCEL";
    mapping['z'] = "BRAKE";
    mapping['y'] = "BRAKE";
    mapping[','] = "LEFT";
    mapping['.'] = "RIGHT";
    mapping[' '] = "ACTION";
    mapping['\t']= "EBRAKE";
    mapping['b'] = "EBRAKE";
  }
}


ControllerKey::~ControllerKey()
{
}


void ControllerKey::Sustain(float dt)
{
  axes[0] = 0.0;
  if (key_steerl) axes[0] -= 0.7;
  if (key_steerr) axes[0] += 0.7;
  axes[1] = (key_accel)?0.9:0.0;
  axes[2] = (key_brake)?0.9:0.0;
}


float ControllerKey::GetAxisValue(int nr) const
{
  assert(nr<3);
  return axes[nr];
}


bool ControllerKey::GetButtonValue(int nr) const
{
  if (nr == 0)
    return key_action;
  if (nr == 1)
    return key_ebrake;
  return false;
}


void ControllerKey::FeedKey(unsigned char k, bool down)
{
  std::map<unsigned char, std::string>::const_iterator ip = mapping.find(k);
  if (ip != mapping.end())
    FeedAction((*ip).second, down);
}


void ControllerKey::FeedAction(const std::string &action, bool down)
{
  if (action=="ACCEL") { key_accel  = down; return; }
  if (action=="BRAKE") { key_brake  = down; return; }
  if (action=="LEFT")  { key_steerl = down; return; }
  if (action=="RIGHT") { key_steerr = down; return; }
  if (action=="ACTION"){ key_action = down; return; }
  if (action=="EBRAKE"){ key_ebrake = down; return; }
}

