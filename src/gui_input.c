#include "gui_input.h"
#include "gui_widget.h"
#include "gui_utils.h"
#include <stdlib.h>
#include <string.h>

static void input_draw(Widget *self, BITMAP *target) {
    InputData *data = (InputData *)self->extra;
    int border_col = self->focused ? theme.input_focus_border : theme.input_border;

    rectfill(target, self->abs_x, self->abs_y,
             self->abs_x + self->w - 1, self->abs_y + self->h - 1, theme.input_bg);
    rect(target, self->abs_x, self->abs_y,
         self->abs_x + self->w - 1, self->abs_y + self->h - 1, border_col);

    int tx = self->abs_x + 4;
    int ty = self->abs_y + (self->h - text_height(font)) / 2;

    set_clip_rect(target, self->abs_x + 2, self->abs_y + 1,
                  self->abs_x + self->w - 3, self->abs_y + self->h - 2);

    const char *display_text = data->text;
    int col = theme.input_fg;

    if (strlen(data->text) == 0 && strlen(data->placeholder) > 0 && !self->focused) {
        display_text = data->placeholder;
        col = theme.disabled_fg;
    }

    if (data->password_mode) {
        char masked[MAX_TEXT_LEN];
        int len = strlen(data->text);
        memset(masked, '*', len);
        masked[len] = 0;
        textout_ex(target, font, masked, tx - data->scroll_offset, ty, col, -1);
    } else {
        textout_ex(target, font, display_text, tx - data->scroll_offset, ty, col, -1);
    }

    if (self->focused && data->cursor_visible) {
        char temp[MAX_TEXT_LEN];
        strncpy(temp, data->text, data->cursor_pos);
        temp[data->cursor_pos] = 0;
        int cursor_x_offset = text_length(font, temp);

        if (data->password_mode) {
            char masked[MAX_TEXT_LEN];
            memset(masked, '*', data->cursor_pos);
            masked[data->cursor_pos] = 0;
            cursor_x_offset = text_length(font, masked);
        }

        int cx = tx + cursor_x_offset - data->scroll_offset;
        vline(target, cx, self->abs_y + 3, self->abs_y + self->h - 4, theme.input_fg);
    }

    set_clip_rect(target, 0, 0, target->w - 1, target->h - 1);
}

static void input_update(Widget *self) {
    InputData *data = (InputData *)self->extra;
    if (self->focused) {
        data->cursor_timer++;
        if (data->cursor_timer > 30) {
            data->cursor_visible = !data->cursor_visible;
            data->cursor_timer = 0;
            self->dirty = 1;
        }
    }
}

static void input_handle_key(Widget *self, Event *event) {
    InputData *data = (InputData *)self->extra;
    int k = event->key;
    int ch = event->key_char;
    int len = strlen(data->text);

    if (k == KEY_LEFT) { if (data->cursor_pos > 0) data->cursor_pos--; }
    else if (k == KEY_RIGHT) { if (data->cursor_pos < len) data->cursor_pos++; }
    else if (k == KEY_HOME) { data->cursor_pos = 0; }
    else if (k == KEY_END) { data->cursor_pos = len; }
    else if (k == KEY_BACKSPACE) {
        if (data->cursor_pos > 0 && !data->readonly) {
            memmove(&data->text[data->cursor_pos - 1], &data->text[data->cursor_pos], len - data->cursor_pos + 1);
            data->cursor_pos--;
            widget_emit_simple(self, EVENT_CHANGE);
        }
    } else if (k == KEY_DEL) {
        if (data->cursor_pos < len && !data->readonly) {
            memmove(&data->text[data->cursor_pos], &data->text[data->cursor_pos + 1], len - data->cursor_pos);
            widget_emit_simple(self, EVENT_CHANGE);
        }
    } else if (ch >= 32 && ch < 127 && !data->readonly) {
        if (len < data->max_length - 1) {
            memmove(&data->text[data->cursor_pos + 1], &data->text[data->cursor_pos], len - data->cursor_pos + 1);
            data->text[data->cursor_pos] = (char)ch;
            data->cursor_pos++;
            widget_emit_simple(self, EVENT_CHANGE);
        }
    }

    char temp[MAX_TEXT_LEN];
    strncpy(temp, data->text, data->cursor_pos);
    temp[data->cursor_pos] = 0;
    int cursor_x = text_length(font, temp);
    int visible_w = self->w - 8;
    if (cursor_x - data->scroll_offset > visible_w) data->scroll_offset = cursor_x - visible_w;
    if (cursor_x - data->scroll_offset < 0) data->scroll_offset = cursor_x;

    data->cursor_visible = 1;
    data->cursor_timer = 0;
    self->dirty = 1;
}

Widget *input_create(int x, int y, int w) {
    Widget *wid = widget_create(WIDGET_INPUT, x, y, w, INPUT_HEIGHT);
    InputData *data = (InputData *)calloc(1, sizeof(InputData));
    data->max_length = MAX_TEXT_LEN;
    data->cursor_visible = 1;
    wid->extra = data;
    wid->draw = input_draw;
    wid->update = input_update;
    widget_on(wid, EVENT_KEY_DOWN, input_handle_key);
    return wid;
}

void input_set_placeholder(Widget *w, const char *text) {
    InputData *data = (InputData *)w->extra;
    strncpy(data->placeholder, text, MAX_TEXT_LEN - 1);
}

void input_set_text(Widget *w, const char *text) {
    InputData *data = (InputData *)w->extra;
    strncpy(data->text, text, MAX_TEXT_LEN - 1);
    data->cursor_pos = strlen(data->text);
    w->dirty = 1;
}

const char *input_get_text(Widget *w) {
    InputData *data = (InputData *)w->extra;
    return data->text;
}

void input_get_text_buf(Widget *w, char *buf, int buf_size) {
    InputData *data = (InputData *)w->extra;
    strncpy(buf, data->text, buf_size - 1);
    buf[buf_size - 1] = 0;
}

void input_set_password_mode(Widget *w, int mode) {
    InputData *data = (InputData *)w->extra;
    data->password_mode = mode;
    w->dirty = 1;
}
