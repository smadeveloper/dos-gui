#include "gui_button.h"
#include "gui_widget.h"
#include "gui_utils.h"
#include <stdlib.h>
#include <string.h>

static void button_draw(Widget *self, BITMAP *target) {
    ButtonData *data = (ButtonData *)self->extra;
    if (!data) return;

    int x = self->abs_x, y = self->abs_y, w = self->w, h = self->h;
    int pressed = self->pressed;
    int hovered = self->hovered;

    /* Background */
    int bg_col;
    if (!self->enabled) bg_col = theme.button_face;
    else if (pressed) bg_col = theme.button_shadow;
    else if (hovered) bg_col = theme.button_highlight;
    else bg_col = theme.button_face;

    rectfill(target, x, y, x + w - 1, y + h - 1, bg_col);

    /* 3D border */
    if (pressed) {
        hline(target, x, y, x + w - 1, theme.button_shadow);
        vline(target, x, y, y + h - 1, theme.button_shadow);
        hline(target, x, y + h - 1, x + w - 1, theme.button_highlight);
        vline(target, x + w - 1, y, y + h - 1, theme.button_highlight);
    } else {
        hline(target, x, y, x + w - 1, theme.button_highlight);
        vline(target, x, y, y + h - 1, theme.button_highlight);
        hline(target, x, y + h - 1, x + w - 1, theme.button_shadow);
        vline(target, x + w - 1, y, y + h - 1, theme.button_shadow);
    }

    /* Text */
    int text_col = self->enabled ? theme.button_text : theme.disabled_fg;
    int off = pressed ? 1 : 0;
    int tx = x + (w - text_length(font, data->text)) / 2 + off;
    int ty = y + (h - text_height(font)) / 2 + off;
    textout_ex(target, font, data->text, tx, ty, text_col, -1);

    /* Focus indicator */
    if (self->focused) {
        rect(target, x + 3, y + 3, x + w - 4, y + h - 4, theme.input_focus_border);
    }
}

static void image_button_draw(Widget *self, BITMAP *target) {
    ButtonData *data = (ButtonData *)self->extra;
    if (!data) return;

    int x = self->abs_x, y = self->abs_y, w = self->w, h = self->h;
    int pressed = self->pressed;
    int hovered = self->hovered;

    int bg_col;
    if (data->is_toggle && data->toggled) bg_col = theme.button_shadow;
    else if (pressed) bg_col = theme.button_shadow;
    else if (hovered) bg_col = theme.button_highlight;
    else bg_col = theme.button_face;

    rectfill(target, x, y, x + w - 1, y + h - 1, bg_col);

    int is_pressed = pressed || (data->is_toggle && data->toggled);

    if (hovered || is_pressed) {
        if (is_pressed) {
            hline(target, x, y, x + w - 1, theme.button_shadow);
            vline(target, x, y, y + h - 1, theme.button_shadow);
            hline(target, x, y + h - 1, x + w - 1, theme.button_highlight);
            vline(target, x + w - 1, y, y + h - 1, theme.button_highlight);
        } else {
            hline(target, x, y, x + w - 1, theme.button_highlight);
            vline(target, x, y, y + h - 1, theme.button_highlight);
            hline(target, x, y + h - 1, x + w - 1, theme.button_shadow);
            vline(target, x + w - 1, y, y + h - 1, theme.button_shadow);
        }
    }

    BITMAP *icon = data->icon;
    if (pressed && data->icon_pressed) icon = data->icon_pressed;
    else if (hovered && data->icon_hover) icon = data->icon_hover;

    if (icon) {
        int off = is_pressed ? 1 : 0;
        int ix = x + (w - icon->w) / 2 + off;
        int iy = y + (h - icon->h) / 2 + off;
        masked_blit(icon, target, 0, 0, ix, iy, icon->w, icon->h);
    }
}

Widget *button_create(const char *text, int x, int y, int w, int h) {
    if (h <= 0) h = BUTTON_HEIGHT;
    if (w <= 0) w = text_length(font, text) + 20;

    Widget *wid = widget_create(WIDGET_BUTTON, x, y, w, h);
    ButtonData *data = (ButtonData *)calloc(1, sizeof(ButtonData));
    strncpy(data->text, text, MAX_TEXT_LEN - 1);
    wid->extra = data;
    wid->draw = button_draw;
    return wid;
}

Widget *image_button_create(BITMAP *icon, int x, int y, int w, int h) {
    Widget *wid = widget_create(WIDGET_IMAGE_BUTTON, x, y, w, h);
    ButtonData *data = (ButtonData *)calloc(1, sizeof(ButtonData));
    data->icon = icon;
    wid->extra = data;
    wid->draw = image_button_draw;
    return wid;
}
