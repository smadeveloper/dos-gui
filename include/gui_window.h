#ifndef GUI_WINDOW_H
#define GUI_WINDOW_H

#include "gui_types.h"

Widget *window_create(const char *title, int x, int y, int w, int h);
Widget *window_get_client(Widget *win);
void window_set_menubar(Widget *win, Widget *menubar);
void window_set_statusbar(Widget *win, Widget *statusbar);
void window_bring_to_front(Widget *win);
Widget *window_at_point(int mx, int my);
void set_active_window(Widget *win);

BITMAP *create_close_icon(void);
BITMAP *create_maximize_icon(void);
BITMAP *create_restore_icon(void);

#endif /* GUI_WINDOW_H */
