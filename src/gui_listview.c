#include "gui_listview.h"
#include "gui_widget.h"
#include "gui_utils.h"
#include "gui_state.h"
#include <stdlib.h>
#include <string.h>

static int listview_visible_items(Widget *self) {
    int header = 0;
    ListViewData *data = (ListViewData *)self->extra;
    if (data->show_headers) header = LISTVIEW_ITEM_HEIGHT;
    return (self->h - 2 - header) / LISTVIEW_ITEM_HEIGHT;
}

static int listview_max_scroll(Widget *self) {
    ListViewData *data = (ListViewData *)self->extra;
    int vis = listview_visible_items(self);
    int max_s = data->item_count - vis;
    return max_s > 0 ? max_s : 0;
}

static int listview_scrollbar_w(Widget *self) {
    ListViewData *data = (ListViewData *)self->extra;
    int vis = listview_visible_items(self);
    if (data->item_count > vis) return SCROLLBAR_SIZE;
    return 0;
}

static void listview_draw(Widget *self, BITMAP *target) {
    ListViewData *data = (ListViewData *)self->extra;

    int border_col = self->focused ? theme.input_focus_border : theme.input_border;

    /* Background */
    rectfill(target, self->abs_x, self->abs_y,
             self->abs_x + self->w - 1, self->abs_y + self->h - 1, theme.input_bg);
    rect(target, self->abs_x, self->abs_y,
         self->abs_x + self->w - 1, self->abs_y + self->h - 1, border_col);

    int sb_w = listview_scrollbar_w(self);
    int content_w = self->w - 2 - sb_w;
    int header_h = 0;

    /* Header */
    if (data->show_headers && data->header_text[0]) {
        header_h = LISTVIEW_ITEM_HEIGHT;
        rectfill(target, self->abs_x + 1, self->abs_y + 1,
                 self->abs_x + content_w, self->abs_y + header_h, theme.button_face);
        hline(target, self->abs_x + 1, self->abs_y + header_h, self->abs_x + content_w, theme.border);
        textout_ex(target, font, data->header_text,
                   self->abs_x + 6, self->abs_y + (header_h - text_height(font)) / 2 + 1,
                   theme.fg, -1);
    }

    int vis = listview_visible_items(self);
    int start_y = self->abs_y + 1 + header_h;

    set_clip_rect(target, self->abs_x + 1, start_y,
                  self->abs_x + content_w, self->abs_y + self->h - 2);

    for (int i = 0; i < vis && (i + data->scroll_offset) < data->item_count; i++) {
        int idx = i + data->scroll_offset;
        int iy = start_y + i * LISTVIEW_ITEM_HEIGHT;

        int is_selected = (idx == data->selected_index);
        if (data->multi_select && idx < MAX_LISTVIEW_ITEMS) {
            is_selected = data->selected_flags[idx];
        }

        if (is_selected) {
            rectfill(target, self->abs_x + 1, iy,
                     self->abs_x + content_w, iy + LISTVIEW_ITEM_HEIGHT - 1,
                     theme.selection_bg);
            textout_ex(target, font, data->items[idx],
                       self->abs_x + 6, iy + (LISTVIEW_ITEM_HEIGHT - text_height(font)) / 2,
                       theme.selection_fg, -1);
        } else if (idx == data->hover_index) {
            rectfill(target, self->abs_x + 1, iy,
                     self->abs_x + content_w, iy + LISTVIEW_ITEM_HEIGHT - 1,
                     makecol(230, 240, 250));
            textout_ex(target, font, data->items[idx],
                       self->abs_x + 6, iy + (LISTVIEW_ITEM_HEIGHT - text_height(font)) / 2,
                       theme.fg, -1);
        } else {
            textout_ex(target, font, data->items[idx],
                       self->abs_x + 6, iy + (LISTVIEW_ITEM_HEIGHT - text_height(font)) / 2,
                       theme.fg, -1);
        }
    }

    set_clip_rect(target, 0, 0, target->w - 1, target->h - 1);

    /* Scrollbar */
    if (sb_w > 0) {
        int sb_x = self->abs_x + self->w - sb_w - 1;
        int sb_y = self->abs_y + 1 + header_h;
        int sb_h = self->h - 2 - header_h;
        int max_s = listview_max_scroll(self);

        rectfill(target, sb_x, sb_y, sb_x + sb_w - 1, sb_y + sb_h - 1, theme.scrollbar_bg);
        rect(target, sb_x, sb_y, sb_x + sb_w - 1, sb_y + sb_h - 1, theme.border);

        if (max_s > 0) {
            int thumb_h = clamp_int(vis * sb_h / data->item_count, 20, sb_h);
            int thumb_y = sb_y + data->scroll_offset * (sb_h - thumb_h) / max_s;
            rectfill(target, sb_x + 1, thumb_y, sb_x + sb_w - 2, thumb_y + thumb_h - 1,
                     theme.scrollbar_thumb);
        }
    }
}

static void listview_mouse_down(Widget *self, Event *event) {
    ListViewData *data = (ListViewData *)self->extra;

    int sb_w = listview_scrollbar_w(self);
    int header_h = (data->show_headers && data->header_text[0]) ? LISTVIEW_ITEM_HEIGHT : 0;

    /* Check scrollbar area */
    if (sb_w > 0) {
        int sb_x = self->abs_x + self->w - sb_w - 1;
        if (event->mouse_x >= sb_x) {
            int sb_y = self->abs_y + 1 + header_h;
            int sb_h = self->h - 2 - header_h;
            int max_s = listview_max_scroll(self);
            int vis = listview_visible_items(self);

            if (max_s > 0) {
                int thumb_h = clamp_int(vis * sb_h / data->item_count, 20, sb_h);
                int thumb_y = sb_y + data->scroll_offset * (sb_h - thumb_h) / max_s;

                if (event->mouse_y >= thumb_y && event->mouse_y < thumb_y + thumb_h) {
                    data->dragging_scroll = 1;
                    data->drag_offset = event->mouse_y - thumb_y;
                    gui_state.captured_widget = self;
                } else {
                    /* Page up/down */
                    if (event->mouse_y < thumb_y) {
                        data->scroll_offset = clamp_int(data->scroll_offset - vis, 0, max_s);
                    } else {
                        data->scroll_offset = clamp_int(data->scroll_offset + vis, 0, max_s);
                    }
                }
            }
            self->dirty = 1;
            event->handled = 1;
            return;
        }
    }

    /* Check items */
    int start_y = self->abs_y + 1 + header_h;
    int rel_y = event->mouse_y - start_y;
    if (rel_y >= 0) {
        int idx = rel_y / LISTVIEW_ITEM_HEIGHT + data->scroll_offset;
        if (idx >= 0 && idx < data->item_count) {
            if (data->multi_select) {
                data->selected_flags[idx] = !data->selected_flags[idx];
            }
            data->selected_index = idx;
            widget_emit_simple(self, EVENT_CHANGE);
        }
    }
    self->dirty = 1;
}

static void listview_mouse_move(Widget *self, Event *event) {
    ListViewData *data = (ListViewData *)self->extra;

    if (data->dragging_scroll) {
        int header_h = (data->show_headers && data->header_text[0]) ? LISTVIEW_ITEM_HEIGHT : 0;
        int sb_y = self->abs_y + 1 + header_h;
        int sb_h = self->h - 2 - header_h;
        int max_s = listview_max_scroll(self);
        int vis = listview_visible_items(self);

        if (max_s > 0) {
            int thumb_h = clamp_int(vis * sb_h / data->item_count, 20, sb_h);
            int new_pos = event->mouse_y - data->drag_offset - sb_y;
            new_pos = clamp_int(new_pos, 0, sb_h - thumb_h);
            data->scroll_offset = new_pos * max_s / (sb_h - thumb_h);
            data->scroll_offset = clamp_int(data->scroll_offset, 0, max_s);
        }
        self->dirty = 1;
    } else {
        int header_h = (data->show_headers && data->header_text[0]) ? LISTVIEW_ITEM_HEIGHT : 0;
        int start_y = self->abs_y + 1 + header_h;
        int sb_w = listview_scrollbar_w(self);
        int rel_y = event->mouse_y - start_y;

        if (event->mouse_x < self->abs_x + self->w - sb_w - 1 && rel_y >= 0) {
            int idx = rel_y / LISTVIEW_ITEM_HEIGHT + data->scroll_offset;
            if (idx >= 0 && idx < data->item_count) {
                if (data->hover_index != idx) {
                    data->hover_index = idx;
                    self->dirty = 1;
                }
            }
        } else {
            if (data->hover_index != -1) {
                data->hover_index = -1;
                self->dirty = 1;
            }
        }
    }
}

static void listview_mouse_up(Widget *self, Event *event) {
    ListViewData *data = (ListViewData *)self->extra;
    (void)event;
    if (data->dragging_scroll) {
        data->dragging_scroll = 0;
        if (gui_state.captured_widget == self) gui_state.captured_widget = NULL;
    }
}

static void listview_key_handler(Widget *self, Event *event) {
    ListViewData *data = (ListViewData *)self->extra;
    int max_s = listview_max_scroll(self);
    int changed = 0;

    if (event->key == KEY_UP) {
        if (data->selected_index > 0) {
            data->selected_index--;
            changed = 1;
        }
    } else if (event->key == KEY_DOWN) {
        if (data->selected_index < data->item_count - 1) {
            data->selected_index++;
            changed = 1;
        }
    } else if (event->key == KEY_PGUP) {
        int vis = listview_visible_items(self);
        data->selected_index = clamp_int(data->selected_index - vis, 0, data->item_count - 1);
        changed = 1;
    } else if (event->key == KEY_PGDN) {
        int vis = listview_visible_items(self);
        data->selected_index = clamp_int(data->selected_index + vis, 0, data->item_count - 1);
        changed = 1;
    } else if (event->key == KEY_HOME) {
        data->selected_index = 0;
        changed = 1;
    } else if (event->key == KEY_END) {
        data->selected_index = data->item_count - 1;
        changed = 1;
    }

    if (changed) {
        /* Ensure visible */
        int vis = listview_visible_items(self);
        if (data->selected_index < data->scroll_offset) {
            data->scroll_offset = data->selected_index;
        } else if (data->selected_index >= data->scroll_offset + vis) {
            data->scroll_offset = data->selected_index - vis + 1;
        }
        data->scroll_offset = clamp_int(data->scroll_offset, 0, max_s);
        widget_emit_simple(self, EVENT_CHANGE);
        self->dirty = 1;
        event->handled = 1;
    }
}

static void listview_scroll_handler(Widget *self, Event *event) {
    ListViewData *data = (ListViewData *)self->extra;
    int max_s = listview_max_scroll(self);
    data->scroll_offset = clamp_int(data->scroll_offset - event->delta * 3, 0, max_s);
    self->dirty = 1;
}

Widget *listview_create(int x, int y, int w, int h) {
    Widget *wid = widget_create(WIDGET_BASE, x, y, w, h);
    ListViewData *data = (ListViewData *)calloc(1, sizeof(ListViewData));
    data->selected_index = -1;
    data->hover_index = -1;
    wid->extra = data;
    wid->draw = listview_draw;
    widget_on(wid, EVENT_MOUSE_DOWN, listview_mouse_down);
    widget_on(wid, EVENT_MOUSE_MOVE, listview_mouse_move);
    widget_on(wid, EVENT_MOUSE_UP, listview_mouse_up);
    widget_on(wid, EVENT_KEY_DOWN, listview_key_handler);
    widget_on(wid, EVENT_SCROLL, listview_scroll_handler);
    return wid;
}

void listview_add_item(Widget *w, const char *text) {
    ListViewData *data = (ListViewData *)w->extra;
    if (data->item_count < MAX_LISTVIEW_ITEMS) {
        strncpy(data->items[data->item_count], text, MAX_TEXT_LEN - 1);
        data->item_count++;
        w->dirty = 1;
    }
}

void listview_remove_item(Widget *w, int index) {
    ListViewData *data = (ListViewData *)w->extra;
    if (index < 0 || index >= data->item_count) return;
    for (int i = index; i < data->item_count - 1; i++) {
        strncpy(data->items[i], data->items[i + 1], MAX_TEXT_LEN - 1);
        data->selected_flags[i] = data->selected_flags[i + 1];
    }
    data->item_count--;
    if (data->selected_index >= data->item_count) {
        data->selected_index = data->item_count - 1;
    }
    w->dirty = 1;
}

void listview_clear(Widget *w) {
    ListViewData *data = (ListViewData *)w->extra;
    data->item_count = 0;
    data->selected_index = -1;
    data->scroll_offset = 0;
    data->hover_index = -1;
    memset(data->selected_flags, 0, sizeof(data->selected_flags));
    w->dirty = 1;
}

int listview_get_selected(Widget *w) {
    return ((ListViewData *)w->extra)->selected_index;
}

const char *listview_get_item_text(Widget *w, int index) {
    ListViewData *data = (ListViewData *)w->extra;
    if (index < 0 || index >= data->item_count) return "";
    return data->items[index];
}

void listview_set_header(Widget *w, const char *text) {
    ListViewData *data = (ListViewData *)w->extra;
    strncpy(data->header_text, text, MAX_TEXT_LEN - 1);
    data->show_headers = 1;
    w->dirty = 1;
}

int listview_get_item_count(Widget *w) {
    return ((ListViewData *)w->extra)->item_count;
}
