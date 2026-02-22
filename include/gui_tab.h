#ifndef GUI_TAB_H
#define GUI_TAB_H

#include "gui_types.h"

Widget *tab_create(int x, int y, int w, int h);
Widget *tab_add_page(Widget *tab, const char *title);
Widget *tab_add(Widget *tab, const char *title);

#endif /* GUI_TAB_H */
