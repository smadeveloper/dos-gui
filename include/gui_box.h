#ifndef GUI_BOX_H
#define GUI_BOX_H

#include "gui_types.h"

void box_calc_content_size(Widget *self, int *out_w, int *out_h);
void box_do_layout(Widget *self);
void box_draw(Widget *self, BITMAP *target);
void box_draw_bg(Widget *self, BITMAP *target);

Widget *hbox_create(int x, int y, int w, int h);
Widget *vbox_create(int x, int y, int w, int h);
Widget *hbox_create_simple(int spacing);
Widget *vbox_create_simple(int spacing);

#endif /* GUI_BOX_H */
