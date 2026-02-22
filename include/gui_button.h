#ifndef GUI_BUTTON_H
#define GUI_BUTTON_H

#include "gui_types.h"

Widget *button_create(const char *text, int x, int y, int w, int h);
Widget *image_button_create(BITMAP *icon, int x, int y, int w, int h);

BITMAP *create_close_icon(void);
BITMAP *create_maximize_icon(void);
BITMAP *create_restore_icon(void);

#endif /* GUI_BUTTON_H */
