#ifndef GUI_PROGRESSBAR_H
#define GUI_PROGRESSBAR_H

#include "gui_types.h"

#define PROGRESSBAR_HEIGHT 22

typedef struct {
    int value;
    int min_value;
    int max_value;
    int horizontal;
    int show_text;
    int bar_color;
    int bar_bg_color;
    int text_color;
    int style;  /* 0=solid, 1=striped */
    int animated;
    int anim_offset;
} ProgressBarData;

Widget *progressbar_create(int horizontal, int x, int y, int length);
void progressbar_set_value(Widget *w, int value);
int progressbar_get_value(Widget *w);
void progressbar_set_range(Widget *w, int min_val, int max_val);
void progressbar_set_colors(Widget *w, int bar_col, int bg_col, int text_col);
void progressbar_set_style(Widget *w, int style);
void progressbar_set_show_text(Widget *w, int show);

#endif /* GUI_PROGRESSBAR_H */
