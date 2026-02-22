#include "gui_slider.h"
#include "gui_widget.h"
#include "gui_utils.h"
#include "gui_state.h"
#include <stdlib.h>
#include <stdio.h>

static void slider_draw(Widget *self, BITMAP *target) {
    SliderData *data = (SliderData *)self->extra;
    int range = data->max_value - data->min_value;
    if (range <= 0) return;
    int track_col = data->track_color ? data->track_color : theme.scrollbar_bg;
    int fill_col = data->fill_color ? data->fill_color : theme.titlebar_active;
    int thumb_col = data->thumb_color ? data->thumb_color : theme.button_face;
    if (!self->enabled) { fill_col = theme.disabled_fg; thumb_col = theme.scrollbar_bg; }
    int th = text_height(font);
    int value_space = data->show_value ? th + 2 : 0;
    if (data->horizontal) {
        int track_x = self->abs_x + 8, track_w = self->w - 16;
        int track_y = self->abs_y + value_space + (self->h - value_space) / 2;
        int fill_w = (data->value - data->min_value) * track_w / range;
        rectfill(target, track_x, track_y - 2, track_x + track_w - 1, track_y + 2, track_col);
        rect(target, track_x, track_y - 2, track_x + track_w - 1, track_y + 2, theme.border);
        if (fill_w > 0) rectfill(target, track_x, track_y - 2, track_x + fill_w, track_y + 2, fill_col);
        if (data->tick_interval > 0) {
            for (int v = data->min_value; v <= data->max_value; v += data->tick_interval) {
                int tick_x = track_x + (v - data->min_value) * track_w / range;
                vline(target, tick_x, track_y + 5, track_y + 9, theme.border);
            }
        }
        int thumb_w = 12, thumb_h = 16;
        int thumb_x = track_x + (data->value - data->min_value) * track_w / range - thumb_w / 2;
        int thumb_y = track_y - thumb_h / 2;
        if (self->hovered || data->dragging)
            rectfill(target, thumb_x, thumb_y, thumb_x + thumb_w - 1, thumb_y + thumb_h - 1, theme.button_highlight);
        else
            rectfill(target, thumb_x, thumb_y, thumb_x + thumb_w - 1, thumb_y + thumb_h - 1, thumb_col);
        rect(target, thumb_x, thumb_y, thumb_x + thumb_w - 1, thumb_y + thumb_h - 1, theme.border);
        vline(target, thumb_x + thumb_w / 2, thumb_y + 3, thumb_y + thumb_h - 4, theme.button_shadow);
        if (data->show_value) {
            char buf[16]; sprintf(buf, "%d", data->value);
            textout_centre_ex(target, font, buf, thumb_x + thumb_w / 2, self->abs_y + 1, theme.fg, -1);
        }
        if (self->focused) rect(target, self->abs_x + 1, self->abs_y + 1, self->abs_x + self->w - 2, self->abs_y + self->h - 2, theme.titlebar_active);
    } else {
        int track_x = self->abs_x + self->w / 2, track_y = self->abs_y + 8, track_h = self->h - 16;
        int fill_h = (data->value - data->min_value) * track_h / range;
        rectfill(target, track_x - 2, track_y, track_x + 2, track_y + track_h - 1, track_col);
        rect(target, track_x - 2, track_y, track_x + 2, track_y + track_h - 1, theme.border);
        if (fill_h > 0) rectfill(target, track_x - 2, track_y + track_h - fill_h, track_x + 2, track_y + track_h - 1, fill_col);
        if (data->tick_interval > 0) {
            for (int v = data->min_value; v <= data->max_value; v += data->tick_interval) {
                int tick_y = track_y + track_h - (v - data->min_value) * track_h / range;
                hline(target, track_x + 5, tick_y, track_x + 9, theme.border);
            }
        }
        int thumb_w = 16, thumb_h = 12;
        int thumb_x = track_x - thumb_w / 2;
        int thumb_y_pos = track_y + track_h - (data->value - data->min_value) * track_h / range - thumb_h / 2;
        if (self->hovered || data->dragging)
            rectfill(target, thumb_x, thumb_y_pos, thumb_x + thumb_w - 1, thumb_y_pos + thumb_h - 1, theme.button_highlight);
        else
            rectfill(target, thumb_x, thumb_y_pos, thumb_x + thumb_w - 1, thumb_y_pos + thumb_h - 1, thumb_col);
        rect(target, thumb_x, thumb_y_pos, thumb_x + thumb_w - 1, thumb_y_pos + thumb_h - 1, theme.border);
        hline(target, thumb_x + 3, thumb_y_pos + thumb_h / 2, thumb_x + thumb_w - 4, theme.button_shadow);
        if (data->show_value) {
            char buf[16]; sprintf(buf, "%d", data->value);
            textout_ex(target, font, buf, thumb_x + thumb_w + 4, thumb_y_pos + (thumb_h - th) / 2, theme.fg, -1);
        }
        if (self->focused) rect(target, self->abs_x + 1, self->abs_y + 1, self->abs_x + self->w - 2, self->abs_y + self->h - 2, theme.titlebar_active);
    }
}

static int slider_thumb_rect(Widget *self, int *tx, int *ty, int *tw, int *tth) {
    SliderData *data = (SliderData *)self->extra;
    int range = data->max_value - data->min_value;
    if (range <= 0) return 0;
    int value_space = data->show_value ? text_height(font) + 2 : 0;
    if (data->horizontal) {
        int track_x = self->abs_x + 8, track_w = self->w - 16;
        int track_y = self->abs_y + value_space + (self->h - value_space) / 2;
        *tw = 12; *tth = 16;
        *tx = track_x + (data->value - data->min_value) * track_w / range - (*tw) / 2;
        *ty = track_y - (*tth) / 2;
    } else {
        int track_y = self->abs_y + 8, track_h = self->h - 16;
        *tw = 16; *tth = 12;
        *tx = self->abs_x + self->w / 2 - (*tw) / 2;
        *ty = track_y + track_h - (data->value - data->min_value) * track_h / range - (*tth) / 2;
    }
    return 1;
}

static void slider_update_value_from_mouse(Widget *self, int mx, int my) {
    SliderData *data = (SliderData *)self->extra;
    int range = data->max_value - data->min_value;
    if (range <= 0) return;
    int new_value;
    if (data->horizontal) {
        int track_x = self->abs_x + 8, track_w = self->w - 16;
        if (track_w <= 0) return;
        new_value = data->min_value + (mx - track_x) * range / track_w;
    } else {
        int track_y = self->abs_y + 8, track_h = self->h - 16;
        if (track_h <= 0) return;
        new_value = data->max_value - (my - track_y) * range / track_h;
    }
    if (data->step > 1) new_value = ((new_value - data->min_value + data->step / 2) / data->step) * data->step + data->min_value;
    new_value = clamp_int(new_value, data->min_value, data->max_value);
    if (new_value != data->value) { data->value = new_value; self->dirty = 1; widget_emit_simple(self, EVENT_CHANGE); }
}

static void slider_mouse_down(Widget *self, Event *event) {
    SliderData *data = (SliderData *)self->extra;
    int ttx, tty, ttw, tth;
    if (slider_thumb_rect(self, &ttx, &tty, &ttw, &tth) && point_in_rect(event->mouse_x, event->mouse_y, ttx, tty, ttw, tth)) {
        data->dragging = 1; gui_state.captured_widget = self;
    } else {
        slider_update_value_from_mouse(self, event->mouse_x, event->mouse_y);
        data->dragging = 1; gui_state.captured_widget = self;
    }
    self->dirty = 1;
}

static void slider_mouse_move(Widget *self, Event *event) {
    SliderData *data = (SliderData *)self->extra;
    if (data->dragging) slider_update_value_from_mouse(self, event->mouse_x, event->mouse_y);
}

static void slider_mouse_up(Widget *self, Event *event) {
    SliderData *data = (SliderData *)self->extra;
    (void)event;
    data->dragging = 0;
    if (gui_state.captured_widget == self) gui_state.captured_widget = NULL;
    self->dirty = 1;
}

static void slider_key_handler(Widget *self, Event *event) {
    SliderData *data = (SliderData *)self->extra;
    int step = data->step > 0 ? data->step : 1;
    int changed = 0;
    if (data->horizontal) {
        if (event->key == KEY_LEFT || event->key == KEY_DOWN) { data->value = clamp_int(data->value - step, data->min_value, data->max_value); changed = 1; }
        else if (event->key == KEY_RIGHT || event->key == KEY_UP) { data->value = clamp_int(data->value + step, data->min_value, data->max_value); changed = 1; }
    } else {
        if (event->key == KEY_DOWN || event->key == KEY_LEFT) { data->value = clamp_int(data->value - step, data->min_value, data->max_value); changed = 1; }
        else if (event->key == KEY_UP || event->key == KEY_RIGHT) { data->value = clamp_int(data->value + step, data->min_value, data->max_value); changed = 1; }
    }
    if (event->key == KEY_HOME) { data->value = data->min_value; changed = 1; }
    else if (event->key == KEY_END) { data->value = data->max_value; changed = 1; }
    else if (event->key == KEY_PGUP) { data->value = clamp_int(data->value + step * 10, data->min_value, data->max_value); changed = 1; }
    else if (event->key == KEY_PGDN) { data->value = clamp_int(data->value - step * 10, data->min_value, data->max_value); changed = 1; }
    if (changed) { self->dirty = 1; widget_emit_simple(self, EVENT_CHANGE); event->handled = 1; }
}

Widget *slider_create(int horizontal, int x, int y, int length, int min_val, int max_val) {
    int w, h;
    if (horizontal) { w = length; h = 40; } else { w = 40; h = length; }
    Widget *wid = widget_create(WIDGET_SLIDER, x, y, w, h);
    SliderData *sdata = (SliderData *)calloc(1, sizeof(SliderData));
    sdata->horizontal = horizontal; sdata->min_value = min_val; sdata->max_value = max_val;
    sdata->value = min_val; sdata->step = 1; sdata->show_value = 1;
    wid->extra = sdata; wid->draw = slider_draw;
    widget_on(wid, EVENT_MOUSE_DOWN, slider_mouse_down);
    widget_on(wid, EVENT_MOUSE_MOVE, slider_mouse_move);
    widget_on(wid, EVENT_MOUSE_UP, slider_mouse_up);
    widget_on(wid, EVENT_KEY_DOWN, slider_key_handler);
    return wid;
}

void slider_set_value(Widget *w, int value) { SliderData *d = (SliderData *)w->extra; d->value = clamp_int(value, d->min_value, d->max_value); w->dirty = 1; }
int slider_get_value(Widget *w) { return ((SliderData *)w->extra)->value; }
void slider_set_step(Widget *w, int step) { ((SliderData *)w->extra)->step = step > 0 ? step : 1; }
void slider_set_ticks(Widget *w, int interval) { ((SliderData *)w->extra)->tick_interval = interval; w->dirty = 1; }
void slider_set_show_value(Widget *w, int show) { ((SliderData *)w->extra)->show_value = show; w->dirty = 1; }
void slider_set_colors(Widget *w, int track, int fill, int thumb) { SliderData *d = (SliderData *)w->extra; d->track_color = track; d->fill_color = fill; d->thumb_color = thumb; w->dirty = 1; }
