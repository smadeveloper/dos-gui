#ifndef GUI_MENU_H
#define GUI_MENU_H

#include "gui_types.h"

Widget *menu_create(void);
Widget *menu_add_item(Widget *menu, const char *text, const char *shortcut, EventCallback on_click);
Widget *menu_add_separator(Widget *menu);
Widget *menu_add_submenu(Widget *menu, const char *text, Widget *submenu);

void menu_calc_geometry(Widget *self);
void menu_draw_popup(Widget *self, BITMAP *target);
void menu_close_all(Widget *menu);
int menu_hit_test_deep(Widget *menu, int mx, int my, Widget **out_menu, int *out_index);
int menu_is_point_in_any(Widget *menu, int mx, int my);
void menu_update_hover_recursive(Widget *menu, int mx, int my);

#endif /* GUI_MENU_H */
