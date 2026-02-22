#ifndef GUI_GROUPBOX_H
#define GUI_GROUPBOX_H

#include "gui_types.h"

#define GROUPBOX_HEADER 16

typedef struct {
    char title[MAX_TEXT_LEN];
    int title_color;
    int border_color;
} GroupBoxData;

Widget *groupbox_create(const char *title, int x, int y, int w, int h);
void groupbox_set_title(Widget *w, const char *title);

#endif /* GUI_GROUPBOX_H */
