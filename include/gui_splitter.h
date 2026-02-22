#ifndef GUI_SPLITTER_H
#define GUI_SPLITTER_H

#include "gui_types.h"

#define SPLITTER_SIZE 6
#define SPLITTER_MIN_PANE 30

typedef struct {
    int horizontal; /* 1=horizontal split (left/right), 0=vertical (top/bottom) */
    int split_pos;
    int dragging;
    int drag_offset;
    int min_first, min_second;
    Widget *first_pane;
    Widget *second_pane;
} SplitterData;

Widget *splitter_create(int horizontal, int x, int y, int w, int h);
Widget *splitter_get_first(Widget *w);
Widget *splitter_get_second(Widget *w);
void splitter_set_position(Widget *w, int pos);
int splitter_get_position(Widget *w);
void splitter_set_min_sizes(Widget *w, int min_first, int min_second);

#endif /* GUI_SPLITTER_H */
