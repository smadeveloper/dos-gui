#include "gui_statusbar.h"
#include "gui_widget.h"
#include "gui_utils.h"
#include <stdlib.h>
#include <string.h>

static void statusbar_draw(Widget *self, BITMAP *target) {
    StatusbarData *data = (StatusbarData *)self->extra;
    rectfill(target, self->abs_x, self->abs_y,
             self->abs_x + self->w - 1, self->abs_y + STATUSBAR_HEIGHT - 1, theme.statusbar_bg);
    hline(target, self->abs_x, self->abs_y, self->abs_x + self->w - 1, theme.border);
    textout_ex(target, font, data->text,
        self->abs_x + 6, self->abs_y + (STATUSBAR_HEIGHT - text_height(font)) / 2, theme.statusbar_fg, -1);
    int rx = self->abs_x + self->w - 4;
    for (int i = data->section_count - 1; i >= 0; i--) {
        int sw = data->section_widths[i];
        vline(target, rx - sw, self->abs_y + 3, self->abs_y + STATUSBAR_HEIGHT - 4, theme.border);
        textout_ex(target, font, data->sections[i],
            rx - sw + 6, self->abs_y + (STATUSBAR_HEIGHT - text_height(font)) / 2, theme.statusbar_fg, -1);
        rx -= sw + 2;
    }
}

Widget *statusbar_create(void) {
    Widget *wid = widget_create(WIDGET_STATUSBAR, 0, 0, 100, STATUSBAR_HEIGHT);
    StatusbarData *data = (StatusbarData *)calloc(1, sizeof(StatusbarData));
    wid->extra = data;
    wid->draw = statusbar_draw;
    wid->fill = FILL_X;
    return wid;
}

void statusbar_set_text(Widget *w, const char *text) {
    StatusbarData *data = (StatusbarData *)w->extra;
    strncpy(data->text, text, MAX_TEXT_LEN - 1);
    w->dirty = 1;
}

void statusbar_add_section(Widget *w, int width) {
    StatusbarData *data = (StatusbarData *)w->extra;
    if (data->section_count < 8) {
        data->section_widths[data->section_count] = width;
        data->section_count++;
    }
}

void statusbar_set_section(Widget *w, int index, const char *text) {
    StatusbarData *data = (StatusbarData *)w->extra;
    if (index >= 0 && index < data->section_count) {
        strncpy(data->sections[index], text, MAX_TEXT_LEN - 1);
        w->dirty = 1;
    }
}
