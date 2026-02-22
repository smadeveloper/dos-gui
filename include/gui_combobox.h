#ifndef GUI_COMBOBOX_H
#define GUI_COMBOBOX_H

#include "gui_types.h"

Widget *combobox_create(int x, int y, int w);
void combobox_add_item(Widget *w, const char *text);
void combobox_draw_dropdown(Widget *self, BITMAP *target);
int combobox_hit_dropdown(Widget *self, int mx, int my);
int combobox_dropdown_item_at(Widget *self, int mx, int my);
Widget *find_open_combobox_recursive(Widget *w);

#endif /* GUI_COMBOBOX_H */
