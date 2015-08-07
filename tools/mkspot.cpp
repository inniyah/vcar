/*
 * mkspot.cxx
 * (c)2007 by Bram Stolk
 */

#include <string.h>
#include <stdio.h>

#define SZ 512

typedef struct
{
  unsigned char r,g,b;
} rgb_t;


static rgb_t im[SZ][SZ];

int main(int argc, char *argv[])
{
  memset(im, sizeof(im), 0);
  for (int y=0; y<SZ; y++)
  {
    for (int x=0; x<SZ; x++)
    {
      int o = 4;
      int m = (SZ*48/100) * (SZ*48/100);
      int c = (SZ*40/100) * (SZ*40/100);
      m = m - c;

      int cx_r = SZ/2+0; int cy_r = SZ/2-o-o;
      int cx_g = SZ/2-o; int cy_g = SZ/2+o;
      int cx_b = SZ/2+o; int cy_b = SZ/2+o;

      int d2_r = (cx_r-x)*(cx_r-x)+(cy_r-y)*(cy_r-y);
      d2_r = (d2_r - c);
      int v_r = m - d2_r;
      if (v_r<0)  v_r = 0;
      if (v_r>=m) v_r = m-1;
      im[y][x].r = (unsigned char) (v_r * 256.0 / m);

      int d2_g = (cx_g-x)*(cx_g-x)+(cy_g-y)*(cy_g-y);
      d2_g = (d2_g - c);
      int v_g = m - d2_g;
      if (v_g<0)  v_g = 0;
      if (v_g>=m) v_g = m-1;
      im[y][x].g = (unsigned char) (v_g * 256.0 / m);

      int d2_b = (cx_b-x)*(cx_b-x)+(cy_b-y)*(cy_b-y);
      d2_b = (d2_b - c);
      int v_b = m - d2_b;
      if (v_b<0)  v_b = 0;
      if (v_b>=m) v_b = m-1;
      im[y][x].b = (unsigned char) (v_b * 256.0 / m);
    }
  }
  fprintf(stdout,"P6 %d %d %d\n", SZ, SZ, 255);
  fwrite(im, sizeof(im), 1, stdout);
}

