#ifndef GUI_STATUSBAR_H
#define GUI_STATUSBAR_H

#include "gui_types.h"

Widget *statusbar_create(void);
void statusbar_set_text(Widget *w, const char *text);
void statusbar_add_section(Widget *w, int width);
void statusbar_set_section(Widget *w, int index, const char *text);

#endif /* GUI_STATUSBAR_H */
