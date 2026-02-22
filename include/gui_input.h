#ifndef GUI_INPUT_H
#define GUI_INPUT_H

#include "gui_types.h"

Widget *input_create(int x, int y, int w);
void input_set_placeholder(Widget *w, const char *text);
void input_set_text(Widget *w, const char *text);
const char *input_get_text(Widget *w);
void input_get_text_buf(Widget *w, char *buf, int buf_size);
void input_set_password_mode(Widget *w, int mode);

#endif /* GUI_INPUT_H */
