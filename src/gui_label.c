#include "gui_label.h"
#include "gui_widget.h"
#include "gui_utils.h"
#include <stdlib.h>
#include <string.h>

static void label_draw(Widget *self, BITMAP *target) {
    LabelData *data = (LabelData *)self->extra;
    int col = self->enabled ? (data->text_color ? data->text_color : theme.fg) : theme.disabled_fg;
    if (data->text_align == ALIGN_CENTER) {
        textout_centre_ex(target, font, data->text,
            self->abs_x + self->w / 2, self->abs_y + (self->h - text_height(font)) / 2, col, -1);
    } else if (data->text_align == ALIGN_RIGHT) {
        textout_right_ex(target, font, data->text,
            self->abs_x + self->w - 2, self->abs_y + (self->h - text_height(font)) / 2, col, -1);
    } else {
        textout_ex(target, font, data->text,
            self->abs_x + 2, self->abs_y + (self->h - text_height(font)) / 2, col, -1);
    }
}

Widget *label_create(const char *text, int x, int y) {
    int tw = text_length(font, text) + 4;
    int th = text_height(font) + 4;
    Widget *w = widget_create(WIDGET_LABEL, x, y, tw, th);
    LabelData *data = (LabelData *)calloc(1, sizeof(LabelData));
    strncpy(data->text, text, MAX_TEXT_LEN - 1);
    data->text_align = ALIGN_LEFT;
    w->extra = data;
    w->draw = label_draw;
    return w;
}
