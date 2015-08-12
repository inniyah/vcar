// splitppm.cxx

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) 
{ 
  if (argc!= 4) 
  { 
    fprintf(stderr,"Usage: %s file w h\n", argv[0]); exit(1); 
  }
  char *fname = argv[1]; 
  int w = atoi(argv[2]); 
  int h = atoi(argv[3]); 
  assert(w>0); assert(h>0); 
  FILE *f = fopen(fname,"rb"); 
  if (!f) 
  { 
    fprintf(stderr,"Cannot open '%s' for reading\n", fname); 
    exit(2); 
  }
  int framenr=0; int chunk = w*h*3; 
  unsigned char *buf = new unsigned char [chunk]; 
  int retval; 
  do { 
    retval = fread(buf, chunk, 1, f); 
    if (retval) 
    { 
      char outname[128]; 
      sprintf(outname, "nb%04d.ppm", framenr); 
      FILE *g=fopen(outname,"wb"); 
      fprintf(g,"P6 %d %d 255\n", w, h); 
      fwrite(buf, chunk, 1, g); 
      fclose(g); 
      framenr++; 
    } 
  }
  while (retval); 
  fprintf(stderr,"Wrote %d frames\n", framenr); 
  return 0; 
}

