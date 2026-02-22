#include "gui_groupbox.h"
#include "gui_widget.h"
#include "gui_utils.h"
#include <stdlib.h>
#include <string.h>

static void groupbox_draw(Widget *self, BITMAP *target) {
    GroupBoxData *data = (GroupBoxData *)self->extra;
    int border_col = data->border_color ? data->border_color : theme.border;
    int title_col = data->title_color ? data->title_color : theme.fg;
    int th = text_height(font);
    int tw = text_length(font, data->title);
    int title_y = self->abs_y + th / 2;

    /* Background */
    rectfill(target, self->abs_x, self->abs_y + th / 2,
             self->abs_x + self->w - 1, self->abs_y + self->h - 1, theme.bg);

    /* Border - draw around but skip title area */
    /* Top line - left part */
    hline(target, self->abs_x, title_y, self->abs_x + 8, border_col);
    /* Top line - right part (after title) */
    hline(target, self->abs_x + 14 + tw, title_y, self->abs_x + self->w - 1, border_col);
    /* Left */
    vline(target, self->abs_x, title_y, self->abs_y + self->h - 1, border_col);
    /* Right */
    vline(target, self->abs_x + self->w - 1, title_y, self->abs_y + self->h - 1, border_col);
    /* Bottom */
    hline(target, self->abs_x, self->abs_y + self->h - 1, self->abs_x + self->w - 1, border_col);

    /* Title text */
    textout_ex(target, font, data->title, self->abs_x + 11, self->abs_y, title_col, theme.bg);
}

static void groupbox_do_layout(Widget *self) {
    /* Adjust client area for children */
    int th = text_height(font);
    self->padding_top = th + 4;
    self->padding_left = 6;
    self->padding_right = 6;
    self->padding_bottom = 6;
}

Widget *groupbox_create(const char *title, int x, int y, int w, int h) {
    Widget *wid = widget_create(WIDGET_BASE, x, y, w, h);
    GroupBoxData *data = (GroupBoxData *)calloc(1, sizeof(GroupBoxData));
    strncpy(data->title, title, MAX_TEXT_LEN - 1);
    wid->extra = data;
    wid->draw = groupbox_draw;
    wid->do_layout = groupbox_do_layout;
    wid->layout = LAYOUT_VERTICAL;
    wid->spacing = 4;
    int th = text_height(font);
    widget_set_padding(wid, th + 4, 6, 6, 6);
    return wid;
}

void groupbox_set_title(Widget *w, const char *title) {
    GroupBoxData *data = (GroupBoxData *)w->extra;
    strncpy(data->title, title, MAX_TEXT_LEN - 1);
    w->dirty = 1;
}
