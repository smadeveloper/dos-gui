#ifndef GUI_COLORPICKER_H
#define GUI_COLORPICKER_H

#include "gui_types.h"

typedef struct {
    int red, green, blue;
    int preview_color;
    int dragging;
    int drag_channel;
} ColorPickerData;

Widget *colorpicker_create(int x, int y, int w, int h);
void colorpicker_set_color(Widget *w, int r, int g, int b);
void colorpicker_get_rgb(Widget *w, int *r, int *g, int *b);
int colorpicker_get_color(Widget *w);

#endif /* GUI_COLORPICKER_H */