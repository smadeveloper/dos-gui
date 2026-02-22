#ifndef GUI_TEXTAREA_H
#define GUI_TEXTAREA_H

#include "gui_types.h"

#define TEXTAREA_MAX_LINES 512
#define TEXTAREA_MAX_LINE_LEN 256

typedef struct {
    char lines[TEXTAREA_MAX_LINES][TEXTAREA_MAX_LINE_LEN];
    int line_count;
    int cursor_line;
    int cursor_col;
    int scroll_y;
    int scroll_x;
    int selection_start_line, selection_start_col;
    int selection_end_line, selection_end_col;
    int selecting;
    int cursor_visible;
    int cursor_timer;
    int readonly;
    int word_wrap;
    int line_height;
    int tab_size;
    int show_line_numbers;
    int gutter_width;
} TextAreaData;

Widget *textarea_create(int x, int y, int w, int h);
void textarea_set_text(Widget *w, const char *text);
char *textarea_get_text(Widget *w, char *buf, int buf_size);
void textarea_set_readonly(Widget *w, int readonly);
void textarea_set_line_numbers(Widget *w, int show);
int textarea_get_line_count(Widget *w);

#endif /* GUI_TEXTAREA_H */
