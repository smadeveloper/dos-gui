#ifndef GUI_SLIDER_H
#define GUI_SLIDER_H

#include "gui_types.h"

Widget *slider_create(int horizontal, int x, int y, int length, int min_val, int max_val);
void slider_set_value(Widget *w, int value);
int slider_get_value(Widget *w);
void slider_set_step(Widget *w, int step);
void slider_set_ticks(Widget *w, int interval);
void slider_set_show_value(Widget *w, int show);
void slider_set_colors(Widget *w, int track, int fill, int thumb);

#endif /* GUI_SLIDER_H */
