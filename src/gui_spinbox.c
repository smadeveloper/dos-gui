#include "gui_spinbox.h"
#include "gui_widget.h"
#include "gui_utils.h"
#include "gui_state.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>

static void spinbox_format_value(Widget *self, char *buf, int buf_size) {
    SpinboxData *data = (SpinboxData *)self->extra;
    if (data->use_float) snprintf(buf, buf_size, "%.*f", data->decimal_places, data->float_value);
    else snprintf(buf, buf_size, "%d", data->value);
}

static void spinbox_clamp(Widget *self) {
    SpinboxData *data = (SpinboxData *)self->extra;
    if (data->use_float) {
        if (data->float_value < data->float_min) data->float_value = data->float_min;
        if (data->float_value > data->float_max) data->float_value = data->float_max;
    } else {
        if (data->value < data->min_value) data->value = data->min_value;
        if (data->value > data->max_value) data->value = data->max_value;
    }
}

static void spinbox_increment(Widget *self) {
    SpinboxData *data = (SpinboxData *)self->extra;
    if (data->use_float) data->float_value += data->float_step;
    else data->value += data->step;
    spinbox_clamp(self); self->dirty = 1; widget_emit_simple(self, EVENT_CHANGE);
}

static void spinbox_decrement(Widget *self) {
    SpinboxData *data = (SpinboxData *)self->extra;
    if (data->use_float) data->float_value -= data->float_step;
    else data->value -= data->step;
    spinbox_clamp(self); self->dirty = 1; widget_emit_simple(self, EVENT_CHANGE);
}

static void spinbox_commit_edit(Widget *self) {
    SpinboxData *data = (SpinboxData *)self->extra;
    if (!data->editing) return;
    if (data->use_float) data->float_value = atof(data->edit_buf);
    else data->value = atoi(data->edit_buf);
    spinbox_clamp(self); data->editing = 0; self->dirty = 1; widget_emit_simple(self, EVENT_CHANGE);
}

static void spinbox_start_edit(Widget *self) {
    SpinboxData *data = (SpinboxData *)self->extra;
    data->editing = 1;
    spinbox_format_value(self, data->edit_buf, sizeof(data->edit_buf));
    data->cursor_pos = strlen(data->edit_buf);
    data->cursor_visible = 1; data->cursor_timer = 0;
}

static void spinbox_draw(Widget *self, BITMAP *target) {
    SpinboxData *data = (SpinboxData *)self->extra;
    int btn_w = SPINBOX_BTN_W;
    int text_w = self->w - btn_w;
    int half_h = self->h / 2;
    int border_col = self->focused ? theme.input_focus_border : theme.input_border;
    rectfill(target, self->abs_x, self->abs_y, self->abs_x + text_w - 1, self->abs_y + self->h - 1, theme.input_bg);
    rect(target, self->abs_x, self->abs_y, self->abs_x + self->w - 1, self->abs_y + self->h - 1, border_col);
    char display[32]; const char *show_text;
    if (data->editing) show_text = data->edit_buf;
    else { spinbox_format_value(self, display, sizeof(display)); show_text = display; }
    int text_y = self->abs_y + (self->h - text_height(font)) / 2;
    set_clip_rect(target, self->abs_x + 2, self->abs_y + 1, self->abs_x + text_w - 2, self->abs_y + self->h - 2);
    int col = self->enabled ? theme.input_fg : theme.disabled_fg;
    textout_ex(target, font, show_text, self->abs_x + 4, text_y, col, -1);
    if (self->focused && data->editing && data->cursor_visible) {
        char temp[32]; strncpy(temp, data->edit_buf, data->cursor_pos); temp[data->cursor_pos] = 0;
        int cx = self->abs_x + 4 + text_length(font, temp);
        vline(target, cx, self->abs_y + 3, self->abs_y + self->h - 4, theme.input_fg);
    }
    set_clip_rect(target, 0, 0, target->w - 1, target->h - 1);
    int btn_x = self->abs_x + text_w;
    int btn_up_y = self->abs_y;
    if (data->btn_up_pressed) rectfill(target, btn_x, btn_up_y, btn_x + btn_w - 1, btn_up_y + half_h - 1, theme.button_shadow);
    else if (data->btn_up_hovered) rectfill(target, btn_x, btn_up_y, btn_x + btn_w - 1, btn_up_y + half_h - 1, theme.button_highlight);
    else rectfill(target, btn_x, btn_up_y, btn_x + btn_w - 1, btn_up_y + half_h - 1, theme.button_face);
    rect(target, btn_x, btn_up_y, btn_x + btn_w - 1, btn_up_y + half_h - 1, theme.border);
    { int ax = btn_x + btn_w / 2, ay = btn_up_y + half_h / 2 - 1, off = data->btn_up_pressed ? 1 : 0;
      int ac = self->enabled ? theme.fg : theme.disabled_fg;
      triangle(target, ax + off, ay - 2 + off, ax - 3 + off, ay + 2 + off, ax + 3 + off, ay + 2 + off, ac); }
    int btn_down_y = self->abs_y + half_h;
    if (data->btn_down_pressed) rectfill(target, btn_x, btn_down_y, btn_x + btn_w - 1, btn_down_y + half_h - 1, theme.button_shadow);
    else if (data->btn_down_hovered) rectfill(target, btn_x, btn_down_y, btn_x + btn_w - 1, btn_down_y + half_h - 1, theme.button_highlight);
    else rectfill(target, btn_x, btn_down_y, btn_x + btn_w - 1, btn_down_y + half_h - 1, theme.button_face);
    rect(target, btn_x, btn_down_y, btn_x + btn_w - 1, btn_down_y + half_h - 1, theme.border);
    { int ax = btn_x + btn_w / 2, ay = btn_down_y + half_h / 2, off = data->btn_down_pressed ? 1 : 0;
      int ac = self->enabled ? theme.fg : theme.disabled_fg;
      triangle(target, ax + off, ay + 2 + off, ax - 3 + off, ay - 2 + off, ax + 3 + off, ay - 2 + off, ac); }
}

static void spinbox_mouse_down(Widget *self, Event *event) {
    SpinboxData *data = (SpinboxData *)self->extra;
    int btn_w = SPINBOX_BTN_W, btn_x = self->abs_x + self->w - btn_w, half_h = self->h / 2;
    if (point_in_rect(event->mouse_x, event->mouse_y, btn_x, self->abs_y, btn_w, half_h)) {
        data->btn_up_pressed = 1; spinbox_increment(self);
        data->repeat_timer = 0; data->repeat_active = 0; data->repeat_dir = 1;
        gui_state.captured_widget = self;
        if (data->editing) spinbox_commit_edit(self);
        self->dirty = 1; event->handled = 1; return;
    }
    if (point_in_rect(event->mouse_x, event->mouse_y, btn_x, self->abs_y + half_h, btn_w, half_h)) {
        data->btn_down_pressed = 1; spinbox_decrement(self);
        data->repeat_timer = 0; data->repeat_active = 0; data->repeat_dir = -1;
        gui_state.captured_widget = self;
        if (data->editing) spinbox_commit_edit(self);
        self->dirty = 1; event->handled = 1; return;
    }
    if (point_in_rect(event->mouse_x, event->mouse_y, self->abs_x, self->abs_y, self->w - btn_w, self->h)) {
        if (!data->editing) spinbox_start_edit(self);
        int text_x = event->mouse_x - self->abs_x - 4;
        int len = strlen(data->edit_buf); int pos;
        for (pos = 0; pos <= len; pos++) {
            char temp[32]; strncpy(temp, data->edit_buf, pos); temp[pos] = 0;
            if (text_length(font, temp) >= text_x) break;
        }
        data->cursor_pos = clamp_int(pos, 0, len);
        data->cursor_visible = 1; data->cursor_timer = 0; self->dirty = 1;
    }
}

static void spinbox_mouse_up(Widget *self, Event *event) {
    SpinboxData *data = (SpinboxData *)self->extra;
    (void)event;
    data->btn_up_pressed = 0; data->btn_down_pressed = 0;
    data->repeat_active = 0; data->repeat_timer = 0; data->repeat_dir = 0;
    if (gui_state.captured_widget == self) gui_state.captured_widget = NULL;
    self->dirty = 1;
}

static void spinbox_mouse_move(Widget *self, Event *event) {
    SpinboxData *data = (SpinboxData *)self->extra;
    int btn_w = SPINBOX_BTN_W, btn_x = self->abs_x + self->w - btn_w, half_h = self->h / 2;
    int old_up = data->btn_up_hovered, old_down = data->btn_down_hovered;
    data->btn_up_hovered = point_in_rect(event->mouse_x, event->mouse_y, btn_x, self->abs_y, btn_w, half_h);
    data->btn_down_hovered = point_in_rect(event->mouse_x, event->mouse_y, btn_x, self->abs_y + half_h, btn_w, half_h);
    if (old_up != data->btn_up_hovered || old_down != data->btn_down_hovered) self->dirty = 1;
}

static void spinbox_key_handler(Widget *self, Event *event) {
    SpinboxData *data = (SpinboxData *)self->extra;
    if (event->key == KEY_UP) { if (data->editing) spinbox_commit_edit(self); spinbox_increment(self); event->handled = 1; return; }
    if (event->key == KEY_DOWN) { if (data->editing) spinbox_commit_edit(self); spinbox_decrement(self); event->handled = 1; return; }
    if (event->key == KEY_PGUP) {
        if (data->editing) spinbox_commit_edit(self);
        if (data->use_float) data->float_value += data->float_step * 10; else data->value += data->step * 10;
        spinbox_clamp(self); self->dirty = 1; widget_emit_simple(self, EVENT_CHANGE); event->handled = 1; return;
    }
    if (event->key == KEY_PGDN) {
        if (data->editing) spinbox_commit_edit(self);
        if (data->use_float) data->float_value -= data->float_step * 10; else data->value -= data->step * 10;
        spinbox_clamp(self); self->dirty = 1; widget_emit_simple(self, EVENT_CHANGE); event->handled = 1; return;
    }
    if (event->key == KEY_ENTER || event->key == KEY_ENTER_PAD) { if (data->editing) { spinbox_commit_edit(self); event->handled = 1; } return; }
    if (event->key == KEY_ESC) { if (data->editing) { data->editing = 0; self->dirty = 1; event->handled = 1; } return; }
    if (!data->editing) {
        int ch = event->key_char;
        if ((ch >= '0' && ch <= '9') || ch == '-' || ch == '.') { spinbox_start_edit(self); data->edit_buf[0] = 0; data->cursor_pos = 0; }
        else return;
    }
    int ch = event->key_char;
    int len = strlen(data->edit_buf);
    if (event->key == KEY_LEFT) { if (data->cursor_pos > 0) data->cursor_pos--; event->handled = 1; }
    else if (event->key == KEY_RIGHT) { if (data->cursor_pos < len) data->cursor_pos++; event->handled = 1; }
    else if (event->key == KEY_HOME) { data->cursor_pos = 0; event->handled = 1; }
    else if (event->key == KEY_END) { data->cursor_pos = len; event->handled = 1; }
    else if (event->key == KEY_BACKSPACE) { if (data->cursor_pos > 0) { memmove(&data->edit_buf[data->cursor_pos - 1], &data->edit_buf[data->cursor_pos], len - data->cursor_pos + 1); data->cursor_pos--; } event->handled = 1; }
    else if (event->key == KEY_DEL) { if (data->cursor_pos < len) { memmove(&data->edit_buf[data->cursor_pos], &data->edit_buf[data->cursor_pos + 1], len - data->cursor_pos); } event->handled = 1; }
    else if (len < 30) {
        int valid = 0;
        if (ch >= '0' && ch <= '9') valid = 1;
        if (ch == '-' && data->cursor_pos == 0 && data->edit_buf[0] != '-') valid = 1;
        if (ch == '.' && data->use_float && !strchr(data->edit_buf, '.')) valid = 1;
        if (valid) { memmove(&data->edit_buf[data->cursor_pos + 1], &data->edit_buf[data->cursor_pos], len - data->cursor_pos + 1); data->edit_buf[data->cursor_pos] = (char)ch; data->cursor_pos++; event->handled = 1; }
    }
    data->cursor_visible = 1; data->cursor_timer = 0; self->dirty = 1;
}

static void spinbox_update(Widget *self) {
    SpinboxData *data = (SpinboxData *)self->extra;
    if (self->focused && data->editing) { data->cursor_timer++; if (data->cursor_timer > 30) { data->cursor_visible = !data->cursor_visible; data->cursor_timer = 0; self->dirty = 1; } }
    if (data->btn_up_pressed || data->btn_down_pressed) {
        data->repeat_timer++;
        if (!data->repeat_active && data->repeat_timer > 25) { data->repeat_active = 1; data->repeat_timer = 0; }
        if (data->repeat_active && data->repeat_timer > 4) { data->repeat_timer = 0; if (data->repeat_dir > 0) spinbox_increment(self); else spinbox_decrement(self); }
    }
}

static void spinbox_focus_lost(Widget *self, Event *event) {
    SpinboxData *data = (SpinboxData *)self->extra;
    (void)event;
    if (data->editing) spinbox_commit_edit(self);
    data->btn_up_hovered = 0; data->btn_down_hovered = 0;
}

Widget *spinbox_create(int x, int y, int w, int min_val, int max_val) {
    if (w <= 0) w = 100;
    Widget *wid = widget_create(WIDGET_SPINBOX, x, y, w, INPUT_HEIGHT);
    SpinboxData *data = (SpinboxData *)calloc(1, sizeof(SpinboxData));
    data->min_value = min_val; data->max_value = max_val; data->value = min_val; data->step = 1; data->cursor_visible = 1;
    wid->extra = data; wid->draw = spinbox_draw; wid->update = spinbox_update;
    widget_on(wid, EVENT_MOUSE_DOWN, spinbox_mouse_down);
    widget_on(wid, EVENT_MOUSE_UP, spinbox_mouse_up);
    widget_on(wid, EVENT_MOUSE_MOVE, spinbox_mouse_move);
    widget_on(wid, EVENT_KEY_DOWN, spinbox_key_handler);
    widget_on(wid, EVENT_BLUR, spinbox_focus_lost);
    return wid;
}

Widget *spinbox_create_float(int x, int y, int w, float min_val, float max_val, float step, int decimal_places) {
    Widget *wid = spinbox_create(x, y, w, 0, 0);
    SpinboxData *data = (SpinboxData *)wid->extra;
    data->use_float = 1; data->float_min = min_val; data->float_max = max_val;
    data->float_value = min_val; data->float_step = step; data->decimal_places = decimal_places;
    return wid;
}

void spinbox_set_value(Widget *w, int value) { SpinboxData *d = (SpinboxData *)w->extra; d->value = value; spinbox_clamp(w); w->dirty = 1; }
int spinbox_get_value(Widget *w) { return ((SpinboxData *)w->extra)->value; }
void spinbox_set_float_value(Widget *w, float value) { SpinboxData *d = (SpinboxData *)w->extra; d->float_value = value; spinbox_clamp(w); w->dirty = 1; }
float spinbox_get_float_value(Widget *w) { return ((SpinboxData *)w->extra)->float_value; }
void spinbox_set_step(Widget *w, int step) { ((SpinboxData *)w->extra)->step = step > 0 ? step : 1; }
