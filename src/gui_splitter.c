#include "gui_splitter.h"
#include "gui_widget.h"
#include "gui_panel.h"
#include "gui_utils.h"
#include "gui_state.h"
#include <stdlib.h>

static void splitter_do_layout(Widget *self) {
    SplitterData *data = (SplitterData *)self->extra;
    if (!data->first_pane || !data->second_pane) return;

    int avail;
    if (data->horizontal) {
        avail = self->w;
    } else {
        avail = self->h;
    }

    if (data->split_pos < data->min_first) data->split_pos = data->min_first;
    if (data->split_pos > avail - SPLITTER_SIZE - data->min_second) {
        data->split_pos = avail - SPLITTER_SIZE - data->min_second;
    }
    if (data->split_pos < SPLITTER_MIN_PANE) data->split_pos = SPLITTER_MIN_PANE;

    if (data->horizontal) {
        data->first_pane->x = 0;
        data->first_pane->y = 0;
        data->first_pane->w = data->split_pos;
        data->first_pane->h = self->h;

        data->second_pane->x = data->split_pos + SPLITTER_SIZE;
        data->second_pane->y = 0;
        data->second_pane->w = self->w - data->split_pos - SPLITTER_SIZE;
        data->second_pane->h = self->h;
        if (data->second_pane->w < 0) data->second_pane->w = 0;
    } else {
        data->first_pane->x = 0;
        data->first_pane->y = 0;
        data->first_pane->w = self->w;
        data->first_pane->h = data->split_pos;

        data->second_pane->x = 0;
        data->second_pane->y = data->split_pos + SPLITTER_SIZE;
        data->second_pane->w = self->w;
        data->second_pane->h = self->h - data->split_pos - SPLITTER_SIZE;
        if (data->second_pane->h < 0) data->second_pane->h = 0;
    }

    data->first_pane->needs_layout = 1;
    data->second_pane->needs_layout = 1;
}

static void splitter_draw(Widget *self, BITMAP *target) {
    SplitterData *data = (SplitterData *)self->extra;

    /* Background */
    rectfill(target, self->abs_x, self->abs_y,
             self->abs_x + self->w - 1, self->abs_y + self->h - 1, theme.bg);

    /* Draw splitter bar */
    int sx, sy, sw, sh;
    if (data->horizontal) {
        sx = self->abs_x + data->split_pos;
        sy = self->abs_y;
        sw = SPLITTER_SIZE;
        sh = self->h;
    } else {
        sx = self->abs_x;
        sy = self->abs_y + data->split_pos;
        sw = self->w;
        sh = SPLITTER_SIZE;
    }

    int bar_col = data->dragging ? theme.button_shadow : theme.button_face;
    rectfill(target, sx, sy, sx + sw - 1, sy + sh - 1, bar_col);

    /* Draw grip dots */
    if (data->horizontal) {
        int cy = sy + sh / 2;
        int cx = sx + sw / 2;
        for (int d = -8; d <= 8; d += 4) {
            putpixel(target, cx - 1, cy + d, theme.button_shadow);
            putpixel(target, cx + 1, cy + d, theme.button_highlight);
            putpixel(target, cx, cy + d, theme.border);
        }
    } else {
        int cx = sx + sw / 2;
        int cy = sy + sh / 2;
        for (int d = -8; d <= 8; d += 4) {
            putpixel(target, cx + d, cy - 1, theme.button_shadow);
            putpixel(target, cx + d, cy + 1, theme.button_highlight);
            putpixel(target, cx + d, cy, theme.border);
        }
    }

    /* Border lines */
    if (data->horizontal) {
        vline(target, sx, sy, sy + sh - 1, theme.button_highlight);
        vline(target, sx + sw - 1, sy, sy + sh - 1, theme.button_shadow);
    } else {
        hline(target, sx, sy, sx + sw - 1, theme.button_highlight);
        hline(target, sx, sy + sh - 1, sx + sw - 1, theme.button_shadow);
    }
}

static int splitter_is_on_bar(Widget *self, int mx, int my) {
    SplitterData *data = (SplitterData *)self->extra;
    if (data->horizontal) {
        int sx = self->abs_x + data->split_pos;
        return mx >= sx && mx < sx + SPLITTER_SIZE &&
               my >= self->abs_y && my < self->abs_y + self->h;
    } else {
        int sy = self->abs_y + data->split_pos;
        return my >= sy && my < sy + SPLITTER_SIZE &&
               mx >= self->abs_x && mx < self->abs_x + self->w;
    }
}

static void splitter_mouse_down(Widget *self, Event *event) {
    SplitterData *data = (SplitterData *)self->extra;
    if (splitter_is_on_bar(self, event->mouse_x, event->mouse_y)) {
        data->dragging = 1;
        if (data->horizontal) {
            data->drag_offset = event->mouse_x - (self->abs_x + data->split_pos);
        } else {
            data->drag_offset = event->mouse_y - (self->abs_y + data->split_pos);
        }
        gui_state.captured_widget = self;
        self->dirty = 1;
        event->handled = 1;
    }
}

static void splitter_mouse_move(Widget *self, Event *event) {
    SplitterData *data = (SplitterData *)self->extra;
    if (data->dragging) {
        int new_pos;
        if (data->horizontal) {
            new_pos = event->mouse_x - self->abs_x - data->drag_offset;
        } else {
            new_pos = event->mouse_y - self->abs_y - data->drag_offset;
        }

        int avail = data->horizontal ? self->w : self->h;
        new_pos = clamp_int(new_pos, data->min_first,
                            avail - SPLITTER_SIZE - data->min_second);
        if (new_pos < SPLITTER_MIN_PANE) new_pos = SPLITTER_MIN_PANE;

        if (new_pos != data->split_pos) {
            data->split_pos = new_pos;
            self->needs_layout = 1;
            widget_mark_dirty(self);
        }
    }
}

static void splitter_mouse_up(Widget *self, Event *event) {
    SplitterData *data = (SplitterData *)self->extra;
    (void)event;
    if (data->dragging) {
        data->dragging = 0;
        if (gui_state.captured_widget == self) gui_state.captured_widget = NULL;
        self->dirty = 1;
    }
}

Widget *splitter_create(int horizontal, int x, int y, int w, int h) {
    Widget *wid = widget_create(WIDGET_BASE, x, y, w, h);
    SplitterData *data = (SplitterData *)calloc(1, sizeof(SplitterData));
    data->horizontal = horizontal;
    data->split_pos = horizontal ? w / 2 : h / 2;
    data->min_first = SPLITTER_MIN_PANE;
    data->min_second = SPLITTER_MIN_PANE;

    /* Create two panes */
    data->first_pane = panel_create(0, 0, 10, 10);
    data->second_pane = panel_create(0, 0, 10, 10);

    wid->extra = data;
    wid->draw = splitter_draw;
    wid->do_layout = splitter_do_layout;
    wid->clip_children = 1;

    widget_add_child(wid, data->first_pane);
    widget_add_child(wid, data->second_pane);

    widget_on(wid, EVENT_MOUSE_DOWN, splitter_mouse_down);
    widget_on(wid, EVENT_MOUSE_MOVE, splitter_mouse_move);
    widget_on(wid, EVENT_MOUSE_UP, splitter_mouse_up);

    return wid;
}

Widget *splitter_get_first(Widget *w) {
    SplitterData *data = (SplitterData *)w->extra;
    return data->first_pane;
}

Widget *splitter_get_second(Widget *w) {
    SplitterData *data = (SplitterData *)w->extra;
    return data->second_pane;
}

void splitter_set_position(Widget *w, int pos) {
    SplitterData *data = (SplitterData *)w->extra;
    data->split_pos = pos;
    w->needs_layout = 1;
    widget_mark_dirty(w);
}

int splitter_get_position(Widget *w) {
    return ((SplitterData *)w->extra)->split_pos;
}

void splitter_set_min_sizes(Widget *w, int min_first, int min_second) {
    SplitterData *data = (SplitterData *)w->extra;
    data->min_first = min_first > 0 ? min_first : SPLITTER_MIN_PANE;
    data->min_second = min_second > 0 ? min_second : SPLITTER_MIN_PANE;
}
