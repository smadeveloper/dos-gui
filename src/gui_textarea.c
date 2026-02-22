#include "gui_textarea.h"
#include "gui_widget.h"
#include "gui_utils.h"
#include "gui_state.h"
#include <stdlib.h>
#include <string.h>

static void textarea_ensure_line(Widget *self) {
    TextAreaData *data = (TextAreaData *)self->extra;
    if (data->line_count == 0) {
        data->lines[0][0] = 0;
        data->line_count = 1;
    }
}

static void textarea_draw(Widget *self, BITMAP *target) {
    TextAreaData *data = (TextAreaData *)self->extra;
    textarea_ensure_line(self);

    int border_col = self->focused ? theme.input_focus_border : theme.input_border;
    int lh = data->line_height;
    int gutter = data->show_line_numbers ? data->gutter_width : 0;

    /* Background */
    rectfill(target, self->abs_x, self->abs_y,
             self->abs_x + self->w - 1, self->abs_y + self->h - 1, theme.input_bg);
    rect(target, self->abs_x, self->abs_y,
         self->abs_x + self->w - 1, self->abs_y + self->h - 1, border_col);

    /* Gutter */
    if (gutter > 0) {
        rectfill(target, self->abs_x + 1, self->abs_y + 1,
                 self->abs_x + gutter, self->abs_y + self->h - 2,
                 makecol(240, 240, 240));
        vline(target, self->abs_x + gutter, self->abs_y + 1,
              self->abs_y + self->h - 2, theme.border);
    }

    int text_x = self->abs_x + gutter + 4;
    int text_area_w = self->w - gutter - 6;
    int visible_lines = (self->h - 2) / lh;

    set_clip_rect(target, self->abs_x + 1, self->abs_y + 1,
                  self->abs_x + self->w - 2, self->abs_y + self->h - 2);

    for (int i = 0; i < visible_lines && (i + data->scroll_y) < data->line_count; i++) {
        int line_idx = i + data->scroll_y;
        int ly = self->abs_y + 2 + i * lh;

        /* Line numbers */
        if (data->show_line_numbers && gutter > 0) {
            char num_buf[8];
            sprintf(num_buf, "%3d", line_idx + 1);
            textout_right_ex(target, font, num_buf,
                             self->abs_x + gutter - 4, ly, theme.disabled_fg, -1);
        }

        /* Highlight current line */
        if (self->focused && line_idx == data->cursor_line) {
            rectfill(target, self->abs_x + gutter + 1, ly,
                     self->abs_x + self->w - 2, ly + lh - 1,
                     makecol(245, 245, 255));
        }

        /* Text */
        textout_ex(target, font, data->lines[line_idx],
                   text_x - data->scroll_x, ly,
                   self->enabled ? theme.input_fg : theme.disabled_fg, -1);
    }

    /* Cursor */
    if (self->focused && data->cursor_visible && !data->readonly) {
        int cursor_screen_line = data->cursor_line - data->scroll_y;
        if (cursor_screen_line >= 0 && cursor_screen_line < visible_lines) {
            char temp[TEXTAREA_MAX_LINE_LEN];
            int cur_col = data->cursor_col;
            int line_len = strlen(data->lines[data->cursor_line]);
            if (cur_col > line_len) cur_col = line_len;
            strncpy(temp, data->lines[data->cursor_line], cur_col);
            temp[cur_col] = 0;
            int cx = text_x + text_length(font, temp) - data->scroll_x;
            int cy = self->abs_y + 2 + cursor_screen_line * lh;
            vline(target, cx, cy, cy + lh - 1, theme.input_fg);
        }
    }

    set_clip_rect(target, 0, 0, target->w - 1, target->h - 1);
}

static void textarea_update(Widget *self) {
    TextAreaData *data = (TextAreaData *)self->extra;
    if (self->focused) {
        data->cursor_timer++;
        if (data->cursor_timer > 30) {
            data->cursor_visible = !data->cursor_visible;
            data->cursor_timer = 0;
            self->dirty = 1;
        }
    }
}

static void textarea_ensure_cursor_visible(Widget *self) {
    TextAreaData *data = (TextAreaData *)self->extra;
    int lh = data->line_height;
    int visible_lines = (self->h - 2) / lh;

    if (data->cursor_line < data->scroll_y) {
        data->scroll_y = data->cursor_line;
    } else if (data->cursor_line >= data->scroll_y + visible_lines) {
        data->scroll_y = data->cursor_line - visible_lines + 1;
    }
    if (data->scroll_y < 0) data->scroll_y = 0;
}

static void textarea_key_handler(Widget *self, Event *event) {
    TextAreaData *data = (TextAreaData *)self->extra;
    textarea_ensure_line(self);

    int k = event->key;
    int ch = event->key_char;
    int line_len = strlen(data->lines[data->cursor_line]);

    if (k == KEY_LEFT) {
        if (data->cursor_col > 0) {
            data->cursor_col--;
        } else if (data->cursor_line > 0) {
            data->cursor_line--;
            data->cursor_col = strlen(data->lines[data->cursor_line]);
        }
        event->handled = 1;
    } else if (k == KEY_RIGHT) {
        if (data->cursor_col < line_len) {
            data->cursor_col++;
        } else if (data->cursor_line < data->line_count - 1) {
            data->cursor_line++;
            data->cursor_col = 0;
        }
        event->handled = 1;
    } else if (k == KEY_UP) {
        if (data->cursor_line > 0) {
            data->cursor_line--;
            int prev_len = strlen(data->lines[data->cursor_line]);
            if (data->cursor_col > prev_len) data->cursor_col = prev_len;
        }
        event->handled = 1;
    } else if (k == KEY_DOWN) {
        if (data->cursor_line < data->line_count - 1) {
            data->cursor_line++;
            int next_len = strlen(data->lines[data->cursor_line]);
            if (data->cursor_col > next_len) data->cursor_col = next_len;
        }
        event->handled = 1;
    } else if (k == KEY_HOME) {
        data->cursor_col = 0;
        event->handled = 1;
    } else if (k == KEY_END) {
        data->cursor_col = line_len;
        event->handled = 1;
    } else if (k == KEY_PGUP) {
        int vis = (self->h - 2) / data->line_height;
        data->cursor_line = clamp_int(data->cursor_line - vis, 0, data->line_count - 1);
        int cur_len = strlen(data->lines[data->cursor_line]);
        if (data->cursor_col > cur_len) data->cursor_col = cur_len;
        event->handled = 1;
    } else if (k == KEY_PGDN) {
        int vis = (self->h - 2) / data->line_height;
        data->cursor_line = clamp_int(data->cursor_line + vis, 0, data->line_count - 1);
        int cur_len = strlen(data->lines[data->cursor_line]);
        if (data->cursor_col > cur_len) data->cursor_col = cur_len;
        event->handled = 1;
    } else if (k == KEY_ENTER || k == KEY_ENTER_PAD) {
        if (!data->readonly && data->line_count < TEXTAREA_MAX_LINES) {
            /* Split line at cursor */
            char *cur_line = data->lines[data->cursor_line];
            char remainder[TEXTAREA_MAX_LINE_LEN];
            strncpy(remainder, &cur_line[data->cursor_col], TEXTAREA_MAX_LINE_LEN - 1);
            remainder[TEXTAREA_MAX_LINE_LEN - 1] = 0;
            cur_line[data->cursor_col] = 0;

            /* Shift lines down */
            for (int i = data->line_count; i > data->cursor_line + 1; i--) {
                strncpy(data->lines[i], data->lines[i - 1], TEXTAREA_MAX_LINE_LEN - 1);
            }
            data->line_count++;
            strncpy(data->lines[data->cursor_line + 1], remainder, TEXTAREA_MAX_LINE_LEN - 1);
            data->cursor_line++;
            data->cursor_col = 0;
            widget_emit_simple(self, EVENT_CHANGE);
        }
        event->handled = 1;
    } else if (k == KEY_BACKSPACE) {
        if (!data->readonly) {
            if (data->cursor_col > 0) {
                char *line = data->lines[data->cursor_line];
                memmove(&line[data->cursor_col - 1], &line[data->cursor_col], line_len - data->cursor_col + 1);
                data->cursor_col--;
                widget_emit_simple(self, EVENT_CHANGE);
            } else if (data->cursor_line > 0) {
                /* Merge with previous line */
                int prev_len = strlen(data->lines[data->cursor_line - 1]);
                if (prev_len + line_len < TEXTAREA_MAX_LINE_LEN - 1) {
                    strcat(data->lines[data->cursor_line - 1], data->lines[data->cursor_line]);
                    /* Shift lines up */
                    for (int i = data->cursor_line; i < data->line_count - 1; i++) {
                        strncpy(data->lines[i], data->lines[i + 1], TEXTAREA_MAX_LINE_LEN - 1);
                    }
                    data->line_count--;
                    data->cursor_line--;
                    data->cursor_col = prev_len;
                    widget_emit_simple(self, EVENT_CHANGE);
                }
            }
        }
        event->handled = 1;
    } else if (k == KEY_DEL) {
        if (!data->readonly) {
            if (data->cursor_col < line_len) {
                char *line = data->lines[data->cursor_line];
                memmove(&line[data->cursor_col], &line[data->cursor_col + 1], line_len - data->cursor_col);
                widget_emit_simple(self, EVENT_CHANGE);
            } else if (data->cursor_line < data->line_count - 1) {
                int next_len = strlen(data->lines[data->cursor_line + 1]);
                if (line_len + next_len < TEXTAREA_MAX_LINE_LEN - 1) {
                    strcat(data->lines[data->cursor_line], data->lines[data->cursor_line + 1]);
                    for (int i = data->cursor_line + 1; i < data->line_count - 1; i++) {
                        strncpy(data->lines[i], data->lines[i + 1], TEXTAREA_MAX_LINE_LEN - 1);
                    }
                    data->line_count--;
                    widget_emit_simple(self, EVENT_CHANGE);
                }
            }
        }
        event->handled = 1;
    } else if (k == KEY_TAB && !data->readonly) {
        /* Insert spaces for tab */
        char *line = data->lines[data->cursor_line];
        int spaces = data->tab_size;
        if (line_len + spaces < TEXTAREA_MAX_LINE_LEN - 1) {
            memmove(&line[data->cursor_col + spaces], &line[data->cursor_col], line_len - data->cursor_col + 1);
            for (int i = 0; i < spaces; i++) line[data->cursor_col + i] = ' ';
            data->cursor_col += spaces;
            widget_emit_simple(self, EVENT_CHANGE);
        }
        event->handled = 1;
    } else if (ch >= 32 && ch < 127 && !data->readonly) {
        char *line = data->lines[data->cursor_line];
        if (line_len < TEXTAREA_MAX_LINE_LEN - 2) {
            memmove(&line[data->cursor_col + 1], &line[data->cursor_col], line_len - data->cursor_col + 1);
            line[data->cursor_col] = (char)ch;
            data->cursor_col++;
            widget_emit_simple(self, EVENT_CHANGE);
        }
        event->handled = 1;
    }

    textarea_ensure_cursor_visible(self);
    data->cursor_visible = 1;
    data->cursor_timer = 0;
    self->dirty = 1;
}

static void textarea_mouse_down(Widget *self, Event *event) {
    TextAreaData *data = (TextAreaData *)self->extra;
    textarea_ensure_line(self);

    int lh = data->line_height;
    int gutter = data->show_line_numbers ? data->gutter_width : 0;
    int text_x = self->abs_x + gutter + 4;
    int rel_y = event->mouse_y - (self->abs_y + 2);
    int rel_x = event->mouse_x - text_x + data->scroll_x;

    int line = rel_y / lh + data->scroll_y;
    if (line < 0) line = 0;
    if (line >= data->line_count) line = data->line_count - 1;
    data->cursor_line = line;

    /* Find column */
    int len = strlen(data->lines[line]);
    int col = 0;
    for (col = 0; col <= len; col++) {
        char temp[TEXTAREA_MAX_LINE_LEN];
        strncpy(temp, data->lines[line], col);
        temp[col] = 0;
        if (text_length(font, temp) >= rel_x) break;
    }
    data->cursor_col = clamp_int(col, 0, len);
    data->cursor_visible = 1;
    data->cursor_timer = 0;
    self->dirty = 1;
}

Widget *textarea_create(int x, int y, int w, int h) {
    Widget *wid = widget_create(WIDGET_BASE, x, y, w, h);
    TextAreaData *data = (TextAreaData *)calloc(1, sizeof(TextAreaData));
    data->line_count = 1;
    data->lines[0][0] = 0;
    data->cursor_visible = 1;
    data->line_height = text_height(font) + 2;
    data->tab_size = 4;
    data->gutter_width = 36;
    wid->extra = data;
    wid->draw = textarea_draw;
    wid->update = textarea_update;
    widget_on(wid, EVENT_KEY_DOWN, textarea_key_handler);
    widget_on(wid, EVENT_MOUSE_DOWN, textarea_mouse_down);
    return wid;
}

void textarea_set_text(Widget *w, const char *text) {
    TextAreaData *data = (TextAreaData *)w->extra;
    data->line_count = 0;
    data->cursor_line = 0;
    data->cursor_col = 0;
    data->scroll_y = 0;

    if (!text || !text[0]) {
        data->lines[0][0] = 0;
        data->line_count = 1;
        w->dirty = 1;
        return;
    }

    const char *p = text;
    while (*p && data->line_count < TEXTAREA_MAX_LINES) {
        const char *eol = strchr(p, '\n');
        int len;
        if (eol) {
            len = eol - p;
            if (len >= TEXTAREA_MAX_LINE_LEN) len = TEXTAREA_MAX_LINE_LEN - 1;
            strncpy(data->lines[data->line_count], p, len);
            data->lines[data->line_count][len] = 0;
            data->line_count++;
            p = eol + 1;
        } else {
            len = strlen(p);
            if (len >= TEXTAREA_MAX_LINE_LEN) len = TEXTAREA_MAX_LINE_LEN - 1;
            strncpy(data->lines[data->line_count], p, len);
            data->lines[data->line_count][len] = 0;
            data->line_count++;
            break;
        }
    }
    if (data->line_count == 0) {
        data->lines[0][0] = 0;
        data->line_count = 1;
    }
    w->dirty = 1;
}

char *textarea_get_text(Widget *w, char *buf, int buf_size) {
    TextAreaData *data = (TextAreaData *)w->extra;
    buf[0] = 0;
    int pos = 0;
    for (int i = 0; i < data->line_count && pos < buf_size - 2; i++) {
        int len = strlen(data->lines[i]);
        if (pos + len + 1 >= buf_size) break;
        strcpy(&buf[pos], data->lines[i]);
        pos += len;
        if (i < data->line_count - 1) {
            buf[pos++] = '\n';
        }
    }
    buf[pos] = 0;
    return buf;
}

void textarea_set_readonly(Widget *w, int readonly) {
    ((TextAreaData *)w->extra)->readonly = readonly;
}

void textarea_set_line_numbers(Widget *w, int show) {
    ((TextAreaData *)w->extra)->show_line_numbers = show;
    w->dirty = 1;
}

int textarea_get_line_count(Widget *w) {
    return ((TextAreaData *)w->extra)->line_count;
}
