
#include "gui_colorpicker.h"
#include "gui_widget.h"
#include "gui_slider.h"
#include "gui_utils.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

static void colorpicker_update_preview(Widget *self) {
    ColorPickerData *data = (ColorPickerData *)self->extra;
    if (!data) return;
    data->preview_color = makecol(data->red, data->green, data->blue);
}

static void colorpicker_draw(Widget *self, BITMAP *target) {
    ColorPickerData *data = (ColorPickerData *)self->extra;
    if (!data) return;

    int x = self->abs_x;
    int y = self->abs_y;
    int w = self->w;
    int h = self->h;

    /* Background */
    rectfill(target, x, y, x + w - 1, y + h - 1, theme.bg);
    rect(target, x, y, x + w - 1, y + h - 1, theme.border);

    /* Preview rectangle */
    int prev_x = x + w - 60;
    int prev_y = y + 8;
    int prev_w = 50;
    int prev_h = h - 16;
    rectfill(target, prev_x, prev_y, prev_x + prev_w - 1, prev_y + prev_h - 1, data->preview_color);
    rect(target, prev_x, prev_y, prev_x + prev_w - 1, prev_y + prev_h - 1, theme.border);

    /* Slider area width */
    int slider_area_w = w - 80;
    int bar_h = 14;
    int bar_y_spacing = (h - 16) / 3;
    if (bar_y_spacing < bar_h + 4) bar_y_spacing = bar_h + 4;

    /* R slider */
    int ry = y + 8;
    int bar_x = x + 24;
    char label_buf[16];

    /* Red */
    textout_ex(target, font, "R:", x + 4, ry + (bar_h - text_height(font)) / 2, makecol(200, 50, 50), -1);
    /* Bar background */
    rectfill(target, bar_x, ry, bar_x + slider_area_w - 1, ry + bar_h - 1, makecol(40, 40, 40));
    /* Bar fill */
    if (data->red > 0) {
        int fill_w = (data->red * (slider_area_w - 2)) / 255;
        rectfill(target, bar_x + 1, ry + 1, bar_x + 1 + fill_w, ry + bar_h - 2, makecol(data->red, 0, 0));
    }
    rect(target, bar_x, ry, bar_x + slider_area_w - 1, ry + bar_h - 1, theme.border);
    /* Thumb */
    int thumb_x = bar_x + (data->red * (slider_area_w - 8)) / 255;
    rectfill(target, thumb_x, ry - 1, thumb_x + 7, ry + bar_h, theme.button_face);
    rect(target, thumb_x, ry - 1, thumb_x + 7, ry + bar_h, theme.border);
    sprintf(label_buf, "%d", data->red);
    textout_ex(target, font, label_buf, bar_x + slider_area_w + 4, ry + (bar_h - text_height(font)) / 2, theme.fg, -1);

    /* Green */
    int gy = ry + bar_y_spacing;
    textout_ex(target, font, "G:", x + 4, gy + (bar_h - text_height(font)) / 2, makecol(50, 180, 50), -1);
    rectfill(target, bar_x, gy, bar_x + slider_area_w - 1, gy + bar_h - 1, makecol(40, 40, 40));
    if (data->green > 0) {
        int fill_w = (data->green * (slider_area_w - 2)) / 255;
        rectfill(target, bar_x + 1, gy + 1, bar_x + 1 + fill_w, gy + bar_h - 2, makecol(0, data->green, 0));
    }
    rect(target, bar_x, gy, bar_x + slider_area_w - 1, gy + bar_h - 1, theme.border);
    thumb_x = bar_x + (data->green * (slider_area_w - 8)) / 255;
    rectfill(target, thumb_x, gy - 1, thumb_x + 7, gy + bar_h, theme.button_face);
    rect(target, thumb_x, gy - 1, thumb_x + 7, gy + bar_h, theme.border);
    sprintf(label_buf, "%d", data->green);
    textout_ex(target, font, label_buf, bar_x + slider_area_w + 4, gy + (bar_h - text_height(font)) / 2, theme.fg, -1);

    /* Blue */
    int by = gy + bar_y_spacing;
    textout_ex(target, font, "B:", x + 4, by + (bar_h - text_height(font)) / 2, makecol(50, 80, 220), -1);
    rectfill(target, bar_x, by, bar_x + slider_area_w - 1, by + bar_h - 1, makecol(40, 40, 40));
    if (data->blue > 0) {
        int fill_w = (data->blue * (slider_area_w - 2)) / 255;
        rectfill(target, bar_x + 1, by + 1, bar_x + 1 + fill_w, by + bar_h - 2, makecol(0, 0, data->blue));
    }
    rect(target, bar_x, by, bar_x + slider_area_w - 1, by + bar_h - 1, theme.border);
    thumb_x = bar_x + (data->blue * (slider_area_w - 8)) / 255;
    rectfill(target, thumb_x, by - 1, thumb_x + 7, by + bar_h, theme.button_face);
    rect(target, thumb_x, by - 1, thumb_x + 7, by + bar_h, theme.border);
    sprintf(label_buf, "%d", data->blue);
    textout_ex(target, font, label_buf, bar_x + slider_area_w + 4, by + (bar_h - text_height(font)) / 2, theme.fg, -1);
}

static int colorpicker_get_channel_at(Widget *self, int my) {
    ColorPickerData *data = (ColorPickerData *)self->extra;
    (void)data;
    int h = self->h;
    int bar_h = 14;
    int bar_y_spacing = (h - 16) / 3;
    if (bar_y_spacing < bar_h + 4) bar_y_spacing = bar_h + 4;

    int ry = self->abs_y + 8;
    int gy = ry + bar_y_spacing;
    int by = gy + bar_y_spacing;

    if (my >= ry - 2 && my <= ry + bar_h + 2) return 0; /* Red */
    if (my >= gy - 2 && my <= gy + bar_h + 2) return 1; /* Green */
    if (my >= by - 2 && my <= by + bar_h + 2) return 2; /* Blue */
    return -1;
}

static int colorpicker_value_from_x(Widget *self, int mx) {
    int bar_x = self->abs_x + 24;
    int slider_area_w = self->w - 80;
    int val = ((mx - bar_x) * 255) / (slider_area_w - 1);
    if (val < 0) val = 0;
    if (val > 255) val = 255;
    return val;
}

static void colorpicker_on_mouse_down(Widget *self, Event *event) {
    ColorPickerData *data = (ColorPickerData *)self->extra;
    if (!data) return;
    int ch = colorpicker_get_channel_at(self, event->mouse_y);
    if (ch >= 0) {
        data->dragging = 1;
        data->drag_channel = ch;
        int val = colorpicker_value_from_x(self, event->mouse_x);
        if (ch == 0) data->red = val;
        else if (ch == 1) data->green = val;
        else data->blue = val;
        colorpicker_update_preview(self);
        widget_emit_simple(self, EVENT_CHANGE);
        self->dirty = 1;
        event->handled = 1;
    }
}

static void colorpicker_on_mouse_move(Widget *self, Event *event) {
    ColorPickerData *data = (ColorPickerData *)self->extra;
    if (!data || !data->dragging) return;
    int val = colorpicker_value_from_x(self, event->mouse_x);
    if (data->drag_channel == 0) data->red = val;
    else if (data->drag_channel == 1) data->green = val;
    else data->blue = val;
    colorpicker_update_preview(self);
    widget_emit_simple(self, EVENT_CHANGE);
    self->dirty = 1;
    event->handled = 1;
}

static void colorpicker_on_mouse_up(Widget *self, Event *event) {
    ColorPickerData *data = (ColorPickerData *)self->extra;
    if (!data) return;
    (void)event;
    data->dragging = 0;
}

Widget *colorpicker_create(int x, int y, int w, int h) {
    Widget *wid = widget_create(WIDGET_BASE, x, y, w, h);
    ColorPickerData *data = (ColorPickerData *)calloc(1, sizeof(ColorPickerData));
    data->red = 128;
    data->green = 128;
    data->blue = 128;
    data->drag_channel = -1;

    /* Assign extra BEFORE calling update_preview */
    wid->extra = data;

    colorpicker_update_preview(wid);

    wid->draw = colorpicker_draw;
    widget_on(wid, EVENT_MOUSE_DOWN, colorpicker_on_mouse_down);
    widget_on(wid, EVENT_MOUSE_MOVE, colorpicker_on_mouse_move);
    widget_on(wid, EVENT_MOUSE_UP, colorpicker_on_mouse_up);

    return wid;
}

void colorpicker_set_color(Widget *w, int r, int g, int b) {
    ColorPickerData *data = (ColorPickerData *)w->extra;
    if (!data) return;
    data->red = r < 0 ? 0 : (r > 255 ? 255 : r);
    data->green = g < 0 ? 0 : (g > 255 ? 255 : g);
    data->blue = b < 0 ? 0 : (b > 255 ? 255 : b);
    colorpicker_update_preview(w);
    w->dirty = 1;
}

void colorpicker_get_rgb(Widget *w, int *r, int *g, int *b) {
    ColorPickerData *data = (ColorPickerData *)w->extra;
    if (!data) { *r = *g = *b = 0; return; }
    *r = data->red;
    *g = data->green;
    *b = data->blue;
}

int colorpicker_get_color(Widget *w) {
    ColorPickerData *data = (ColorPickerData *)w->extra;
    if (!data) return 0;
    return data->preview_color;
}