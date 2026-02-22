#ifndef GUI_MENUBAR_H
#define GUI_MENUBAR_H

#include "gui_types.h"

Widget *menubar_create(void);
Widget *menubar_add_menu(Widget *menubar, const char *title);
int menubar_item_at(Widget *menubar, int mx, int my);
void menubar_draw_menus(Widget *self, BITMAP *target);

#endif /* GUI_MENUBAR_H */
