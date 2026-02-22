#include "gui_combobox.h"
#include "gui_widget.h"
#include "gui_utils.h"
#include "gui_state.h"
#include <stdlib.h>
#include <string.h>

static void combobox_draw(Widget *self, BITMAP *target) {
    ComboboxData *data = (ComboboxData *)self->extra;
    if (!data) return;

    rectfill(target, self->abs_x, self->abs_y,
             self->abs_x + self->w - 1, self->abs_y + self->h - 1, theme.input_bg);
    rect(target, self->abs_x, self->abs_y,
         self->abs_x + self->w - 1, self->abs_y + self->h - 1,
         self->focused ? theme.input_focus_border : theme.input_border);

    set_clip_rect(target, self->abs_x + 2, self->abs_y + 1,
                  self->abs_x + self->w - 20, self->abs_y + self->h - 2);

    if (data->selected_index >= 0 && data->selected_index < data->item_count) {
        textout_ex(target, font, data->items[data->selected_index],
                   self->abs_x + 4, self->abs_y + (self->h - text_height(font)) / 2,
                   theme.input_fg, -1);
    }

    set_clip_rect(target, 0, 0, target->w - 1, target->h - 1);

    int arrow_x = self->abs_x + self->w - 18;
    int arrow_pressed = data->open;
    if (arrow_pressed)
        draw_sunken_rect(target, arrow_x, self->abs_y + 1, 17, self->h - 2);
    else
        draw_raised_rect(target, arrow_x, self->abs_y + 1, 17, self->h - 2);

    int ax = arrow_x + 4;
    int ay = self->abs_y + self->h / 2 - 1;
    int off = arrow_pressed ? 1 : 0;
    for (int i = 0; i < 4; i++) {
        hline(target, ax + i + off, ay + i + off, ax + 8 - i + off, theme.fg);
    }
}

void combobox_draw_dropdown(Widget *self, BITMAP *target) {
    ComboboxData *data = (ComboboxData *)self->extra;
    if (!data || !data->open) return;

    int item_h = text_height(font) + 6;
    int dd_x = self->abs_x;
    int dd_y = self->abs_y + self->h;
    int dd_w = self->w;
    int visible_items = data->item_count;
    if (visible_items > 10) visible_items = 10;
    int dd_h = visible_items * item_h + 2;

    if (dd_y + dd_h > gui_state.screen_h) {
        dd_y = self->abs_y - dd_h;
        if (dd_y < 0) dd_y = 0;
    }

    data->dropdown_y = dd_y;
    data->dropdown_height = dd_h;

    rectfill(target, dd_x, dd_y, dd_x + dd_w - 1, dd_y + dd_h - 1, theme.menu_bg);
    rect(target, dd_x, dd_y, dd_x + dd_w - 1, dd_y + dd_h - 1, theme.border);

    set_clip_rect(target, dd_x + 1, dd_y + 1, dd_x + dd_w - 2, dd_y + dd_h - 2);

    for (int i = 0; i < data->item_count; i++) {
        int iy = dd_y + 1 + i * item_h;
        if (iy + item_h < dd_y || iy > dd_y + dd_h) continue;
        if (i == data->hover_index) {
            rectfill(target, dd_x + 1, iy, dd_x + dd_w - 2, iy + item_h - 1, theme.menu_highlight);
            textout_ex(target, font, data->items[i], dd_x + 6, iy + 3, theme.menu_highlight_fg, -1);
        } else if (i == data->selected_index) {
            rectfill(target, dd_x + 1, iy, dd_x + dd_w - 2, iy + item_h - 1, makecol(220, 230, 245));
            textout_ex(target, font, data->items[i], dd_x + 6, iy + 3, theme.menu_fg, -1);
        } else {
            textout_ex(target, font, data->items[i], dd_x + 6, iy + 3, theme.menu_fg, -1);
        }
    }

    set_clip_rect(target, 0, 0, target->w - 1, target->h - 1);
}

int combobox_hit_dropdown(Widget *self, int mx, int my) {
    ComboboxData *data = (ComboboxData *)self->extra;
    if (!data || !data->open) return 0;
    int dd_x = self->abs_x;
    int dd_y = data->dropdown_y;
    int dd_w = self->w;
    int dd_h = data->dropdown_height;
    return point_in_rect(mx, my, dd_x, dd_y, dd_w, dd_h);
}

int combobox_dropdown_item_at(Widget *self, int mx, int my) {
    ComboboxData *data = (ComboboxData *)self->extra;
    (void)mx;
    if (!data || !data->open) return -1;
    int item_h = text_height(font) + 6;
    int dd_y = data->dropdown_y + 1;
    int idx = (my - dd_y) / item_h;
    if (idx >= 0 && idx < data->item_count) return idx;
    return -1;
}

Widget *combobox_create(int x, int y, int w) {
    Widget *wid = widget_create(WIDGET_COMBOBOX, x, y, w, INPUT_HEIGHT);
    ComboboxData *data = (ComboboxData *)calloc(1, sizeof(ComboboxData));
    data->selected_index = -1;
    data->hover_index = -1;
    wid->extra = data;
    wid->draw = combobox_draw;
    return wid;
}

void combobox_add_item(Widget *w, const char *text) {
    ComboboxData *data = (ComboboxData *)w->extra;
    if (!data) return;
    if (data->item_count < MAX_COMBO_ITEMS) {
        strncpy(data->items[data->item_count], text, MAX_TEXT_LEN - 1);
        data->item_count++;
        if (data->selected_index < 0) data->selected_index = 0;
    }
}

Widget *find_open_combobox_recursive(Widget *w) {
    if (!w || !w->visible) return NULL;
    if (w->type == WIDGET_COMBOBOX) {
        ComboboxData *cd = (ComboboxData *)w->extra;
        if (cd && cd->open) return w;
    }
    for (int i = 0; i < w->child_count; i++) {
        Widget *found = find_open_combobox_recursive(w->children[i]);
        if (found) return found;
    }
    return NULL;
}
