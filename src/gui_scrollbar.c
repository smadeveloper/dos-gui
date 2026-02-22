#include "gui_scrollbar.h"
#include "gui_widget.h"
#include "gui_state.h"
#include "gui_utils.h"
#include <stdlib.h>

static void scrollbar_draw(Widget *self, BITMAP *target) {
    ScrollbarData *data = (ScrollbarData *)self->extra;
    rectfill(target, self->abs_x, self->abs_y,
             self->abs_x + self->w - 1, self->abs_y + self->h - 1, theme.scrollbar_bg);
    int range = data->max_value - data->min_value;
    if (range <= 0) return;
    if (data->horizontal) {
        int track = self->w - 2;
        data->thumb_size = clamp_int(data->page_size * track / (range + data->page_size), 20, track);
        data->thumb_pos = (data->value - data->min_value) * (track - data->thumb_size) / range;
        rectfill(target,
            self->abs_x + 1 + data->thumb_pos, self->abs_y + 1,
            self->abs_x + 1 + data->thumb_pos + data->thumb_size - 1, self->abs_y + self->h - 2,
            self->hovered ? theme.button_shadow : theme.scrollbar_thumb);
    } else {
        int track = self->h - 2;
        data->thumb_size = clamp_int(data->page_size * track / (range + data->page_size), 20, track);
        data->thumb_pos = (data->value - data->min_value) * (track - data->thumb_size) / range;
        rectfill(target,
            self->abs_x + 1, self->abs_y + 1 + data->thumb_pos,
            self->abs_x + self->w - 2, self->abs_y + 1 + data->thumb_pos + data->thumb_size - 1,
            self->hovered ? theme.button_shadow : theme.scrollbar_thumb);
    }
    rect(target, self->abs_x, self->abs_y,
         self->abs_x + self->w - 1, self->abs_y + self->h - 1, theme.border);
}

static void scrollbar_handle_mouse(Widget *self, Event *event) {
    ScrollbarData *data = (ScrollbarData *)self->extra;
    int range = data->max_value - data->min_value;
    if (range <= 0) return;
    if (event->type == EVENT_MOUSE_DOWN) {
        if (data->horizontal) {
            int track = self->w - 2;
            int thumb_screen = self->abs_x + 1 + data->thumb_pos;
            if (event->mouse_x >= thumb_screen && event->mouse_x < thumb_screen + data->thumb_size) {
                data->dragging_thumb = 1;
                data->drag_offset = event->mouse_x - thumb_screen;
                gui_state.captured_widget = self;
            } else {
                if (event->mouse_x < thumb_screen)
                    data->value = clamp_int(data->value - data->page_size, data->min_value, data->max_value);
                else
                    data->value = clamp_int(data->value + data->page_size, data->min_value, data->max_value);
                widget_emit_simple(self, EVENT_CHANGE);
            }
        } else {
            int track = self->h - 2;
            int thumb_screen = self->abs_y + 1 + data->thumb_pos;
            if (event->mouse_y >= thumb_screen && event->mouse_y < thumb_screen + data->thumb_size) {
                data->dragging_thumb = 1;
                data->drag_offset = event->mouse_y - thumb_screen;
                gui_state.captured_widget = self;
            } else {
                if (event->mouse_y < thumb_screen)
                    data->value = clamp_int(data->value - data->page_size, data->min_value, data->max_value);
                else
                    data->value = clamp_int(data->value + data->page_size, data->min_value, data->max_value);
                widget_emit_simple(self, EVENT_CHANGE);
            }
        }
        self->dirty = 1;
    } else if (event->type == EVENT_MOUSE_MOVE && data->dragging_thumb) {
        if (data->horizontal) {
            int track = self->w - 2;
            int new_pos = event->mouse_x - self->abs_x - 1 - data->drag_offset;
            new_pos = clamp_int(new_pos, 0, track - data->thumb_size);
            data->value = data->min_value + new_pos * range / (track - data->thumb_size);
        } else {
            int track = self->h - 2;
            int new_pos = event->mouse_y - self->abs_y - 1 - data->drag_offset;
            new_pos = clamp_int(new_pos, 0, track - data->thumb_size);
            data->value = data->min_value + new_pos * range / (track - data->thumb_size);
        }
        data->value = clamp_int(data->value, data->min_value, data->max_value);
        widget_emit_simple(self, EVENT_CHANGE);
        self->dirty = 1;
    } else if (event->type == EVENT_MOUSE_UP) {
        data->dragging_thumb = 0;
        if (gui_state.captured_widget == self) gui_state.captured_widget = NULL;
    }
}

Widget *scrollbar_create(int horizontal, int x, int y, int length) {
    int w = horizontal ? length : SCROLLBAR_SIZE;
    int h = horizontal ? SCROLLBAR_SIZE : length;
    Widget *wid = widget_create(WIDGET_SCROLLBAR, x, y, w, h);
    ScrollbarData *data = (ScrollbarData *)calloc(1, sizeof(ScrollbarData));
    data->horizontal = horizontal;
    data->max_value = 100;
    data->page_size = 10;
    wid->extra = data;
    wid->draw = scrollbar_draw;
    widget_on(wid, EVENT_MOUSE_DOWN, scrollbar_handle_mouse);
    widget_on(wid, EVENT_MOUSE_MOVE, scrollbar_handle_mouse);
    widget_on(wid, EVENT_MOUSE_UP, scrollbar_handle_mouse);
    return wid;
}
