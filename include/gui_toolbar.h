#ifndef GUI_TOOLBAR_H
#define GUI_TOOLBAR_H

#include "gui_types.h"

/* ToolbarItemData - for toggle buttons in toolbar */
typedef struct {
    BITMAP *icon;
    char tooltip[64];
    int toggled;
    int is_separator;
} ToolbarItemData;

Widget *toolbar_create(void);
Widget *toolbar_add_button(Widget *tb, BITMAP *icon, const char *tooltip_text, EventCallback on_click);
Widget *toolbar_add_toggle(Widget *tb, BITMAP *icon, const char *tooltip_text, EventCallback on_click);
void toolbar_add_separator(Widget *tb);

#endif /* GUI_TOOLBAR_H */
