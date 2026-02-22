#ifndef GUI_LISTVIEW_H
#define GUI_LISTVIEW_H

#include "gui_types.h"

#define MAX_LISTVIEW_ITEMS 256
#define LISTVIEW_ITEM_HEIGHT 20

typedef struct {
    char items[MAX_LISTVIEW_ITEMS][MAX_TEXT_LEN];
    int item_count;
    int selected_index;
    int scroll_offset;
    int hover_index;
    int multi_select;
    int selected_flags[MAX_LISTVIEW_ITEMS];
    int dragging_scroll;
    int drag_offset;
    int show_headers;
    char header_text[MAX_TEXT_LEN];
} ListViewData;

Widget *listview_create(int x, int y, int w, int h);
void listview_add_item(Widget *w, const char *text);
void listview_remove_item(Widget *w, int index);
void listview_clear(Widget *w);
int listview_get_selected(Widget *w);
const char *listview_get_item_text(Widget *w, int index);
void listview_set_header(Widget *w, const char *text);
int listview_get_item_count(Widget *w);

#endif /* GUI_LISTVIEW_H */
