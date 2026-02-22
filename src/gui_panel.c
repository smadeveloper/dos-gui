#include "gui_panel.h"
#include "gui_widget.h"
#include "gui_utils.h"
#include <stdlib.h>

static void panel_draw(Widget *self, BITMAP *target) {
    rectfill(target, self->abs_x, self->abs_y,
             self->abs_x + self->w - 1, self->abs_y + self->h - 1, theme.bg);
}

Widget *panel_create(int x, int y, int w, int h) {
    Widget *wid = widget_create(WIDGET_PANEL, x, y, w, h);
    wid->draw = panel_draw;
    wid->layout = LAYOUT_VERTICAL;
    wid->spacing = 4;
    return wid;
}
