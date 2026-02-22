#ifndef GUI_UTILS_H
#define GUI_UTILS_H

#include "gui_types.h"

void init_theme(void);
void theme_init_default(void);

int clamp_int(int val, int lo, int hi);
int point_in_rect(int px, int py, int rx, int ry, int rw, int rh);

/* Draw helpers */
void draw_raised_rect(BITMAP *bmp, int x, int y, int w, int h);
void draw_sunken_rect(BITMAP *bmp, int x, int y, int w, int h);
void draw_3d_rect(BITMAP *bmp, int x, int y, int w, int h, int pressed);

#endif /* GUI_UTILS_H */
