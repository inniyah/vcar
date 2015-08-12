/*
 * stormbaancoureur
 * (c) 2006,2007,2008 by Bram Stolk
 * bram at gmail.com
 * LICENSED ACCORDING TO THE GPLV3
 */

#include "vectortext.h"


#include <plib/ssg.h>


static void glprint_a() { glBegin(GL_POLYGON); glVertex2f(0,16); glVertex2f(0,0); glVertex2f(4,0); glVertex2f(4,16); glEnd(); glBegin(GL_POLYGON); glVertex2f(6,16); glVertex2f(6,0); glVertex2f(10,0); glVertex2f(10,16); glEnd(); glBegin(GL_POLYGON); glVertex2f(4,16); glVertex2f(4,14); glVertex2f(6,14); glVertex2f(6,16); glEnd(); glBegin(GL_POLYGON); glVertex2f(4,10); glVertex2f(4,6); glVertex2f(6,6); glVertex2f(6,10); glEnd(); }
static void glprint_b() { glBegin(GL_POLYGON); glVertex2f(0,16); glVertex2f(0,0); glVertex2f(4,0); glVertex2f(4,16); glEnd(); glBegin(GL_POLYGON); glVertex2f(4,16); glVertex2f(4,14); glVertex2f(8,14); glVertex2f(8,16); glEnd(); glBegin(GL_POLYGON); glVertex2f(6,14); glVertex2f(6,10); glVertex2f(8,10); glVertex2f(8,14); glEnd(); glBegin(GL_POLYGON); glVertex2f(4,10); glVertex2f(4,8); glVertex2f(10,8); glVertex2f(10,10); glEnd(); glBegin(GL_POLYGON); glVertex2f(6,8); glVertex2f(6,0); glVertex2f(10,0); glVertex2f(10,8); glEnd(); glBegin(GL_POLYGON); glVertex2f(4,2); glVertex2f(4,0); glVertex2f(6,0); glVertex2f(6,2); glEnd(); }
static void glprint_c() { glBegin(GL_POLYGON); glVertex2f(0,0); glVertex2f(4,0); glVertex2f(4,16); glVertex2f(0,16); glEnd(); glBegin(GL_POLYGON); glVertex2f(4,4); glVertex2f(4,0); glVertex2f(10,0); glVertex2f(10,4); glEnd(); glBegin(GL_POLYGON); glVertex2f(4,16); glVertex2f(4,12); glVertex2f(10,12); glVertex2f(10,16); glEnd(); }
static void glprint_d() { glBegin(GL_POLYGON); glVertex2f(0,16); glVertex2f(0,0); glVertex2f(4,0); glVertex2f(4,16); glEnd(); glBegin(GL_POLYGON); glVertex2f(4,16); glVertex2f(4,12); glVertex2f(6,12); glVertex2f(8,16); glEnd(); glBegin(GL_POLYGON); glVertex2f(6,12); glVertex2f(7,4); glVertex2f(10,0); glVertex2f(8,16); glEnd(); glBegin(GL_POLYGON); glVertex2f(4,4); glVertex2f(4,0); glVertex2f(10,0); glVertex2f(7,4); glEnd(); }
static void glprint_e() { glBegin(GL_POLYGON); glVertex2f(0,16); glVertex2f(0,0); glVertex2f(4,0); glVertex2f(4,16); glEnd(); glBegin(GL_POLYGON); glVertex2f(4,16); glVertex2f(4,12); glVertex2f(10,12); glVertex2f(10,16); glEnd(); glBegin(GL_POLYGON); glVertex2f(4,10); glVertex2f(4,6); glVertex2f(6,6); glVertex2f(6,10); glEnd(); glBegin(GL_POLYGON); glVertex2f(4,4); glVertex2f(4,0); glVertex2f(10,0); glVertex2f(10,4); glEnd(); }
static void glprint_f() { glBegin(GL_POLYGON); glVertex2f(0,16); glVertex2f(0,0); glVertex2f(4,0); glVertex2f(4,16); glEnd(); glBegin(GL_POLYGON); glVertex2f(4,16); glVertex2f(4,12); glVertex2f(10,12); glVertex2f(10,16); glEnd(); glBegin(GL_POLYGON); glVertex2f(4,10); glVertex2f(4,6); glVertex2f(8,6); glVertex2f(8,10); glEnd(); }
static void glprint_g() { glBegin(GL_POLYGON); glVertex2f(0,16); glVertex2f(0,0); glVertex2f(4,0); glVertex2f(4,16); glEnd(); glBegin(GL_POLYGON); glVertex2f(4,16); glVertex2f(4,12); glVertex2f(10,12); glVertex2f(10,16); glEnd(); glBegin(GL_POLYGON); glVertex2f(6,10); glVertex2f(6,6); glVertex2f(10,6); glVertex2f(10,10); glEnd(); glBegin(GL_POLYGON); glVertex2f(8,6); glVertex2f(8,3); glVertex2f(10,3); glVertex2f(10,6); glEnd(); glBegin(GL_POLYGON); glVertex2f(4,3); glVertex2f(4,0); glVertex2f(10,0); glVertex2f(10,3); glEnd(); }
static void glprint_h() { glBegin(GL_POLYGON); glVertex2f(0,16); glVertex2f(0,0); glVertex2f(4,0); glVertex2f(4,16); glEnd(); glBegin(GL_POLYGON); glVertex2f(4,10); glVertex2f(4,6); glVertex2f(6,6); glVertex2f(6,10); glEnd(); glBegin(GL_POLYGON); glVertex2f(6,16); glVertex2f(6,0); glVertex2f(10,0); glVertex2f(10,16); glEnd(); }
static void glprint_i() { glBegin(GL_POLYGON); glVertex2f(0,16); glVertex2f(0,12); glVertex2f(10,12); glVertex2f(10,16); glEnd(); glBegin(GL_POLYGON); glVertex2f(0,4); glVertex2f(0,0); glVertex2f(10,0); glVertex2f(10,4); glEnd(); glBegin(GL_POLYGON); glVertex2f(3,12); glVertex2f(3,4); glVertex2f(7,4); glVertex2f(7,12); glEnd(); }
static void glprint_j() { glBegin(GL_POLYGON); glVertex2f(6,0); glVertex2f(10,0); glVertex2f(10,16); glVertex2f(6,16); glEnd(); glBegin(GL_POLYGON); glVertex2f(0,4); glVertex2f(0,0); glVertex2f(6,0); glVertex2f(6,4); glEnd(); }
static void glprint_k() { glBegin(GL_POLYGON); glVertex2f(0,16); glVertex2f(0,0); glVertex2f(4,0); glVertex2f(4,16); glEnd(); glBegin(GL_POLYGON); glVertex2f(6,16); glVertex2f(4,10); glVertex2f(4,8); glVertex2f(7,8); glVertex2f(10,16); glEnd(); glBegin(GL_POLYGON); glVertex2f(4,8); glVertex2f(4,6); glVertex2f(6,0); glVertex2f(10,0); glVertex2f(7,8); glEnd(); }
static void glprint_l() { glBegin(GL_POLYGON); glVertex2f(0,16); glVertex2f(0,0); glVertex2f(4,0); glVertex2f(4,16); glEnd(); glBegin(GL_POLYGON); glVertex2f(4,4); glVertex2f(4,0); glVertex2f(10,0); glVertex2f(10,4); glEnd(); }
static void glprint_m() { glBegin(GL_POLYGON); glVertex2f(0,16); glVertex2f(0,0); glVertex2f(4,0); glVertex2f(4,16); glEnd(); glBegin(GL_POLYGON); glVertex2f(4,16); glVertex2f(4,12); glVertex2f(5,10); glVertex2f(5,14); glEnd(); glBegin(GL_POLYGON); glVertex2f(5,14); glVertex2f(5,10); glVertex2f(6,12); glVertex2f(6,16); glEnd(); glBegin(GL_POLYGON); glVertex2f(6,16); glVertex2f(6,0); glVertex2f(10,0); glVertex2f(10,16); glEnd(); }
static void glprint_n() { glBegin(GL_POLYGON); glVertex2f(0,16); glVertex2f(0,0); glVertex2f(4,0); glVertex2f(4,16); glEnd(); glBegin(GL_POLYGON); glVertex2f(4,16); glVertex2f(4,8); glVertex2f(6,0); glVertex2f(6,8); glEnd(); glBegin(GL_POLYGON); glVertex2f(6,16); glVertex2f(6,0); glVertex2f(10,0); glVertex2f(10,16); glEnd(); }
static void glprint_o() { glBegin(GL_POLYGON); glVertex2f(0,16); glVertex2f(0,0); glVertex2f(4,0); glVertex2f(4,16); glEnd(); glBegin(GL_POLYGON); glVertex2f(4,16); glVertex2f(4,12); glVertex2f(6,12); glVertex2f(6,16); glEnd(); glBegin(GL_POLYGON); glVertex2f(4,4); glVertex2f(4,0); glVertex2f(6,0); glVertex2f(6,4); glEnd(); glBegin(GL_POLYGON); glVertex2f(6,16); glVertex2f(6,0); glVertex2f(10,0); glVertex2f(10,16); glEnd(); }
static void glprint_p() { glBegin(GL_POLYGON); glVertex2f(0,16); glVertex2f(0,0); glVertex2f(4,0); glVertex2f(4,16); glEnd(); glBegin(GL_POLYGON); glVertex2f(6,16); glVertex2f(6,6); glVertex2f(10,6); glVertex2f(10,16); glEnd(); glBegin(GL_POLYGON); glVertex2f(4,16); glVertex2f(4,12); glVertex2f(6,12); glVertex2f(6,16); glEnd(); glBegin(GL_POLYGON); glVertex2f(4,10); glVertex2f(4,6); glVertex2f(6,6); glVertex2f(6,10); glEnd(); }
static void glprint_q() { glBegin(GL_POLYGON); glVertex2f(0,16); glVertex2f(0,6); glVertex2f(4,6); glVertex2f(4,16); glEnd(); glBegin(GL_POLYGON); glVertex2f(6,16); glVertex2f(6,0); glVertex2f(10,0); glVertex2f(10,16); glEnd(); glBegin(GL_POLYGON); glVertex2f(4,10); glVertex2f(4,6); glVertex2f(6,6); glVertex2f(6,10); glEnd(); glBegin(GL_POLYGON); glVertex2f(4,16); glVertex2f(4,12); glVertex2f(6,12); glVertex2f(6,16); glEnd(); }
static void glprint_r() { glBegin(GL_POLYGON); glVertex2f(0,16); glVertex2f(0,0); glVertex2f(4,0); glVertex2f(4,16); glEnd(); glBegin(GL_POLYGON); glVertex2f(4,16); glVertex2f(4,12); glVertex2f(10,12); glVertex2f(10,16); glEnd(); glBegin(GL_POLYGON); glVertex2f(4,10); glVertex2f(4,6); glVertex2f(10,6); glVertex2f(10,10); glEnd(); glBegin(GL_POLYGON); glVertex2f(6,12); glVertex2f(6,10); glVertex2f(10,10); glVertex2f(10,12); glEnd(); glBegin(GL_POLYGON); glVertex2f(4,4); glVertex2f(6,0); glVertex2f(10,0); glVertex2f(7,6); glVertex2f(4,6); glEnd(); }
static void glprint_s() { glBegin(GL_POLYGON); glVertex2f(0,16); glVertex2f(0,12); glVertex2f(10,12); glVertex2f(10,16); glEnd(); glBegin(GL_POLYGON); glVertex2f(0,4); glVertex2f(0,0); glVertex2f(10,0); glVertex2f(10,4); glEnd(); glBegin(GL_POLYGON); glVertex2f(0,12); glVertex2f(0,6); glVertex2f(4,6); glVertex2f(4,12); glEnd(); glBegin(GL_POLYGON); glVertex2f(6,10); glVertex2f(6,4); glVertex2f(10,4); glVertex2f(10,10); glEnd(); glBegin(GL_POLYGON); glVertex2f(4,10); glVertex2f(4,6); glVertex2f(6,6); glVertex2f(6,10); glEnd(); }
static void glprint_t() { glBegin(GL_POLYGON); glVertex2f(0,16); glVertex2f(0,12); glVertex2f(10,12); glVertex2f(10,16); glEnd(); glBegin(GL_POLYGON); glVertex2f(3,12); glVertex2f(3,0); glVertex2f(7,0); glVertex2f(7,12); glEnd(); }
static void glprint_u() { glBegin(GL_POLYGON); glVertex2f(0,16); glVertex2f(0,0); glVertex2f(4,0); glVertex2f(4,16); glEnd(); glBegin(GL_POLYGON); glVertex2f(6,16); glVertex2f(6,0); glVertex2f(10,0); glVertex2f(10,16); glEnd(); glBegin(GL_POLYGON); glVertex2f(4,4); glVertex2f(4,0); glVertex2f(6,0); glVertex2f(6,4); glEnd(); }
static void glprint_v() { glBegin(GL_POLYGON); glVertex2f(0,16); glVertex2f(2,0); glVertex2f(4,10); glVertex2f(3,16); glEnd(); glBegin(GL_POLYGON); glVertex2f(4,10); glVertex2f(2,0); glVertex2f(10,16); glVertex2f(7,16); glEnd(); }
static void glprint_w() { glBegin(GL_POLYGON); glVertex2f(0,16); glVertex2f(0,0); glVertex2f(4,0); glVertex2f(4,16); glEnd(); glBegin(GL_POLYGON); glVertex2f(4,4); glVertex2f(4,0); glVertex2f(5,2); glVertex2f(5,6); glEnd(); glBegin(GL_POLYGON); glVertex2f(5,6); glVertex2f(5,2); glVertex2f(6,0); glVertex2f(6,4); glEnd(); glBegin(GL_POLYGON); glVertex2f(6,16); glVertex2f(6,0); glVertex2f(10,0); glVertex2f(10,16); glEnd(); }
static void glprint_x() { glBegin(GL_POLYGON); glVertex2f(3,8); glVertex2f(5,8); glVertex2f(5,11); glVertex2f(3,16); glVertex2f(0,16); glEnd(); glBegin(GL_POLYGON); glVertex2f(5,11); glVertex2f(5,8); glVertex2f(7,8); glVertex2f(10,16); glVertex2f(7,16); glEnd(); glBegin(GL_POLYGON); glVertex2f(3,8); glVertex2f(0,0); glVertex2f(3,0); glVertex2f(5,5); glVertex2f(5,8); glEnd(); glBegin(GL_POLYGON); glVertex2f(5,5); glVertex2f(7,0); glVertex2f(10,0); glVertex2f(7,8); glVertex2f(5,8); glEnd(); }
static void glprint_y() { glBegin(GL_POLYGON); glVertex2f(0,16); glVertex2f(0,10); glVertex2f(4,10); glVertex2f(4,16); glEnd(); glBegin(GL_POLYGON); glVertex2f(6,16); glVertex2f(6,10); glVertex2f(10,10); glVertex2f(10,16); glEnd(); glBegin(GL_POLYGON); glVertex2f(0,10); glVertex2f(0,8); glVertex2f(10,8); glVertex2f(10,10); glEnd(); glBegin(GL_POLYGON); glVertex2f(3,8); glVertex2f(3,0); glVertex2f(7,0); glVertex2f(7,8); glEnd(); }
static void glprint_z() { glBegin(GL_POLYGON); glVertex2f(0,16); glVertex2f(0,12); glVertex2f(10,12); glVertex2f(10,16); glEnd(); glBegin(GL_POLYGON); glVertex2f(0,4); glVertex2f(0,0); glVertex2f(10,0); glVertex2f(10,4); glEnd(); glBegin(GL_POLYGON); glVertex2f(6,12); glVertex2f(0,4); glVertex2f(4,4); glVertex2f(10,12); glEnd(); }



static void glprint_0() { glBegin(GL_POLYGON); glVertex2f(0,16); glVertex2f(0,0); glVertex2f(4,0); glVertex2f(4,16); glEnd(); glBegin(GL_POLYGON); glVertex2f(6,16); glVertex2f(6,0); glVertex2f(10,0); glVertex2f(10,16); glEnd(); glBegin(GL_POLYGON); glVertex2f(4,16); glVertex2f(4,12); glVertex2f(6,12); glVertex2f(6,16); glEnd(); glBegin(GL_POLYGON); glVertex2f(4,4); glVertex2f(4,0); glVertex2f(6,0); glVertex2f(6,4); glEnd(); }
static void glprint_1() { glBegin(GL_POLYGON); glVertex2f(0,4); glVertex2f(0,0); glVertex2f(10,0); glVertex2f(10,4); glEnd(); glBegin(GL_POLYGON); glVertex2f(3,16); glVertex2f(3,4); glVertex2f(7,4); glVertex2f(7,16); glEnd(); glBegin(GL_POLYGON); glVertex2f(0,16); glVertex2f(0,12); glVertex2f(3,12); glVertex2f(3,16); glEnd(); }
static void glprint_2() { glBegin(GL_POLYGON); glVertex2f(0,16); glVertex2f(0,12); glVertex2f(10,12); glVertex2f(10,16); glEnd(); glBegin(GL_POLYGON); glVertex2f(0,4); glVertex2f(0,0); glVertex2f(10,0); glVertex2f(10,4); glEnd(); glBegin(GL_POLYGON); glVertex2f(0,10); glVertex2f(0,4); glVertex2f(4,4); glVertex2f(4,10); glEnd(); glBegin(GL_POLYGON); glVertex2f(6,12); glVertex2f(6,6); glVertex2f(10,6); glVertex2f(10,12); glEnd(); glBegin(GL_POLYGON); glVertex2f(4,10); glVertex2f(4,6); glVertex2f(6,6); glVertex2f(6,10); glEnd(); }
static void glprint_3() { glBegin(GL_POLYGON); glVertex2f(0,16); glVertex2f(0,12); glVertex2f(10,12); glVertex2f(10,16); glEnd(); glBegin(GL_POLYGON); glVertex2f(0,4); glVertex2f(0,0); glVertex2f(10,0); glVertex2f(10,4); glEnd(); glBegin(GL_POLYGON); glVertex2f(6,12); glVertex2f(6,4); glVertex2f(10,4); glVertex2f(10,12); glEnd(); glBegin(GL_POLYGON); glVertex2f(2,10); glVertex2f(2,6); glVertex2f(6,6); glVertex2f(6,10); glEnd(); }
static void glprint_4() { glBegin(GL_POLYGON); glVertex2f(6,16); glVertex2f(6,0); glVertex2f(10,0); glVertex2f(10,16); glEnd(); glBegin(GL_POLYGON); glVertex2f(0,16); glVertex2f(0,6); glVertex2f(4,6); glVertex2f(4,16); glEnd(); glBegin(GL_POLYGON); glVertex2f(4,10); glVertex2f(4,6); glVertex2f(6,6); glVertex2f(6,10); glEnd(); }
static void glprint_5() { glBegin(GL_POLYGON); glVertex2f(0,16); glVertex2f(0,12); glVertex2f(10,12); glVertex2f(10,16); glEnd(); glBegin(GL_POLYGON); glVertex2f(0,4); glVertex2f(0,0); glVertex2f(10,0); glVertex2f(10,4); glEnd(); glBegin(GL_POLYGON); glVertex2f(0,12); glVertex2f(0,6); glVertex2f(4,6); glVertex2f(4,12); glEnd(); glBegin(GL_POLYGON); glVertex2f(6,10); glVertex2f(6,4); glVertex2f(10,4); glVertex2f(10,10); glEnd(); glBegin(GL_POLYGON); glVertex2f(4,10); glVertex2f(4,6); glVertex2f(6,6); glVertex2f(6,10); glEnd(); }
static void glprint_6() { glBegin(GL_POLYGON); glVertex2f(0,16); glVertex2f(0,0); glVertex2f(4,0); glVertex2f(4,16); glEnd(); glBegin(GL_POLYGON); glVertex2f(4,16); glVertex2f(4,12); glVertex2f(10,12); glVertex2f(10,16); glEnd(); glBegin(GL_POLYGON); glVertex2f(6,10); glVertex2f(6,0); glVertex2f(10,0); glVertex2f(10,10); glEnd(); glBegin(GL_POLYGON); glVertex2f(4,10); glVertex2f(4,6); glVertex2f(6,6); glVertex2f(6,10); glEnd(); glBegin(GL_POLYGON); glVertex2f(4,4); glVertex2f(4,0); glVertex2f(6,0); glVertex2f(6,4); glEnd(); }
static void glprint_7() { glBegin(GL_POLYGON); glVertex2f(0,16); glVertex2f(0,12); glVertex2f(10,12); glVertex2f(10,16); glEnd(); glBegin(GL_POLYGON); glVertex2f(0,0); glVertex2f(4,0); glVertex2f(10,12); glVertex2f(6,12); glEnd(); }
static void glprint_8() { glBegin(GL_POLYGON); glVertex2f(0,16); glVertex2f(0,0); glVertex2f(4,0); glVertex2f(4,16); glEnd(); glBegin(GL_POLYGON); glVertex2f(6,16); glVertex2f(6,0); glVertex2f(10,0); glVertex2f(10,16); glEnd(); glBegin(GL_POLYGON); glVertex2f(4,16); glVertex2f(4,12); glVertex2f(6,12); glVertex2f(6,16); glEnd(); glBegin(GL_POLYGON); glVertex2f(4,4); glVertex2f(4,0); glVertex2f(6,0); glVertex2f(6,4); glEnd(); glBegin(GL_POLYGON); glVertex2f(4,10); glVertex2f(4,6); glVertex2f(6,6); glVertex2f(6,10); glEnd(); }
static void glprint_9() { glBegin(GL_POLYGON); glVertex2f(0,16); glVertex2f(0,12); glVertex2f(10,12); glVertex2f(10,16); glEnd(); glBegin(GL_POLYGON); glVertex2f(0,12); glVertex2f(0,6); glVertex2f(4,6); glVertex2f(4,12); glEnd(); glBegin(GL_POLYGON); glVertex2f(4,10); glVertex2f(4,6); glVertex2f(6,6); glVertex2f(6,10); glEnd(); glBegin(GL_POLYGON); glVertex2f(0,4); glVertex2f(0,0); glVertex2f(10,0); glVertex2f(10,4); glEnd(); glBegin(GL_POLYGON); glVertex2f(6,12); glVertex2f(6,4); glVertex2f(10,4); glVertex2f(10,12); glEnd(); }

static void (*printfuncs_alpha[26])() =
{
  glprint_a,
  glprint_b,
  glprint_c,
  glprint_d,
  glprint_e,
  glprint_f,
  glprint_g,
  glprint_h,
  glprint_i,
  glprint_j,
  glprint_k,
  glprint_l,
  glprint_m,
  glprint_n,
  glprint_o,
  glprint_p,
  glprint_q,
  glprint_r,
  glprint_s,
  glprint_t,
  glprint_u,
  glprint_v,
  glprint_w,
  glprint_x,
  glprint_y,
  glprint_z
};

static void (*printfuncs_numeric[10])() =
{
  glprint_0,
  glprint_1,
  glprint_2,
  glprint_3,
  glprint_4,
  glprint_5,
  glprint_6,
  glprint_7,
  glprint_8,
  glprint_9,
};

void vt_init(float screenw=30, float screenh=8)
{
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  glOrtho(0, screenw*12, 0, screenh*16, -1, 1);
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();

  glEnable(GL_BLEND);
  glDisable(GL_LIGHTING);
  glDisable(GL_TEXTURE_2D);
  glDisable(GL_DEPTH_TEST);
}


void vt_exit(void)
{
  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_LIGHTING);
  glDisable(GL_BLEND);
}


void vt_set_colour(float r, float g, float b, float a)
{
  glColor4f(r,g,b,a);
}


void vt_set_pos(float x, float y)
{
  glLoadIdentity();
  glTranslatef(x*12, y*16, 0);
}


void vt_print(const std::string &text, bool centered)
{
  int sz = text.size();
  float offsetx=0;
  float offsety=0;
  if (centered)
  {
    offsetx = -sz * 5.0;
    offsety = -8.0;
  }
  glTranslatef(offsetx, offsety, 0);
  for (int i=0; i<sz; i++)
  {
    char c = text[i];
    if (c>='A' && c<='Z')
      printfuncs_alpha[c-'A']();
    if (c>='0' && c<='9')
      printfuncs_numeric[c-'0']();
    glTranslatef(12,0,0);
  }
}


