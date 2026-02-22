#ifndef GUI_SPINBOX_H
#define GUI_SPINBOX_H

#include "gui_types.h"

Widget *spinbox_create(int x, int y, int w, int min_val, int max_val);
Widget *spinbox_create_float(int x, int y, int w, float min_val, float max_val, float step, int decimal_places);
void spinbox_set_value(Widget *w, int value);
int spinbox_get_value(Widget *w);
void spinbox_set_float_value(Widget *w, float value);
float spinbox_get_float_value(Widget *w);
void spinbox_set_step(Widget *w, int step);

#endif /* GUI_SPINBOX_H */
