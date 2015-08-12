#ifndef VECTORTEXT_H
#define VECTORTEXT_H

#include <string>

extern void vt_init(float screenw, float screenh);

extern void vt_exit(void);

extern void vt_set_pos(float x, float y);

extern void vt_set_colour(float r, float g, float b, float a=1.0);

extern void vt_print(const std::string &text, bool centered);


#endif

