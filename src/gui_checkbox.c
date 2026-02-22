#include "gui_checkbox.h"
#include "gui_widget.h"
#include "gui_utils.h"
#include <stdlib.h>
#include <string.h>

static void checkbox_draw(Widget *self, BITMAP *target) {
    CheckboxData *data = (CheckboxData *)self->extra;
    if (!data) return;

    int x = self->abs_x, y = self->abs_y;
    int box_y = y + (self->h - CHECKBOX_SIZE) / 2;

    /* Checkbox box */
    rectfill(target, x, box_y, x + CHECKBOX_SIZE - 1, box_y + CHECKBOX_SIZE - 1, theme.checkbox_bg);
    rect(target, x, box_y, x + CHECKBOX_SIZE - 1, box_y + CHECKBOX_SIZE - 1,
         self->focused ? theme.input_focus_border : theme.input_border);

    /* Hover highlight */
    if (self->hovered) {
        rect(target, x + 1, box_y + 1, x + CHECKBOX_SIZE - 2, box_y + CHECKBOX_SIZE - 2,
             theme.checkbox_check);
    }

    /* Check mark */
    if (data->checked) {
        int cx = x + 3, cy = box_y + 3;
        int cw = CHECKBOX_SIZE - 6;
        for (int i = 0; i < cw; i++) {
            int px, py;
            if (i < cw / 2) {
                px = cx + i;
                py = cy + i;
            } else {
                px = cx + i;
                py = cy + cw - 1 - i;
            }
            putpixel(target, px, py, theme.checkbox_check);
            putpixel(target, px, py + 1, theme.checkbox_check);
        }
    }

    /* Label text */
    int text_col = self->enabled ? theme.fg : theme.disabled_fg;
    textout_ex(target, font, data->text,
               x + CHECKBOX_SIZE + 6, y + (self->h - text_height(font)) / 2,
               text_col, -1);
}

Widget *checkbox_create(const char *text, int x, int y) {
    int w = CHECKBOX_SIZE + 6 + text_length(font, text) + 4;
    int h = CHECKBOX_SIZE > text_height(font) ? CHECKBOX_SIZE + 4 : text_height(font) + 4;

    Widget *wid = widget_create(WIDGET_CHECKBOX, x, y, w, h);
    CheckboxData *data = (CheckboxData *)calloc(1, sizeof(CheckboxData));
    strncpy(data->text, text, MAX_TEXT_LEN - 1);
    wid->extra = data;
    wid->draw = checkbox_draw;
    return wid;
}
