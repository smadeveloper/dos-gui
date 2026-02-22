#include "gui_radio.h"
#include "gui_widget.h"
#include "gui_utils.h"
#include <stdlib.h>
#include <string.h>

static void radio_draw(Widget *self, BITMAP *target) {
    RadioData *data = (RadioData *)self->extra;
    if (!data) return;

    int x = self->abs_x, y = self->abs_y;
    int cx = x + RADIO_SIZE / 2;
    int cy = y + self->h / 2;
    int r = RADIO_SIZE / 2 - 1;

    /* Circle background */
    circlefill(target, cx, cy, r, theme.checkbox_bg);
    circle(target, cx, cy, r, self->focused ? theme.input_focus_border : theme.input_border);

    /* Hover */
    if (self->hovered) {
        circle(target, cx, cy, r - 1, theme.checkbox_check);
    }

    /* Selected dot */
    if (data->selected) {
        circlefill(target, cx, cy, r - 3, theme.checkbox_check);
    }

    /* Label */
    int text_col = self->enabled ? theme.fg : theme.disabled_fg;
    textout_ex(target, font, data->text,
               x + RADIO_SIZE + 6, y + (self->h - text_height(font)) / 2,
               text_col, -1);
}

Widget *radio_create(const char *text, int group_id, int x, int y) {
    int w = RADIO_SIZE + 6 + text_length(font, text) + 4;
    int h = RADIO_SIZE > text_height(font) ? RADIO_SIZE + 4 : text_height(font) + 4;

    Widget *wid = widget_create(WIDGET_RADIO, x, y, w, h);
    RadioData *data = (RadioData *)calloc(1, sizeof(RadioData));
    strncpy(data->text, text, MAX_TEXT_LEN - 1);
    data->group_id = group_id;
    wid->extra = data;
    wid->draw = radio_draw;
    return wid;
}
